#!/usr/bin/env python3
import sys
import termios
import tty
import select
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from lab1_msgs.msg import RobotAction
from std_srvs.srv import SetBool, Trigger
import time
import threading


class TurtleBotTeleopNode(Node):
    def __init__(self):
        super().__init__("turtle_bot_teleop")

        self.vel_publisher = self.create_publisher(Twist, "turtlebot_cmdVel", 10)
        self.action_publisher = self.create_publisher(RobotAction, "/turtlebot_actions", 10)

        # --- Lift service clients ---
        self.lift_control_client = self.create_client(SetBool, '/lift_tof/lift_control')
        self.lift_stop_client    = self.create_client(Trigger,  '/lift_tof/lift_stop')

        self.get_logger().info("Turtle Bot Teleop has been started.")
        self.get_logger().info("WASD: drive  |  R/F: lift up/down  |  E: lift stop  |  Q: quit")

        self.settings = termios.tcgetattr(sys.stdin)

        self.action_descriptions = {
            'w': 'Adelante',
            's': 'Atrás',
            'a': 'Girar izquierda',
            'd': 'Girar derecha',
            'stop': 'Detenido',
        }

        self.current_action  = 'stop'
        self.last_logged_action = None
        self.running = True

    # ------------------------------------------------------------------
    #  Lift helpers (non-blocking — fire and forget)
    # ------------------------------------------------------------------

    def _send_lift_control(self, go_up: bool):
        if not self.lift_control_client.service_is_ready():
            self.get_logger().warn('lift_control service not available')
            return
        req = SetBool.Request()
        req.data = go_up
        future = self.lift_control_client.call_async(req)
        future.add_done_callback(
            lambda f: self.get_logger().info(
                f'Lift {"up" if go_up else "down"}: {f.result().message}'
                if f.result() else 'Lift control call failed'
            )
        )

    def _send_lift_stop(self):
        if not self.lift_stop_client.service_is_ready():
            self.get_logger().warn('lift_stop service not available')
            return
        future = self.lift_stop_client.call_async(Trigger.Request())
        future.add_done_callback(
            lambda f: self.get_logger().info(
                f'Lift stop: {f.result().message}'
                if f.result() else 'Lift stop call failed'
            )
        )

    # ------------------------------------------------------------------
    #  Action publisher thread (unchanged)
    # ------------------------------------------------------------------

    def publish_actions_thread(self):
        while self.running and rclpy.ok():
            if self.current_action in self.action_descriptions:
                action_msg = RobotAction()
                action_msg.action = (
                    f"{self.current_action}|"
                    f"{self.action_descriptions[self.current_action]}"
                )
                action_msg.timestamp = time.time()
                self.action_publisher.publish(action_msg)

                if self.last_logged_action != self.current_action:
                    self.get_logger().info(
                        self.action_descriptions[self.current_action]
                    )
                    self.last_logged_action = self.current_action
            time.sleep(0.1)

    # ------------------------------------------------------------------
    #  Key reader
    # ------------------------------------------------------------------

    def get_key(self):
        tty.setraw(sys.stdin.fileno())
        rlist, _, _ = select.select([sys.stdin], [], [], 0.1)
        key = sys.stdin.read(1) if rlist else ''
        termios.tcsetattr(sys.stdin, termios.TCSADRAIN, self.settings)
        return key

    # ------------------------------------------------------------------
    #  Main loop
    # ------------------------------------------------------------------

    def run(self):
        action_thread = threading.Thread(
            target=self.publish_actions_thread, daemon=True
        )
        action_thread.start()

        try:
            last_key_time = time.time()
            key_timeout   = 0.15

            while rclpy.ok():
                key          = self.get_key()
                twist        = Twist()
                current_time = time.time()

                if key:
                    last_key_time = current_time

                    # --- Drive ---
                    if key == 'w':
                        twist.linear.x  =  1.5
                        self.current_action = 'w'

                    elif key == 's':
                        twist.linear.x  = -1.5
                        self.current_action = 's'

                    elif key == 'a':
                        twist.angular.z =  1.5
                        self.current_action = 'a'

                    elif key == 'd':
                        twist.angular.z = -1.5
                        self.current_action = 'd'

                    elif key == ' ':
                        self.current_action = 'stop'

                    # --- Lift ---
                    elif key == 'r':
                        self.get_logger().info('Lift: up')
                        self._send_lift_control(True)

                    elif key == 'f':
                        self.get_logger().info('Lift: down')
                        self._send_lift_control(False)

                    elif key == 'e':
                        self.get_logger().info('Lift: stop')
                        self._send_lift_stop()

                    # --- Quit ---
                    elif key in ('q', '\x03'):
                        self.get_logger().info('Saliendo...')
                        break

                    self.vel_publisher.publish(twist)

                else:
                    # Auto-stop after timeout
                    if (current_time - last_key_time) > key_timeout:
                        if self.current_action != 'stop':
                            self.vel_publisher.publish(Twist())
                            self.current_action = 'stop'

        finally:
            self.running = False
            termios.tcsetattr(sys.stdin, termios.TCSADRAIN, self.settings)
            self.vel_publisher.publish(Twist())


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