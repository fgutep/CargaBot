#!/usr/bin/env python3
"""
CargaBot Motion Controller v2
==============================
Nodo ROS2 de control de movimiento preciso para CargaBot.
Usa std_msgs/String JSON — sin compilar .srv.

Service:
  /cargabot/execute  (std_srvs/SetBool no — usamos srv String)

  REQUEST  topic: /cargabot/execute_req  (std_msgs/String)  — path planner → MC
  RESPONSE topic: /cargabot/execute_res  (std_msgs/String)  — MC → path planner

  El path planner publica en /cargabot/execute_req y espera en /cargabot/execute_res.
  Solo se procesa un comando a la vez (mutex).

REQUEST JSON:
  {"cmd":"move",      "distance":0.50}
  {"cmd":"rotate",    "angle":90.0}
  {"cmd":"rotate_to", "heading":90.0}
  {"cmd":"goto",      "x":1.0, "y":0.5, "heading":0.0}
  {"cmd":"waypoints", "waypoints":[[x1,y1],[x2,y2],...], "heading":0.0}
  {"cmd":"stop"}

RESPONSE JSON:
  {"success":true,  "message":"...", "x":0.0, "y":0.0, "heading":0.0}
  {"success":false, "message":"error description"}

Parámetros ROS2:
  linear_speed    0.15  m/s
  angular_speed   0.30  rad/s
  dist_factor     1.7277
  pos_tolerance   0.008 m
  angle_tolerance 2.0   deg
  settle_time     0.4   s

Uso desde path planner:
  publisher  = node.create_publisher(String, '/cargabot/execute_req', 10)
  subscriber = node.create_subscription(String, '/cargabot/execute_res', cb, 10)

  req = String()
  req.data = json.dumps({"cmd": "move", "distance": 0.50})
  publisher.publish(req)
  # cb recibe: {"success": true, "message": "...", "x": ..., "y": ..., "heading": ...}
"""

import json
import math
import threading
import time

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from nav_msgs.msg import Odometry
from std_msgs.msg import String


# ── Utilidades angulares ──────────────────────────────────────────────

def normalize_angle(a: float) -> float:
    while a >  math.pi: a -= 2 * math.pi
    while a < -math.pi: a += 2 * math.pi
    return a

def deg2rad(d: float) -> float: return d * math.pi / 180.0
def rad2deg(r: float) -> float: return r * 180.0 / math.pi

OCTAL_HEADINGS = [0, 45, 90, 135, 180, 225, 270, 315]

def nearest_octal(deg: float) -> float:
    deg = deg % 360
    return min(OCTAL_HEADINGS,
               key=lambda h: abs(normalize_angle(deg2rad(h - deg))))


# ── Motion Controller ─────────────────────────────────────────────────

