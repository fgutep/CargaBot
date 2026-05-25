#!/usr/bin/env python3
"""
CargaBot Serial Bridge — ROS2 ↔ ESP32
======================================
Compatible con firmware CargaBot v2 (sesión tuning 2025-05-10).

Cambios respecto al bridge original:
  - Puerto default /dev/ttyACM0 (ESP32-S3 en Linux)
  - stop limpio con {"stop":1} en shutdown (resetea integradores en la ESP32)
  - Publica velocidades lineales y angulares medidas en /odom (vx, vyaw)
  - Log de RPM y PWM en DEBUG para diagnóstico
  - Reconexión automática si se pierde el serial
"""

from lab1_msgs import msg
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from nav_msgs.msg import Odometry
import serial
import serial.tools.list_ports
import json
import threading
import math
import time


class CargaBotSerialBridge(Node):
    def __init__(self):
        super().__init__("cargabot_serial_bridge")

        # ── Parámetros ──────────────────────────────────────────────
        self.port     = self.declare_parameter('port',     '/dev/ttyACM0').value
        self.baudrate = self.declare_parameter('baudrate', 115200).value

        # ── Serial ──────────────────────────────────────────────────
        self.ser     = None
        self.ser_lock = threading.Lock()
        self._connect_serial()

        # ── Subscribers ─────────────────────────────────────────────
        self.cmd_vel_sub = self.create_subscription(
            Twist,
            'turtlebot_cmdVel',
            self.cmd_vel_callback,
            10
        )
        # ── Watchdog: auto-stop if no cmd_vel received ─────────
        self._last_cmd_time = time.time()
        self._watchdog_timeout = 0.5  # seconds
        self._watchdog_timer = self.create_timer(0.2, self._watchdog_cb)

        # ── Publishers ──────────────────────────────────────────────
        self.odom_pub    = self.create_publisher(Odometry, '/odom', 10)
        self.gui_pos_pub = self.create_publisher(Twist, '/turtlebot_position', 10)

        # ── Background serial reader ─────────────────────────────────
        self.running     = True
        self.read_thread = threading.Thread(target=self._serial_read_loop, daemon=True)
        self.read_thread.start()

        self.get_logger().info("CargaBot Serial Bridge started.")

    # ── CONEXIÓN SERIAL ─────────────────────────────────────────────

    def _connect_serial(self):
        """Intenta conectar al puerto serial. Reintenta cada 2s si falla."""
        while rclpy.ok():
            try:
                self.ser = serial.Serial(self.port, self.baudrate, timeout=1.0)
                time.sleep(2)  # ESP32 reset tras conexión
                self.get_logger().info(f"Connected to {self.port} at {self.baudrate} baud.")
                return
            except serial.SerialException as e:
                self.get_logger().warn(f"Serial connect failed: {e}. Retrying in 2s...")
                time.sleep(2)

    def _serial_write(self, d: dict):
        """Envía un dict como JSON newline-terminated. Thread-safe."""
        if not self.ser or not self.ser.is_open:
            return
        payload = (json.dumps(d) + '\n').encode('utf-8')
        try:
            with self.ser_lock:
                self.ser.write(payload)
        except serial.SerialException as e:
            self.get_logger().error(f"Serial write error: {e}")

    # ── CMD_VEL CALLBACK ────────────────────────────────────────────

    def cmd_vel_callback(self, msg: Twist):
        """
        Traduce Twist → JSON {"v": ..., "w": ...} para la ESP32.
        
        NOTA: el firmware niega v internamente para corregir la dirección
        física del robot. Aquí mandamos el v tal como lo publica ROS
        (positivo = adelante desde la perspectiva de ROS).
        """
        self._last_cmd_time = time.time()
        self._serial_write({"v": msg.linear.x, "w": msg.angular.z})

    # ── LOOP DE LECTURA SERIAL ──────────────────────────────────────

    def _serial_read_loop(self):
        """Lee telemetría JSON de la ESP32 continuamente."""
        while self.running and rclpy.ok():
            if not self.ser or not self.ser.is_open:
                time.sleep(0.5)
                continue
            try:
                if self.ser.in_waiting > 0:
                    line = self.ser.readline().decode('utf-8', errors='ignore').strip()
                    if line.startswith('{') and line.endswith('}'):
                        data = json.loads(line)
                        self._publish_odometry(data)
                        self._log_debug(data)
            except json.JSONDecodeError:
                pass
            except serial.SerialException as e:
                self.get_logger().warn(f"Serial read error: {e}. Reconnecting...")
                try:
                    self.ser.close()
                except Exception:
                    pass
                self._connect_serial()
            except Exception:
                pass

    # ── PUBLICACIÓN ODOMETRÍA ───────────────────────────────────────

    def _publish_odometry(self, data: dict):
        """
        Parsea el JSON de telemetría y publica:
          - /odom  (nav_msgs/Odometry) — pose + velocidades
          - /turtlebot_position (Twist) — para la GUI existente
        """
        x   = float(data.get('x',  0.0))
        y   = float(data.get('y',  0.0))
        th  = float(data.get('th', 0.0))

        # Velocidades medidas (del RPM filtrado)
        rpm_l = float(data.get('rpmL', 0.0))
        rpm_r = float(data.get('rpmR', 0.0))
        R = 0.04   # radio rueda [m]  — debe coincidir con firmware
        L = 0.20   # track width [m]  — debe coincidir con firmware

        v_l = (rpm_l / 60.0) * (2.0 * math.pi * R)
        v_r = (rpm_r / 60.0) * (2.0 * math.pi * R)
        vx  = (v_l + v_r) / 2.0
        wz  = (v_r - v_l) / L

        current_time = self.get_clock().now().to_msg()

        # ── Odometry ──
        odom = Odometry()
        odom.header.stamp    = current_time
        odom.header.frame_id = "odom"
        odom.child_frame_id  = "base_link"

        odom.pose.pose.position.x = x
        odom.pose.pose.position.y = y
        odom.pose.pose.position.z = 0.0

        # Cuaternión desde yaw
        odom.pose.pose.orientation.x = 0.0
        odom.pose.pose.orientation.y = 0.0
        odom.pose.pose.orientation.z = math.sin(th / 2.0)
        odom.pose.pose.orientation.w = math.cos(th / 2.0)

        # Velocidades en body frame
        odom.twist.twist.linear.x  = vx
        odom.twist.twist.angular.z = wz

        self.odom_pub.publish(odom)

        # ── GUI Twist ──
        gui = Twist()
        gui.linear.x = x
        gui.linear.y = y
        self.gui_pos_pub.publish(gui)

    # ── LOG DEBUG ───────────────────────────────────────────────────

    def _log_debug(self, data: dict):
        """Log RPM y PWM en nivel DEBUG — visible con --log-level debug."""
        self.get_logger().debug(
            f"L: rpm={data.get('rpmL',0):.1f} pwm={data.get('pwmL',0)} | "
            f"R: rpm={data.get('rpmR',0):.1f} pwm={data.get('pwmR',0)} | "
            f"mode={data.get('mode',0)}"
        )

    # ── SHUTDOWN ────────────────────────────────────────────────────

    def destroy_node(self):
        self.running = False
        if self.ser and self.ser.is_open:
            try:
                # {"stop":1} resetea integradores y PWM en la ESP32
                # más seguro que {"v":0,"w":0} que no limpia el estado PID
                with self.ser_lock:
                    self.ser.write(b'{"stop":1}\n')
                    time.sleep(0.1)
                    self.ser.close()
            except Exception:
                pass
        super().destroy_node()

    # ── WATCHDOG CALLBACK ───────────────────────────────────────────
    def _watchdog_cb(self):
        elapsed = time.time() - self._last_cmd_time
        if elapsed > self._watchdog_timeout:
            self._serial_write({"v": 0.0, "w": 0.0})


def main(args=None):
    rclpy.init(args=args)
    node = CargaBotSerialBridge()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()