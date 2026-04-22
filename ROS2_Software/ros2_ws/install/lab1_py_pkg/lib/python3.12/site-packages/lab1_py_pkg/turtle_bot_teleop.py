#!/usr/bin/env python3
import sys
import termios
import tty
import select
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from lab1_msgs.msg import RobotAction
import time
import threading

class TurtleBotTeleopNode(Node):
    def __init__(self):
        super().__init__("turtle_bot_teleop")
        
        
        self.vel_publisher = self.create_publisher(Twist, "turtlebot_cmdVel", 10)
        
        
        self.action_publisher = self.create_publisher(RobotAction, "/turtlebot_actions", 10)
        
        self.get_logger().info("Turtle Bot Teleop has been started.")
        self.get_logger().info("Use WASD keys to move the robot, Q to quit.")
        self.get_logger().info("Release keys to stop the robot automatically.")
        
        
        self.settings = termios.tcgetattr(sys.stdin)
        
        
        self.action_descriptions = {
            'w': 'Adelante',
            's': 'Atrás',
            'a': 'Girar izquierda',
            'd': 'Girar derecha',
            'stop': 'Detenido'
        }
        
        
        self.current_action = 'stop'
        self.last_logged_action = None
        self.running = True
        
    def get_key(self):
        tty.setraw(sys.stdin.fileno())
        rlist, _, _ = select.select([sys.stdin], [], [], 0.1)
        if rlist:
            key = sys.stdin.read(1)
        else:
            key = ''
        termios.tcsetattr(sys.stdin, termios.TCSADRAIN, self.settings)
        return key
    
    def publish_actions_thread(self):
        
        while self.running and rclpy.ok():
            if self.current_action in self.action_descriptions:
                action_msg = RobotAction()
                action_msg.action = f"{self.current_action}|{self.action_descriptions[self.current_action]}"
                action_msg.timestamp = time.time()
                self.action_publisher.publish(action_msg)
                
                
                if self.last_logged_action != self.current_action:
                    self.get_logger().info(f"{self.action_descriptions[self.current_action]}")
                    self.last_logged_action = self.current_action
            
            time.sleep(0.1)  
    
    def run(self):
        
        action_thread = threading.Thread(target=self.publish_actions_thread, daemon=True)
        action_thread.start()
        
        try:
            last_key_time = time.time()
            key_timeout = 0.15
            
            while rclpy.ok():
                key = self.get_key()
                twist = Twist()
                current_time = time.time()
                
                if key:
                    last_key_time = current_time
                    
                    if key == 'w':
                        twist.linear.x = 0.5
                        twist.angular.z = 0.0
                        self.current_action = 'w'
                        
                    elif key == 's':
                        twist.linear.x = -0.5
                        twist.angular.z = 0.0
                        self.current_action = 's'
                        
                    elif key == 'a':
                        twist.linear.x = 0.0
                        twist.angular.z = 0.5
                        self.current_action = 'a'
                        
                    elif key == 'd':
                        twist.linear.x = 0.0
                        twist.angular.z = -0.5
                        self.current_action = 'd'
                        
                    elif key == ' ':
                        twist.linear.x = 0.0
                        twist.angular.z = 0.0
                        self.current_action = 'stop'
                        
                    elif key == 'q':
                        self.get_logger().info("Saliendo...")
                        break
                        
                    elif key == '\x03':
                        break
                    
                    self.vel_publisher.publish(twist)
                    
                else:
                    # Si pasó el timeout sin tecla, detener
                    if (current_time - last_key_time) > key_timeout:
                        if self.current_action != 'stop':
                            twist.linear.x = 0.0
                            twist.angular.z = 0.0
                            self.vel_publisher.publish(twist)
                            self.current_action = 'stop'
                
        finally:
            self.running = False
            termios.tcsetattr(sys.stdin, termios.TCSADRAIN, self.settings)
            twist = Twist()
            self.vel_publisher.publish(twist)

def main(args=None):
    rclpy.init(args=args)
    node = TurtleBotTeleopNode()
    
    try:
        node.run()
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == "__main__":
    main()