class MotionController(Node):

    def __init__(self):
        super().__init__("cargabot_motion_controller")

        # ── Parámetros ──────────────────────────────────────────────
        self.linear_speed    = self.declare_parameter('linear_speed',    0.15  ).value
        self.angular_speed   = self.declare_parameter('angular_speed',   0.30  ).value
        self.dist_factor     = self.declare_parameter('dist_factor',     1.7277).value
        self.pos_tolerance   = self.declare_parameter('pos_tolerance',   0.008 ).value
        self.angle_tolerance = self.declare_parameter('angle_tolerance', 2.0   ).value
        self.settle_time     = self.declare_parameter('settle_time',     0.4   ).value

        # ── Pose ────────────────────────────────────────────────────
        self._pose_lock = threading.Lock()
        self._x  = 0.0
        self._y  = 0.0
        self._th = 0.0

        # ── Mutex: solo un movimiento a la vez ───────────────────────
        self._motion_lock = threading.Lock()

        # ── Publishers / Subscribers ─────────────────────────────────
        self._cmd_pub = self.create_publisher(Twist,  'turtlebot_cmdVel',       10)
        self._res_pub = self.create_publisher(String, '/cargabot/execute_res',  10)

        self.create_subscription(Odometry, '/odom',                self._odom_cb,    10)
        self.create_subscription(String,   '/cargabot/execute_req', self._execute_cb, 10)

        self.get_logger().info(
            f"CargaBot Motion Controller ready\n"
            f"  dist_factor  = {self.dist_factor}\n"
            f"  linear_speed = {self.linear_speed} m/s\n"
            f"  angular_speed= {self.angular_speed} rad/s\n"
            f"  REQ  topic   = /cargabot/execute_req\n"
            f"  RES  topic   = /cargabot/execute_res"
        )

    # ── ODOMETRY ─────────────────────────────────────────────────────

    def _odom_cb(self, msg: Odometry):
        q = msg.pose.pose.orientation
        siny = 2.0 * (q.w * q.z + q.x * q.y)
        cosy = 1.0 - 2.0 * (q.y * q.y + q.z * q.z)
        with self._pose_lock:
            self._x  = msg.pose.pose.position.x
            self._y  = msg.pose.pose.position.y
            self._th = math.atan2(siny, cosy)

    def _pose(self):
        with self._pose_lock:
            return self._x, self._y, self._th

    # ── EXECUTE CALLBACK ─────────────────────────────────────────────

    def _execute_cb(self, msg: String):
        """
        Recibe comando JSON del path planner.
        Corre en hilo separado para no bloquear el spin.
        """
        try:
            req = json.loads(msg.data)
        except json.JSONDecodeError as e:
            self._respond(False, f"Invalid JSON: {e}")
            return

        # Corre en hilo para no bloquear el executor de ROS
        threading.Thread(target=self._execute, args=(req,), daemon=True).start()

    def _execute(self, req: dict):
        cmd = req.get("cmd", "").strip().lower()
        self.get_logger().info(f"[execute] {json.dumps(req)}")

        with self._motion_lock:
            try:
                if cmd == "stop":
                    self._stop()
                    self._respond(True, "stopped")

                elif cmd == "move":
                    ok, msg = self._do_move(float(req.get("distance", 0)))
                    self._respond(ok, msg)

                elif cmd == "rotate":
                    ok, msg = self._do_rotate(deg2rad(float(req.get("angle", 0))))
                    self._respond(ok, msg)

                elif cmd == "rotate_to":
                    snapped = nearest_octal(float(req.get("heading", 0)))
                    _, _, th = self._pose()
                    delta = normalize_angle(deg2rad(snapped) - th)
                    ok, msg = self._do_rotate(delta)
                    self._respond(ok, f"rotate_to {snapped}°: {msg}")

                elif cmd == "goto":
                    ok, msg = self._do_goto(
                        float(req.get("x", 0)),
                        float(req.get("y", 0)),
                        req.get("heading")      # None = no final rotation
                    )
                    self._respond(ok, msg)

                elif cmd == "waypoints":
                    wps = req.get("waypoints", [])  # [[x1,y1], [x2,y2], ...]
                    hdg = req.get("heading")
                    ok, msg = self._do_waypoints(wps, hdg)
                    self._respond(ok, msg)

                else:
                    self._respond(False, f"unknown command: '{cmd}'")

            except Exception as e:
                self._stop()
                self._respond(False, f"exception: {e}")
                self.get_logger().error(f"[execute] exception: {e}")

    def _respond(self, success: bool, message: str):
        x, y, th = self._pose()
        payload = {
            "success": success,
            "message": message,
            "x":       round(x, 4),
            "y":       round(y, 4),
            "heading": round(rad2deg(th) % 360, 2),
        }
        msg = String(); msg.data = json.dumps(payload)
        self._res_pub.publish(msg)
        self.get_logger().info(f"[response] {json.dumps(payload)}")

    # ── PRIMITIVAS DE MOVIMIENTO ─────────────────────────────────────

    def _do_move(self, distance_m: float) -> tuple:
        if abs(distance_m) < 0.001:
            return True, "distance too small, skipped"

        corrected = distance_m / self.dist_factor
        x0, y0, _ = self._pose()
        self._send_vel(math.copysign(self.linear_speed, corrected), 0.0)

        timeout = abs(corrected) / self.linear_speed * 3.0 + 2.0
        t0 = time.time()

        while time.time() - t0 < timeout:
            x, y, _ = self._pose()
            traveled  = math.sqrt((x - x0)**2 + (y - y0)**2)
            remaining = abs(corrected) - traveled

            # Rampa desaceleración: últimos 3 cm
            if 0 < remaining < 0.03:
                factor = max(0.25, remaining / 0.03)
                self._send_vel(
                    math.copysign(self.linear_speed * factor, corrected), 0.0)

            if traveled >= abs(corrected) - self.pos_tolerance:
                self._stop()
                time.sleep(self.settle_time)
                return True, f"moved {distance_m*100:.1f}cm (traveled {traveled*100:.1f}cm)"

            time.sleep(0.02)

        self._stop()
        return False, f"move timeout ({time.time()-t0:.1f}s)"

    def _do_rotate(self, angle_rad: float) -> tuple:
        if abs(angle_rad) < deg2rad(1.0):
            return True, "angle too small, skipped"

        _, _, th0 = self._pose()
        last_th   = th0
        accum     = 0.0
        self._send_vel(0.0, math.copysign(self.angular_speed, angle_rad))

        timeout = abs(angle_rad) / self.angular_speed * 3.0 + 3.0
        t0 = time.time()

        while time.time() - t0 < timeout:
            _, _, th = self._pose()
            delta  = normalize_angle(th - last_th)
            accum += delta
            last_th = th

            remaining = abs(angle_rad) - abs(accum)

            # Rampa desaceleración: últimos 10°
            if 0 < remaining < deg2rad(10):
                factor = max(0.2, remaining / deg2rad(10))
                self._send_vel(
                    0.0, math.copysign(self.angular_speed * factor, angle_rad))

            if abs(accum) >= abs(angle_rad) - deg2rad(self.angle_tolerance):
                self._stop()
                time.sleep(self.settle_time)
                return True, (
                    f"rotated {rad2deg(accum):.1f}° "
                    f"(target {rad2deg(angle_rad):.1f}°)"
                )

            time.sleep(0.02)

        self._stop()
        return False, (
            f"rotate timeout — accum={rad2deg(accum):.1f}° "
            f"target={rad2deg(angle_rad):.1f}°"
        )

    def _do_goto(self, tx: float, ty: float, heading_deg=None) -> tuple:
        x, y, th = self._pose()
        dx = tx - x; dy = ty - y
        dist = math.sqrt(dx**2 + dy**2)

        if dist > self.pos_tolerance:
            # 1. Rotar hacia el punto
            target_angle = math.atan2(dy, dx)
            delta = normalize_angle(target_angle - th)
            ok, msg = self._do_rotate(delta)
            if not ok:
                return False, f"rotate to point failed: {msg}"

            # 2. Avanzar
            ok, msg = self._do_move(dist)
            if not ok:
                return False, f"move failed: {msg}"

        # 3. Heading final (opcional)
        if heading_deg is not None:
            snapped = nearest_octal(float(heading_deg))
            _, _, th_now = self._pose()
            delta = normalize_angle(deg2rad(snapped) - th_now)
            ok, msg = self._do_rotate(delta)
            if not ok:
                return False, f"final rotate failed: {msg}"

        x, y, _ = self._pose()
        return True, f"goto ({tx:.2f},{ty:.2f}) → actual ({x:.2f},{y:.2f})"

    def _do_waypoints(self, waypoints: list, final_heading=None) -> tuple:
        if not waypoints:
            return False, "empty waypoints list"

        for i, wp in enumerate(waypoints):
            # Cada waypoint es [x, y] o {"x":..., "y":...}
            if isinstance(wp, (list, tuple)):
                wx, wy = float(wp[0]), float(wp[1])
            else:
                wx, wy = float(wp.get("x", 0)), float(wp.get("y", 0))

            hdg = final_heading if i == len(waypoints) - 1 else None
            ok, msg = self._do_goto(wx, wy, hdg)
            self.get_logger().info(f"[wp {i+1}/{len(waypoints)}] {msg}")
            if not ok:
                return False, f"waypoint {i+1} failed: {msg}"

        return True, f"completed {len(waypoints)} waypoints"

    # ── HELPERS ──────────────────────────────────────────────────────

    def _send_vel(self, v: float, w: float):
        msg = Twist()
        msg.linear.x  = v
        msg.angular.z = w
        self._cmd_pub.publish(msg)

    def _stop(self):
        self._send_vel(0.0, 0.0)


# ── ENTRY POINT ──────────────────────────────────────────────────────

def main(args=None):
    rclpy.init(args=args)
    node = MotionController()
    executor = rclpy.executors.MultiThreadedExecutor()
    executor.add_node(node)
    try:
        executor.spin()
    except KeyboardInterrupt:
        pass
    finally:
        node._stop()
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()