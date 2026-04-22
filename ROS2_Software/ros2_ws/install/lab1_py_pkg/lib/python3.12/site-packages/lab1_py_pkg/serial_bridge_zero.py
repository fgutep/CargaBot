#!/usr/bin/env python3
"""
serial_bridge_zero.py
ROS2 node that bridges the ESP32 JSON serial protocol to ROS2 topics and services.

Publishers:
  /lift_tof/ranges/<sensor_name>  [sensor_msgs/Range]   — one topic per ToF sensor
  /lift_tof/lift_state             [std_msgs/String]      — JSON lift status

Services:
  /lift_tof/lift_control           [std_srvs/SetBool]    — True=up, False=down
  /lift_tof/lift_stop              [std_srvs/Trigger]    — stop immediately

Parameters (set via ROS2 params or launch file):
  serial_port     (str)   default: /dev/ttyUSB0
  baud_rate       (int)   default: 115200
  sensor_fov      (float) default: 0.436332  rad (~25 deg, VL53L0X spec)
  sensor_min_m    (float) default: 0.03       m
  sensor_max_m    (float) default: 2.0        m

Usage:
  ros2 run <your_package> serial_bridge_zero --ros-args \
      -p serial_port:=/dev/ttyUSB0 -p baud_rate:=115200

      serial_bridge_zero --ros-args -p serial_port:=/dev/ttyUSB0 -p baud_rate:=115200
"""

import json
import threading

import rclpy
from rclpy.node import Node
from rclpy.executors import MultiThreadedExecutor

import serial

from sensor_msgs.msg import Range
from std_msgs.msg import String
from std_srvs.srv import SetBool, Trigger


class SerialBridgeNode(Node):

    def __init__(self):
        super().__init__('serial_bridge_zero')

        # --- Parameters ---
        self.declare_parameter('serial_port',  '/dev/ttyUSB0')
        self.declare_parameter('baud_rate',    115200)
        self.declare_parameter('sensor_fov',   0.436332)   # radians
        self.declare_parameter('sensor_min_m', 0.03)
        self.declare_parameter('sensor_max_m', 2.0)

        port     = self.get_parameter('serial_port').value
        baud     = self.get_parameter('baud_rate').value
        self.fov     = self.get_parameter('sensor_fov').value
        self.min_r   = self.get_parameter('sensor_min_m').value
        self.max_r   = self.get_parameter('sensor_max_m').value

        # --- Serial port ---
        try:
            self.ser = serial.Serial(port, baud, timeout=1.0)
            self.get_logger().info(f'Serial open: {port} @ {baud}')
        except serial.SerialException as e:
            self.get_logger().fatal(f'Cannot open serial port: {e}')
            raise SystemExit(1)

        # --- Dynamic publisher registry (keyed by sensor name) ---
        self.range_pubs: dict[str, rclpy.publisher.Publisher] = {}

        # --- Lift state publisher ---
        self.lift_state_pub = self.create_publisher(
            String, '/lift_tof/lift_state', 10)

        # --- Lift control service: True = up, False = down ---
        self.lift_control_srv = self.create_service(
            SetBool,
            '/lift_tof/lift_control',
            self.handle_lift_control)

        # --- Lift stop service ---
        self.lift_stop_srv = self.create_service(
            Trigger,
            '/lift_tof/lift_stop',
            self.handle_lift_stop)

        # --- Serial read thread ---
        self._stop_event = threading.Event()
        self._serial_thread = threading.Thread(
            target=self._serial_reader, daemon=True)
        self._serial_thread.start()

        self.get_logger().info('serial_bridge_zero ready')

    # ------------------------------------------------------------------
    #  Serial reader (runs in background thread)
    # ------------------------------------------------------------------

    def _serial_reader(self):
        while not self._stop_event.is_set():
            try:
                raw = self.ser.readline().decode('utf-8', errors='replace').strip()
                if not raw:
                    continue
                self._dispatch(raw)
            except Exception as e:
                self.get_logger().warn(f'Serial read error: {e}')

    def _dispatch(self, raw: str):
        try:
            msg = json.loads(raw)
        except json.JSONDecodeError:
            self.get_logger().debug(f'Non-JSON line: {raw}')
            return

        msg_type = msg.get('type', '')

        if msg_type == 'sensor_data':
            self._handle_sensor_data(msg)

        elif msg_type == 'status':
            self.get_logger().info(f"[ESP32] {msg.get('message', '')}")

        elif msg_type == 'error':
            self.get_logger().error(
                f"[ESP32 ERROR] {msg.get('code', '?')}: {msg.get('message', '')}")

        elif msg_type == 'service_response':
            self.get_logger().debug(f'Service response: {msg}')

    # ------------------------------------------------------------------
    #  Sensor data → Range messages
    # ------------------------------------------------------------------

    def _handle_sensor_data(self, msg: dict):
        ranges = msg.get('ranges', {})
        stamp  = self.get_clock().now().to_msg()

        for sensor_name, value_mm in ranges.items():
            # Lazy-create publisher the first time we see a sensor name
            if sensor_name not in self.range_pubs:
                topic = f'/lift_tof/ranges/{sensor_name}'
                self.range_pubs[sensor_name] = self.create_publisher(
                    Range, topic, 10)
                self.get_logger().info(f'New range topic: {topic}')

            out                  = Range()
            out.header.stamp     = stamp
            out.header.frame_id  = sensor_name
            out.radiation_type   = Range.INFRARED
            out.field_of_view    = self.fov
            out.min_range        = self.min_r
            out.max_range        = self.max_r

            if value_mm is None:
                out.range = float('inf')   # OOB
            else:
                out.range = float(value_mm) / 1000.0  # mm → m

            self.range_pubs[sensor_name].publish(out)

        # Publish lift state as JSON string
        lift_msg        = String()
        lift_msg.data   = json.dumps({
            'running': msg.get('lift_running', False),
            'dir':     msg.get('lift_dir', 0),
            'limits':  msg.get('limit_switches', {})
        })
        self.lift_state_pub.publish(lift_msg)

    # ------------------------------------------------------------------
    #  Lift service handlers
    # ------------------------------------------------------------------

    def handle_lift_control(
            self,
            request: SetBool.Request,
            response: SetBool.Response) -> SetBool.Response:
        """True = up, False = down"""
        action = 'up' if request.data else 'down'
        cmd    = json.dumps({'cmd': 'lift', 'action': action}) + '\n'
        self.ser.write(cmd.encode())
        response.success = True
        response.message = f'lift {action} sent'
        self.get_logger().info(f'Lift command: {action}')
        return response

    def handle_lift_stop(
            self,
            request: Trigger.Request,
            response: Trigger.Response) -> Trigger.Response:
        cmd = json.dumps({'cmd': 'lift', 'action': 'stop'}) + '\n'
        self.ser.write(cmd.encode())
        response.success = True
        response.message = 'lift stop sent'
        self.get_logger().info('Lift stop command sent')
        return response

    # ------------------------------------------------------------------
    #  Cleanup
    # ------------------------------------------------------------------

    def destroy_node(self):
        self._stop_event.set()
        if self.ser.is_open:
            self.ser.close()
        super().destroy_node()


# ----------------------------------------------------------------------
#  Entry point
# ----------------------------------------------------------------------

def main(args=None):
    rclpy.init(args=args)
    node = SerialBridgeNode()
    executor = MultiThreadedExecutor()
    executor.add_node(node)
    try:
        executor.spin()
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()