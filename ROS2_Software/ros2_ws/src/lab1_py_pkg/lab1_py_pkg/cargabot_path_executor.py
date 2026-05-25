#!/usr/bin/env python3
"""
CargaBot Path Executor v1
=========================
Bridges VisionApp path planning → MotionController commands.

Flow:
  VisionApp publishes Path on /cargabot/global_path
  VisionApp publishes PoseStamped on /cargabot/cmd_goto (lookahead)
  This node converts those into sequential execute_req JSON commands.

Subscriptions:
  /cargabot/global_path   (nav_msgs/Path)       — full A* path from VisionApp
  /cargabot/cmd_goto      (PoseStamped)          — lookahead target point
  /cargabot/execute_res   (std_msgs/String)      — response from MotionController
  /odom                   (nav_msgs/Odometry)    — current robot pose

Publications:
  /cargabot/execute_req   (std_msgs/String)      — JSON commands to MotionController

Modes:
  WAYPOINT mode (default): receives full path, sends waypoints command
  GOTO mode:               receives single cmd_goto, sends goto command

The executor handles heading-awareness: if the goal is behind the robot,
the MotionController's goto already does rotate→move→rotate, but we
also support a pure-pursuit style where we feed one lookahead at a time.

Usage:
  ros2 run <pkg> cargabot_path_executor
  # or standalone:
  python3 cargabot_path_executor.py
"""

import json
import math
import threading
import time
from enum import Enum, auto

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import PoseStamped
from nav_msgs.msg import Path, Odometry
from std_msgs.msg import String


def normalize_angle(a: float) -> float:
    while a > math.pi:
        a -= 2 * math.pi
    while a < -math.pi:
        a += 2 * math.pi
    return a


class ExecutorState(Enum):
    IDLE = auto()
    EXECUTING = auto()
    WAITING_RESPONSE = auto()


