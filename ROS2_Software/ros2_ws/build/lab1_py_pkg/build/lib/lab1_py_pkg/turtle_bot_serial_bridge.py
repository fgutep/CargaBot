#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from nav_msgs.msg import Odometry
import serial
import json
import threading
import math
import time

class TurtleBotSerialBridge(Node):
    def __init__(self):
        super().__init__("turtle_bot_serial_bridge")
        
        # --- Serial Configuration ---
        self.port = self.declare_parameter('port', '/dev/ttyUSB0').value
        self.baudrate = self.declare_parameter('baudrate', 115200).value
        
        try:
            self.ser = serial.Serial(self.port, self.baudrate, timeout=1.0)
            time.sleep(2)  # Wait for ESP32 to reset after connection
            self.get_logger().info(f"Connected to ESP32 on {self.port} at {self.baudrate} baud.")
        except serial.SerialException as e:
            self.get_logger().error(f"Failed to connect to serial port: {e}")
            self.ser = None

        # --- Subscribers ---
        # Listens to the teleop node
        self.cmd_vel_sub = self.create_subscription(
            Twist, 
            'turtlebot_cmdVel', 
            self.cmd_vel_callback, 
            10
        )

        # --- Publishers ---
        # Standard ROS 2 Odometry
        self.odom_pub = self.create_publisher(Odometry, '/odom', 10)
        
        # Custom Twist publisher strictly for the turtle_bot_interface GUI
        self.gui_pos_pub = self.create_publisher(Twist, '/turtlebot_position', 10)

        # --- Background Serial Reader ---
        if self.ser:
            self.running = True
            self.read_thread = threading.Thread(target=self.serial_read_loop, daemon=True)
            self.read_thread.start()

    def cmd_vel_callback(self, msg):
        """ Translates Twist to JSON and sends it to the ESP32. """
        if not self.ser:
            return
            
        v = msg.linear.x
        w = msg.angular.z
        
        payload = json.dumps({"v": v, "w": w}) + '\n'
        
        try:
            self.ser.write(payload.encode('utf-8'))
        except Exception as e:
            self.get_logger().error(f"Serial write error: {e}")

    def serial_read_loop(self):
        """ Continuously reads telemetry from ESP32 and publishes ROS messages. """
        while self.running and rclpy.ok():
            try:
                if self.ser.in_waiting > 0:
                    line = self.ser.readline().decode('utf-8').strip()
                    
                    if line.startswith('{') and line.endswith('}'):
                        data = json.loads(line)
                        self.publish_odometry(data)
                        
            except Exception as e:
                pass # Ignore malformed JSON or read errors during fast streaming

    def publish_odometry(self, data):
        x  = float(data.get('x',  0.0))
        y  = float(data.get('y',  0.0))
        th = float(data.get('th', 0.0))

        current_time = self.get_clock().now().to_msg()

        odom_msg = Odometry()
        odom_msg.header.stamp = current_time
        odom_msg.header.frame_id = "odom"
        odom_msg.child_frame_id = "base_link"

        odom_msg.pose.pose.position.x = x
        odom_msg.pose.pose.position.y = y
        odom_msg.pose.pose.position.z = 0.0

        odom_msg.pose.pose.orientation.x = 0.0
        odom_msg.pose.pose.orientation.y = 0.0
        odom_msg.pose.pose.orientation.z = math.sin(th / 2.0)
        odom_msg.pose.pose.orientation.w = math.cos(th / 2.0)

        self.odom_pub.publish(odom_msg)

        gui_msg = Twist()
        gui_msg.linear.x = x
        gui_msg.linear.y = y
        self.gui_pos_pub.publish(gui_msg)

    def destroy_node(self):
        self.running = False
        if self.ser:
            # Stop the motors before shutting down
            try:
                self.ser.write(b'{"v":0.0, "w":0.0}\n')
                self.ser.close()
            except:
                pass
        super().destroy_node()

def main(args=None):
    rclpy.init(args=args)
    node = TurtleBotSerialBridge()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()