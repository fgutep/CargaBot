#!/usr/bin/env python3
"""
CargaBot Safety Guardian v1
============================
Monitors ToF sensors and overrides velocity commands to prevent collisions.

Sensor layout:
  #1  Forward Upper  (blocked when lift at top)
  #2  Behind
  #3  Forward Lower  (blocked when lift at bottom)
  #4  Left
  #5  Right

Subscriptions:
  /lift_tof/ranges/sensor_{1..5}  (std_msgs/Float32 or similar)
  /lift_tof/lift_state            (std_msgs/String — JSON)
  turtlebot_cmdVel                (geometry_msgs/Twist — from MC)

Publications:
  turtlebot_cmdVel_safe           (geometry_msgs/Twist — to serial bridge)
  /cargabot/safety_status         (std_msgs/String — JSON status)
  /cargabot/execute_req           (std_msgs/String — emergency stop/backup)

Architecture:
  MotionController → turtlebot_cmdVel → [Guardian] → turtlebot_cmdVel_safe → SerialBridge

  The guardian sits between MC and serial bridge. It can:
    1. Pass through commands (safe)
    2. Zero out commands (obstacle detected, stop)
    3. Override with backup maneuver (too close, must retreat)
    4. Ignore frozen sensors (health watchdog)

Sensor health:
  Each sensor has a watchdog. If a sensor's value hasn't changed for
  SENSOR_FREEZE_TIMEOUT seconds, it's marked as frozen/unreliable
  and excluded from decisions. This prevents a stuck sensor from
  either permanently blocking movement or permanently allowing it.

Usage:
  python3 cargabot_safety_guardian.py

  IMPORTANT: Rename topics so the chain is:
    MC publishes to       turtlebot_cmdVel
    Guardian subscribes   turtlebot_cmdVel
    Guardian publishes    turtlebot_cmdVel_safe
    SerialBridge subs     turtlebot_cmdVel_safe  (change its subscription!)
"""

import json
import math
import time
import threading
from enum import Enum, auto
from collections import deque
from typing import Optional, Dict

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from std_msgs.msg import String, Float32


# ── Safety states ────────────────────────────────────────────────────

class SafetyState(Enum):
    CLEAR = auto()          # All clear, pass through
    CAUTION = auto()        # Obstacle nearby, reduce speed
    BLOCKED_FRONT = auto()  # Front blocked, stop forward motion
    BLOCKED_REAR = auto()   # Rear blocked, stop backward motion
    BLOCKED_LEFT = auto()   # Left blocked, stop left rotation
    BLOCKED_RIGHT = auto()  # Right blocked, stop right rotation
    EMERGENCY = auto()      # Too close on front, need backup maneuver
    BACKUP = auto()         # Currently executing backup maneuver


# ── Sensor health tracker ────────────────────────────────────────────