class CargaBotPathExecutor(Node):

    def __init__(self):
        super().__init__("cargabot_path_executor")

        # ── Parameters ──────────────────────────────────────────────
        self.declare_parameter('mode', 'goto')  # 'goto' or 'waypoints'
        self.declare_parameter('waypoint_skip', 3)  # skip every N waypoints to reduce commands
        self.declare_parameter('arrival_tolerance_m', 0.03)  # 3 cm
        self.declare_parameter('goal_timeout', 30.0)  # seconds per goto command
        self.declare_parameter('min_cmd_interval', 0.3)  # throttle cmd_goto messages

        self._mode = self.get_parameter('mode').value
        self._wp_skip = self.get_parameter('waypoint_skip').value
        self._arrival_tol = self.get_parameter('arrival_tolerance_m').value
        self._goal_timeout = self.get_parameter('goal_timeout').value
        self._min_cmd_interval = self.get_parameter('min_cmd_interval').value

        # ── State ───────────────────────────────────────────────────
        self._state = ExecutorState.IDLE
        self._state_lock = threading.Lock()
        self._last_response: dict = {}
        self._response_event = threading.Event()

        # Robot pose from odom
        self._pose_lock = threading.Lock()
        self._x = 0.0
        self._y = 0.0
        self._th = 0.0
        self._has_odom = False

        # Throttle for cmd_goto
        self._last_cmd_time = 0.0

        # Current path being executed (for waypoints mode)
        self._active_path: list = []
        self._path_lock = threading.Lock()

        # ── Publishers ──────────────────────────────────────────────
        self._req_pub = self.create_publisher(
            String, '/cargabot/execute_req', 10)

        # ── Subscribers ─────────────────────────────────────────────
        self.create_subscription(
            Odometry, '/odom', self._odom_cb, 10)
        self.create_subscription(
            String, '/cargabot/execute_res', self._response_cb, 10)
        self.create_subscription(
            PoseStamped, '/cargabot/cmd_goto', self._goto_cb, 10)
        self.create_subscription(
            Path, '/cargabot/global_path', self._path_cb, 10)

        self.get_logger().info(
            f"CargaBot Path Executor ready\n"
            f"  mode             = {self._mode}\n"
            f"  waypoint_skip    = {self._wp_skip}\n"
            f"  arrival_tol      = {self._arrival_tol} m\n"
            f"  goal_timeout     = {self._goal_timeout} s\n"
            f"  min_cmd_interval = {self._min_cmd_interval} s"
        )

    # ── Odometry ────────────────────────────────────────────────────

    def _odom_cb(self, msg: Odometry):
        q = msg.pose.pose.orientation
        siny = 2.0 * (q.w * q.z + q.x * q.y)
        cosy = 1.0 - 2.0 * (q.y * q.y + q.z * q.z)
        with self._pose_lock:
            self._x = msg.pose.pose.position.x
            self._y = msg.pose.pose.position.y
            self._th = math.atan2(siny, cosy)
            self._has_odom = True

    def _pose(self):
        with self._pose_lock:
            return self._x, self._y, self._th

    # ── Response from MotionController ──────────────────────────────

    def _response_cb(self, msg: String):
        try:
            resp = json.loads(msg.data)
        except json.JSONDecodeError:
            self.get_logger().warn(f"Bad response JSON: {msg.data}")
            return

        self._last_response = resp
        self._response_event.set()

        success = resp.get('success', False)
        message = resp.get('message', '')
        self.get_logger().info(
            f"[MC response] success={success} msg={message}")

        with self._state_lock:
            if self._state == ExecutorState.WAITING_RESPONSE:
                self._state = ExecutorState.IDLE

    # ── cmd_goto callback (single lookahead point) ──────────────────

    def _goto_cb(self, msg: PoseStamped):
        """
        VisionApp publishes lookahead targets here.
        In 'goto' mode, we forward each as a goto command.
        Coordinates come in meters (VisionApp converts cm → m).
        """
        if self._mode != 'goto':
            return

        # Throttle: don't spam the MC
        now = time.time()
        if now - self._last_cmd_time < self._min_cmd_interval:
            return

        with self._state_lock:
            if self._state != ExecutorState.IDLE:
                return  # still executing previous command

        tx = msg.pose.position.x  # meters
        ty = msg.pose.position.y  # meters

        if not self._has_odom:
            self.get_logger().warn("No odom yet, skipping goto")
            return

        # Check if we're already close enough
        x, y, _ = self._pose()
        dist = math.hypot(tx - x, ty - y)
        if dist < self._arrival_tol:
            return  # already there

        self._last_cmd_time = now
        self._send_goto(tx, ty)

    # ── global_path callback (full A* path) ─────────────────────────

    def _path_cb(self, msg: Path):
        """
        VisionApp publishes the full A* path here.
        In 'waypoints' mode, we decimate and send as a waypoints command.
        Coordinates are in meters (VisionApp converts cm/100).
        """
        if not msg.poses:
            return

        # Extract waypoints, skip every N to reduce command count
        waypoints = []
        for i, pose_stamped in enumerate(msg.poses):
            if i % self._wp_skip == 0 or i == len(msg.poses) - 1:
                waypoints.append([
                    pose_stamped.pose.position.x,
                    pose_stamped.pose.position.y
                ])

        # Always include the last point
        last = msg.poses[-1]
        last_wp = [last.pose.position.x, last.pose.position.y]
        if waypoints[-1] != last_wp:
            waypoints.append(last_wp)

        self.get_logger().info(
            f"Path received: {len(msg.poses)} poses → "
            f"{len(waypoints)} waypoints (skip={self._wp_skip})")

        if self._mode == 'waypoints':
            # Send entire decimated path as waypoints command
            threading.Thread(
                target=self._execute_waypoints,
                args=(waypoints,),
                daemon=True
            ).start()
        else:
            # In goto mode, store for reference but let cmd_goto drive
            with self._path_lock:
                self._active_path = waypoints

    # ── Command senders ─────────────────────────────────────────────

    def _send_command(self, cmd: dict) -> bool:
        """Send a command and wait for response. Returns success."""
        with self._state_lock:
            if self._state != ExecutorState.IDLE:
                self.get_logger().warn(
                    f"Cannot send command, state={self._state.name}")
                return False
            self._state = ExecutorState.WAITING_RESPONSE

        self._response_event.clear()
        msg = String()
        msg.data = json.dumps(cmd)
        self._req_pub.publish(msg)
        self.get_logger().info(f"[send] {json.dumps(cmd)}")

        # Wait for response with timeout
        got_response = self._response_event.wait(timeout=self._goal_timeout)

        if not got_response:
            self.get_logger().error(
                f"Command timed out after {self._goal_timeout}s: {cmd}")
            with self._state_lock:
                self._state = ExecutorState.IDLE
            # Send stop for safety
            stop_msg = String()
            stop_msg.data = json.dumps({"cmd": "stop"})
            self._req_pub.publish(stop_msg)
            return False

        return self._last_response.get('success', False)

    def _send_goto(self, x: float, y: float, heading: float = None):
        """Send a single goto command (non-blocking, fires in thread)."""
        cmd = {"cmd": "goto", "x": x, "y": y}
        if heading is not None:
            cmd["heading"] = heading
        threading.Thread(
            target=self._send_command,
            args=(cmd,),
            daemon=True
        ).start()

    def _execute_waypoints(self, waypoints: list):
        """Execute a sequence of waypoints via the MC waypoints command."""
        with self._state_lock:
            if self._state != ExecutorState.IDLE:
                self.get_logger().warn(
                    "Cannot execute waypoints, already busy")
                return

        self.get_logger().info(
            f"Executing {len(waypoints)} waypoints...")

        # Option A: Send all at once via MC's waypoints command
        # This is simpler but gives less control
        cmd = {
            "cmd": "waypoints",
            "waypoints": waypoints
        }
        success = self._send_command(cmd)

        if success:
            self.get_logger().info("Waypoints execution complete!")
        else:
            self.get_logger().error("Waypoints execution failed")

    # ── Shutdown ────────────────────────────────────────────────────

    def destroy_node(self):
        # Send stop on shutdown
        stop_msg = String()
        stop_msg.data = json.dumps({"cmd": "stop"})
        self._req_pub.publish(stop_msg)
        super().destroy_node()


# ── Entry point ─────────────────────────────────────────────────────

def main(args=None):
    rclpy.init(args=args)
    node = CargaBotPathExecutor()
    executor = rclpy.executors.MultiThreadedExecutor()
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