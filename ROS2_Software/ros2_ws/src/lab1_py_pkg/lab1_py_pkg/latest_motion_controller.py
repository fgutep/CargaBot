#!/usr/bin/env python3
"""
CargaBot Motion Controller v2
==============================
Nodo ROS2 de control de movimiento preciso para CargaBot.
Expone un Service ROS2 para el path planner de la cámara.

Service:
  /cargabot/execute  (lab1_py_pkg/CargaBotExecute)

Suscripciones:
  /odom              (nav_msgs/Odometry) — pose + heading IMU

Publicaciones:
  /turtlebot_cmdVel  (geometry_msgs/Twist) — hacia serial bridge

Parámetros:
  linear_speed      float  0.15  m/s
  angular_speed     float  0.30  rad/s  — bajo para reducir derrape
  dist_factor       float  1.7277       — calibrado con cinta métrica
  pos_tolerance     float  0.008 m
  angle_tolerance   float  2.0   deg
  settle_time       float  0.4   s

Uso desde path planner (Python):
    import rclpy
    from rclpy.node import Node
    from lab1_py_pkg.srv import CargaBotExecute

    client = node.create_client(CargaBotExecute, '/cargabot/execute')
    req = CargaBotExecute.Request()
    req.cmd = 'move'; req.distance = 0.50
    future = client.call_async(req)
    # future.result().success / .message / .final_x / .final_y / .final_heading
"""

import math
import threading
import time

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from nav_msgs.msg import Odometry

# Import del service generado por colcon
# Si falla el import, el nodo imprime instrucciones y sale
try:
    from lab1_py_pkg.srv import CargaBotExecute
    SRV_AVAILABLE = True