class SensorHealth:
    """Tracks a single ToF sensor's health and value."""

    def __init__(self, name: str, freeze_timeout: float = 3.0,
                 history_size: int = 5):
        self.name = name
        self.freeze_timeout = freeze_timeout

        self.value: Optional[float] = None          # latest reading (meters)
        self.last_update: float = 0.0                # time of last message
        self.last_change: float = 0.0                # time value actually changed
        self.prev_value: Optional[float] = None
        self.healthy: bool = False                   # has received at least one msg
        self.frozen: bool = False                    # value hasn't changed in a while
        self.history: deque = deque(maxlen=history_size)

        # Minimum change to count as "different" (avoid float noise)
        self.change_epsilon = 0.005  # 5mm

    def update(self, value: float):
        now = time.time()
        self.last_update = now
        self.healthy = True

        # Check if value actually changed
        if self.prev_value is None:
            self.last_change = now
        elif abs(value - self.prev_value) > self.change_epsilon:
            self.last_change = now
            self.frozen = False

        self.prev_value = self.value
        self.value = value
        self.history.append((now, value))

        # Check freeze
        if now - self.last_change > self.freeze_timeout:
            self.frozen = True

    def check_timeout(self, now: float, msg_timeout: float = 2.0):
        """Call periodically. Mark unhealthy if no messages received."""
        if self.last_update == 0.0:
            self.healthy = False
            return
        if now - self.last_update > msg_timeout:
            self.healthy = False

    def is_reliable(self) -> bool:
        """Sensor is reliable only if healthy AND not frozen."""
        return self.healthy and not self.frozen

    def get_filtered(self) -> Optional[float]:
        """Return median of recent readings, or None if unreliable."""
        if not self.is_reliable():
            return None
        if len(self.history) < 2:
            return self.value
        values = [v for _, v in self.history]
        values.sort()
        return values[len(values) // 2]


# ── Safety Guardian Node ─────────────────────────────────────────────

class SafetyGuardian(Node):

    # Distances in CENTIMETERS (sensors report meters, we convert)
    EMERGENCY_DIST_CM = 8.0     # Too close — must backup
    STOP_DIST_CM = 16.0         # Stop forward/backward motion
    CAUTION_DIST_CM = 25.0      # Reduce speed
    SPEED_REDUCE_FACTOR = 0.4   # Speed multiplier in caution zone

    # Backup maneuver params
    BACKUP_SPEED = -0.10        # m/s backward
    BACKUP_DURATION = 1.2       # seconds
    BACKUP_ROTATE_SPEED = 0.4   # rad/s
    BACKUP_ROTATE_DURATION = 1.5  # seconds (~85 degrees)

    # Sensor health params
    SENSOR_FREEZE_TIMEOUT = 3.0   # seconds without value change
    SENSOR_MSG_TIMEOUT = 2.0      # seconds without any message
    HEALTH_CHECK_INTERVAL = 0.5   # seconds between health checks

    # Status publish interval
    STATUS_INTERVAL = 0.5

    def __init__(self):
        super().__init__("cargabot_safety_guardian")

        # ── Parameters ──────────────────────────────────────────────
        self.declare_parameter('enabled', True)
        self.declare_parameter('emergency_dist_cm', self.EMERGENCY_DIST_CM)
        self.declare_parameter('stop_dist_cm', self.STOP_DIST_CM)
        self.declare_parameter('caution_dist_cm', self.CAUTION_DIST_CM)

        self._enabled = self.get_parameter('enabled').value
        self.EMERGENCY_DIST_CM = self.get_parameter('emergency_dist_cm').value
        self.STOP_DIST_CM = self.get_parameter('stop_dist_cm').value
        self.CAUTION_DIST_CM = self.get_parameter('caution_dist_cm').value

        # ── Sensor state ────────────────────────────────────────────
        self._sensors: Dict[str, SensorHealth] = {
            'front_upper': SensorHealth('front_upper', self.SENSOR_FREEZE_TIMEOUT),
            'behind':      SensorHealth('behind',      self.SENSOR_FREEZE_TIMEOUT),
            'front_lower': SensorHealth('front_lower', self.SENSOR_FREEZE_TIMEOUT),
            'left':        SensorHealth('left',        self.SENSOR_FREEZE_TIMEOUT),
            'right':       SensorHealth('right',       self.SENSOR_FREEZE_TIMEOUT),
        }

        # Map sensor numbers to names
        self._sensor_map = {
            1: 'front_upper',
            2: 'behind',
            3: 'front_lower',
            4: 'left',
            5: 'right',
        }

        # ── Lift state ──────────────────────────────────────────────
        self._lift_lock = threading.Lock()
        self._lift_top_limit = False
        self._lift_bottom_limit = False

        # ── Safety state ────────────────────────────────────────────
        self._state = SafetyState.CLEAR
        self._state_lock = threading.Lock()
        self._backup_active = False
        self._backup_thread: Optional[threading.Thread] = None
        self._last_cmd = Twist()  # last command from MC

        # ── Publishers ──────────────────────────────────────────────
        self._safe_pub = self.create_publisher(
            Twist, 'turtlebot_cmdVel_safe', 10)
        self._status_pub = self.create_publisher(
            String, '/cargabot/safety_status', 10)
        self._exec_pub = self.create_publisher(
            String, '/cargabot/execute_req', 10)

        # ── Subscribers ─────────────────────────────────────────────
        # Velocity from MotionController
        self.create_subscription(
            Twist, 'turtlebot_cmdVel', self._cmd_vel_cb, 10)

        # Lift state
        self.create_subscription(
            String, '/lift_tof/lift_state', self._lift_state_cb, 10)

        # ToF sensors — try both Float32 and String (adapt to your msg type)
        for sensor_num in range(1, 6):
            # Assuming std_msgs/Float32 with .data = distance in meters
            self.create_subscription(
                Float32,
                f'/lift_tof/ranges/sensor_{sensor_num}',
                lambda msg, sn=sensor_num: self._tof_cb(sn, msg),
                10)

        # ── Timers ──────────────────────────────────────────────────
        self.create_timer(self.HEALTH_CHECK_INTERVAL, self._health_check)
        self.create_timer(self.STATUS_INTERVAL, self._publish_status)

        self.get_logger().info(
            f"Safety Guardian ready\n"
            f"  emergency < {self.EMERGENCY_DIST_CM} cm → BACKUP\n"
            f"  stop      < {self.STOP_DIST_CM} cm → BLOCK direction\n"
            f"  caution   < {self.CAUTION_DIST_CM} cm → SLOW DOWN\n"
            f"  freeze timeout = {self.SENSOR_FREEZE_TIMEOUT} s"
        )

    # ── Sensor callbacks ────────────────────────────────────────────

    def _tof_cb(self, sensor_num: int, msg: Float32):
        name = self._sensor_map.get(sensor_num)
        if name is None:
            return
        # msg.data is in meters
        self._sensors[name].update(msg.data)

    def _lift_state_cb(self, msg: String):
        try:
            data = json.loads(msg.data)
            limits = data.get('limits', {})
            with self._lift_lock:
                self._lift_top_limit = limits.get('top', False)
                self._lift_bottom_limit = limits.get('bottom', False)
        except (json.JSONDecodeError, AttributeError):
            pass

    # ── Health check ────────────────────────────────────────────────

    def _health_check(self):
        now = time.time()
        for sensor in self._sensors.values():
            sensor.check_timeout(now, self.SENSOR_MSG_TIMEOUT)

    # ── Get effective front distance ────────────────────────────────

    def _get_front_distance_cm(self) -> Optional[float]:
        """
        Returns the best front distance reading in cm, considering
        lift position and sensor health.

        Lift at top    → front_upper is physically blocked → use front_lower
        Lift at bottom → front_lower is physically blocked → use front_upper
        Both available → use the minimum (most conservative)
        """
        with self._lift_lock:
            top_blocked = self._lift_top_limit
            bottom_blocked = self._lift_bottom_limit

        upper = self._sensors['front_upper']
        lower = self._sensors['front_lower']

        upper_val = None
        lower_val = None

        # Only consider sensors that aren't physically blocked by lift
        if not top_blocked and upper.is_reliable():
            raw = upper.get_filtered()
            if raw is not None:
                upper_val = raw * 100.0  # meters → cm

        if not bottom_blocked and lower.is_reliable():
            raw = lower.get_filtered()
            if raw is not None:
                lower_val = raw * 100.0

        # Return most conservative (minimum) of available readings
        candidates = [v for v in [upper_val, lower_val] if v is not None]
        if not candidates:
            return None  # No reliable front reading
        return min(candidates)

    def _get_rear_distance_cm(self) -> Optional[float]:
        sensor = self._sensors['behind']
        if not sensor.is_reliable():
            return None
        raw = sensor.get_filtered()
        return raw * 100.0 if raw is not None else None

    def _get_left_distance_cm(self) -> Optional[float]:
        sensor = self._sensors['left']
        if not sensor.is_reliable():
            return None
        raw = sensor.get_filtered()
        return raw * 100.0 if raw is not None else None

    def _get_right_distance_cm(self) -> Optional[float]:
        sensor = self._sensors['right']
        if not sensor.is_reliable():
            return None
        raw = sensor.get_filtered()
        return raw * 100.0 if raw is not None else None

    # ── Evaluate safety ─────────────────────────────────────────────

    def _evaluate(self, cmd: Twist) -> Twist:
        """
        Evaluate incoming velocity command against sensor readings.
        Returns a safe velocity command (possibly modified or zeroed).
        """
        if not self._enabled:
            return cmd

        # During backup maneuver, guardian controls the robot
        if self._backup_active:
            return Twist()  # MC commands are suppressed

        safe = Twist()
        safe.linear.x = cmd.linear.x
        safe.angular.z = cmd.angular.z

        front_cm = self._get_front_distance_cm()
        rear_cm = self._get_rear_distance_cm()
        left_cm = self._get_left_distance_cm()
        right_cm = self._get_right_distance_cm()

        new_state = SafetyState.CLEAR

        # ── Forward protection ────────────────────────────────────
        if front_cm is not None and cmd.linear.x > 0:
            if front_cm < self.EMERGENCY_DIST_CM:
                # Too close — need emergency backup
                safe.linear.x = 0.0
                safe.angular.z = 0.0
                new_state = SafetyState.EMERGENCY
                self._trigger_backup()

            elif front_cm < self.STOP_DIST_CM:
                # Blocked — stop forward, allow rotation
                safe.linear.x = 0.0
                new_state = SafetyState.BLOCKED_FRONT
                self.get_logger().warn(
                    f"FRONT BLOCKED at {front_cm:.1f} cm — stopping forward")

            elif front_cm < self.CAUTION_DIST_CM:
                # Caution — reduce speed
                safe.linear.x = cmd.linear.x * self.SPEED_REDUCE_FACTOR
                new_state = SafetyState.CAUTION

        # ── Rear protection ───────────────────────────────────────
        if rear_cm is not None and cmd.linear.x < 0:
            if rear_cm < self.STOP_DIST_CM:
                safe.linear.x = 0.0
                new_state = SafetyState.BLOCKED_REAR
                self.get_logger().warn(
                    f"REAR BLOCKED at {rear_cm:.1f} cm — stopping backward")
            elif rear_cm < self.CAUTION_DIST_CM:
                safe.linear.x = cmd.linear.x * self.SPEED_REDUCE_FACTOR
                if new_state == SafetyState.CLEAR:
                    new_state = SafetyState.CAUTION

        # ── Side protection (rotation) ────────────────────────────
        # Positive angular.z = turn left → left side gets closer to walls
        if left_cm is not None and cmd.angular.z > 0:
            if left_cm < self.STOP_DIST_CM:
                safe.angular.z = 0.0
                new_state = SafetyState.BLOCKED_LEFT
                self.get_logger().warn(
                    f"LEFT BLOCKED at {left_cm:.1f} cm — stopping left turn")

        # Negative angular.z = turn right → right side gets closer
        if right_cm is not None and cmd.angular.z < 0:
            if right_cm < self.STOP_DIST_CM:
                safe.angular.z = 0.0
                new_state = SafetyState.BLOCKED_RIGHT
                self.get_logger().warn(
                    f"RIGHT BLOCKED at {right_cm:.1f} cm — stopping right turn")

        with self._state_lock:
            if new_state != self._state:
                self._state = new_state
                if new_state != SafetyState.CLEAR:
                    self.get_logger().info(f"Safety state → {new_state.name}")

        return safe

    # ── Backup maneuver ─────────────────────────────────────────────

    def _trigger_backup(self):
        """Start emergency backup maneuver in a separate thread."""
        if self._backup_active:
            return  # already backing up

        self.get_logger().error(
            "EMERGENCY — obstacle too close, initiating backup maneuver")

        # Notify MC to stop current command
        stop_msg = String()
        stop_msg.data = json.dumps({"cmd": "stop"})
        self._exec_pub.publish(stop_msg)

        self._backup_active = True
        self._backup_thread = threading.Thread(
            target=self._backup_sequence, daemon=True)
        self._backup_thread.start()

    def _backup_sequence(self):
        """
        Emergency backup sequence:
          1. Stop completely
          2. Back up slowly
          3. Check rear sensor — abort if rear is also blocked
          4. Rotate away from obstacle
          5. Resume normal operation
        """
        try:
            with self._state_lock:
                self._state = SafetyState.BACKUP

            # Step 1: Full stop
            self._send_safe(0.0, 0.0)
            time.sleep(0.3)

            # Step 2: Check rear before backing up
            rear_cm = self._get_rear_distance_cm()
            if rear_cm is not None and rear_cm < self.STOP_DIST_CM:
                self.get_logger().error(
                    f"Rear also blocked ({rear_cm:.1f} cm) — cannot backup, "
                    f"only rotating")
                # Skip backup, just rotate
            else:
                # Back up slowly
                self.get_logger().info("Backing up...")
                t0 = time.time()
                while time.time() - t0 < self.BACKUP_DURATION:
                    # Continuously check rear while backing up
                    rear_cm = self._get_rear_distance_cm()
                    if rear_cm is not None and rear_cm < self.STOP_DIST_CM:
                        self.get_logger().warn(
                            f"Rear obstacle at {rear_cm:.1f} cm — "
                            f"stopping backup early")
                        break
                    self._send_safe(self.BACKUP_SPEED, 0.0)
                    time.sleep(0.05)

                self._send_safe(0.0, 0.0)
                time.sleep(0.2)

            # Step 3: Rotate away — pick direction based on side sensors
            left_cm = self._get_left_distance_cm()
            right_cm = self._get_right_distance_cm()

            # Rotate toward the side with more space
            if left_cm is not None and right_cm is not None:
                rotate_dir = 1.0 if left_cm > right_cm else -1.0
            elif left_cm is not None:
                rotate_dir = 1.0  # left is known, prefer it
            elif right_cm is not None:
                rotate_dir = -1.0  # right is known
            else:
                rotate_dir = 1.0  # default: turn left

            self.get_logger().info(
                f"Rotating {'left' if rotate_dir > 0 else 'right'}...")
            t0 = time.time()
            while time.time() - t0 < self.BACKUP_ROTATE_DURATION:
                self._send_safe(0.0, rotate_dir * self.BACKUP_ROTATE_SPEED)
                time.sleep(0.05)

            self._send_safe(0.0, 0.0)
            time.sleep(0.3)

            self.get_logger().info("Backup maneuver complete")

        except Exception as e:
            self.get_logger().error(f"Backup maneuver exception: {e}")
            self._send_safe(0.0, 0.0)

        finally:
            self._backup_active = False
            with self._state_lock:
                self._state = SafetyState.CLEAR

    def _send_safe(self, v: float, w: float):
        """Publish directly to the safe output topic."""
        msg = Twist()
        msg.linear.x = v
        msg.angular.z = w
        self._safe_pub.publish(msg)

    # ── Velocity callback ───────────────────────────────────────────

    def _cmd_vel_cb(self, msg: Twist):
        """
        Intercept velocity from MotionController, evaluate safety,
        publish filtered command to serial bridge.
        """
        self._last_cmd = msg
        safe_cmd = self._evaluate(msg)
        self._safe_pub.publish(safe_cmd)

    # ── Status publishing ───────────────────────────────────────────

    def _publish_status(self):
        with self._state_lock:
            state_name = self._state.name

        front_cm = self._get_front_distance_cm()
        rear_cm = self._get_rear_distance_cm()
        left_cm = self._get_left_distance_cm()
        right_cm = self._get_right_distance_cm()

        sensor_health = {}
        for name, sensor in self._sensors.items():
            sensor_health[name] = {
                'healthy': sensor.healthy,
                'frozen': sensor.frozen,
                'reliable': sensor.is_reliable(),
                'value_cm': round(sensor.value * 100, 1) if sensor.value else None,
            }

        with self._lift_lock:
            lift = {
                'top_limit': self._lift_top_limit,
                'bottom_limit': self._lift_bottom_limit,
            }

        status = {
            'state': state_name,
            'backup_active': self._backup_active,
            'enabled': self._enabled,
            'distances_cm': {
                'front': round(front_cm, 1) if front_cm else None,
                'rear': round(rear_cm, 1) if rear_cm else None,
                'left': round(left_cm, 1) if left_cm else None,
                'right': round(right_cm, 1) if right_cm else None,
            },
            'lift': lift,
            'sensors': sensor_health,
        }

        msg = String()
        msg.data = json.dumps(status)
        self._status_pub.publish(msg)

    # ── Shutdown ────────────────────────────────────────────────────

    def destroy_node(self):
        self._send_safe(0.0, 0.0)
        super().destroy_node()


# ── Entry point ─────────────────────────────────────────────────────

def main(args=None):
    rclpy.init(args=args)
    node = SafetyGuardian()
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