except ImportError:
    SRV_AVAILABLE = False


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
        self._th = 0.0  # radianes, viene del IMU via odom

        # ── Publishers / Subscribers ─────────────────────────────────
        self._cmd_pub = self.create_publisher(Twist, 'turtlebot_cmdVel', 10)
        self.create_subscription(Odometry, '/odom', self._odom_cb, 10)

        # ── Service ─────────────────────────────────────────────────
        if not SRV_AVAILABLE:
            self.get_logger().error(
                "lab1_py_pkg.srv.CargaBotExecute not found.\n"
                "Agrega CargaBotExecute.srv a tu paquete y corre 'colcon build'.\n"
                "Ver: /srv/CargaBotExecute.srv"
            )
            return

        self._srv = self.create_service(
            CargaBotExecute,
            '/cargabot/execute',
            self._execute_cb
        )

        # Mutex para que solo un movimiento corra a la vez
        self._motion_lock = threading.Lock()

        self.get_logger().info(
            f"CargaBot Motion Controller ready\n"
            f"  dist_factor    = {self.dist_factor}\n"
            f"  linear_speed   = {self.linear_speed} m/s\n"
            f"  angular_speed  = {self.angular_speed} rad/s\n"
            f"  Service        = /cargabot/execute"
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

    # ── SERVICE CALLBACK ─────────────────────────────────────────────

    def _execute_cb(self, request, response):
        """
        Punto de entrada del path planner.
        Bloquea hasta completar el movimiento (síncrono).
        """
        cmd = request.cmd.strip().lower()
        self.get_logger().info(f"[execute] cmd='{cmd}'")

        with self._motion_lock:
            try:
                if cmd == "stop":
                    self._stop()
                    response.success = True
                    response.message = "Stopped"

                elif cmd == "move":
                    ok, msg = self._do_move(request.distance)
                    response.success = ok
                    response.message = msg

                elif cmd == "rotate":
                    ok, msg = self._do_rotate(deg2rad(request.angle))
                    response.success = ok
                    response.message = msg

                elif cmd == "rotate_to":
                    snapped = nearest_octal(request.heading)
                    _, _, th = self._pose()
                    delta = normalize_angle(deg2rad(snapped) - th)
                    ok, msg = self._do_rotate(delta)
                    response.success = ok
                    response.message = f"rotate_to {snapped}° — {msg}"

                elif cmd == "goto":
                    ok, msg = self._do_goto(
                        request.x, request.y, request.heading)
                    response.success = ok
                    response.message = msg

                elif cmd == "waypoints":
                    ok, msg = self._do_waypoints(
                        request.waypoints, request.heading)
                    response.success = ok
                    response.message = msg

                else:
                    response.success = False
                    response.message = f"Unknown command: '{cmd}'"

            except Exception as e:
                self._stop()
                response.success = False
                response.message = f"Exception: {e}"
                self.get_logger().error(f"[execute] {e}")

        # Pose final
        x, y, th = self._pose()
        response.final_x       = x
        response.final_y       = y
        response.final_heading = rad2deg(th) % 360

        self.get_logger().info(
            f"[execute] done — success={response.success} "
            f"pos=({x:.3f},{y:.3f}) hdg={response.final_heading:.1f}°"
        )
        return response

    # ── PRIMITIVAS DE MOVIMIENTO ─────────────────────────────────────

    def _do_move(self, distance_m: float) -> tuple[bool, str]:
        """
        Mueve distance_m metros (positivo=adelante).
        Aplica dist_factor de calibración.
        Rampa de desaceleración en los últimos 3 cm.
        """
        if abs(distance_m) < 0.001:
            return True, "distance too small, skipped"

        corrected = distance_m / self.dist_factor
        x0, y0, _ = self._pose()
        v = math.copysign(self.linear_speed, corrected)
        self._send_vel(v, 0.0)

        timeout = abs(corrected) / self.linear_speed * 3.0 + 2.0
        t0 = time.time()

        while time.time() - t0 < timeout:
            x, y, _ = self._pose()
            traveled = math.sqrt((x-x0)**2 + (y-y0)**2)
            remaining = abs(corrected) - traveled

            # Rampa: últimos 3 cm
            if 0 < remaining < 0.03:
                factor = max(0.25, remaining / 0.03)
                self._send_vel(math.copysign(self.linear_speed * factor, corrected), 0.0)

            if traveled >= abs(corrected) - self.pos_tolerance:
                self._stop()
                time.sleep(self.settle_time)
                return True, f"moved {distance_m*100:.1f}cm (traveled {traveled*100:.1f}cm)"

            time.sleep(0.02)

        self._stop()
        return False, f"move timeout after {time.time()-t0:.1f}s"

    def _do_rotate(self, angle_rad: float) -> tuple[bool, str]:
        """
        Rota angle_rad radianes (positivo=CCW).
        Usa IMU para medir ángulo real — no necesita angle_factor.
        Acumula delta ciclo a ciclo para soportar >180°.
        """
        if abs(angle_rad) < deg2rad(1.0):
            return True, "angle too small, skipped"

        _, _, th0 = self._pose()
        last_th      = th0
        accum        = 0.0
        w = math.copysign(self.angular_speed, angle_rad)
        self._send_vel(0.0, w)

        timeout = abs(angle_rad) / self.angular_speed * 3.0 + 3.0
        t0 = time.time()

        while time.time() - t0 < timeout:
            _, _, th = self._pose()
            delta  = normalize_angle(th - last_th)
            accum += delta
            last_th = th

            remaining = abs(angle_rad) - abs(accum)

            # Rampa: últimos 10°
            if 0 < remaining < deg2rad(10):
                factor = max(0.2, remaining / deg2rad(10))
                self._send_vel(0.0, math.copysign(self.angular_speed * factor, angle_rad))

            if abs(accum) >= abs(angle_rad) - deg2rad(self.angle_tolerance):
                self._stop()
                time.sleep(self.settle_time)
                return True, f"rotated {rad2deg(accum):.1f}° (target {rad2deg(angle_rad):.1f}°)"

            time.sleep(0.02)

        self._stop()
        return False, f"rotate timeout — accum={rad2deg(accum):.1f}° target={rad2deg(angle_rad):.1f}°"

    def _do_goto(self, tx: float, ty: float,
                 heading_deg: float = None) -> tuple[bool, str]:
        """
        Navega al punto (tx, ty) con movimientos ortogonales:
        1. Rota hacia el punto
        2. Avanza la distancia
        3. Rota al heading final si se especifica (snappea a octal)
        """
        x, y, th = self._pose()
        dx = tx - x
        dy = ty - y
        dist = math.sqrt(dx**2 + dy**2)

        if dist < self.pos_tolerance:
            # Ya estamos en el punto — solo rotar al heading si se pide
            if heading_deg is not None:
                snapped = nearest_octal(heading_deg)
                delta   = normalize_angle(deg2rad(snapped) - th)
                ok, msg = self._do_rotate(delta)
                return ok, f"already at point, rotated to {snapped}°: {msg}"
            return True, "already at target point"

        # 1. Rotar hacia el punto objetivo
        target_angle = math.atan2(dy, dx)
        delta = normalize_angle(target_angle - th)
        ok, msg = self._do_rotate(delta)
        if not ok:
            return False, f"rotate to point failed: {msg}"

        # 2. Avanzar
        ok, msg = self._do_move(dist)
        if not ok:
            return False, f"move to point failed: {msg}"

        # 3. Rotar al heading final si se especifica
        if heading_deg is not None:
            snapped = nearest_octal(heading_deg)
            _, _, th_now = self._pose()
            delta = normalize_angle(deg2rad(snapped) - th_now)
            ok, msg = self._do_rotate(delta)
            if not ok:
                return False, f"final rotate failed: {msg}"

        x, y, _ = self._pose()
        return True, f"goto ({tx:.2f},{ty:.2f}) done — actual ({x:.2f},{y:.2f})"

    def _do_waypoints(self, waypoints, final_heading_deg: float = None) -> tuple[bool, str]:
        """
        Ejecuta una lista de waypoints secuencialmente.
        Cada waypoint es un geometry_msgs/Point (usa x,y).
        """
        if not waypoints:
            return False, "empty waypoints list"

        for i, wp in enumerate(waypoints):
            # Heading final solo en el último waypoint
            hdg = final_heading_deg if i == len(waypoints) - 1 else None
            ok, msg = self._do_goto(wp.x, wp.y, hdg)
            self.get_logger().info(f"[waypoints] {i+1}/{len(waypoints)}: {msg}")
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
