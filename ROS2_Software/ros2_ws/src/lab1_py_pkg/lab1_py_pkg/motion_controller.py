#!/usr/bin/env python3
"""
CargaBot Motion Controller
==========================
Nodo ROS2 que ejecuta comandos de movimiento preciso para CargaBot.

Suscripciones:
  /cargabot/move_cmd  (cargabot_msgs/MoveCommand  — o std_msgs/String JSON)
  /odom               (nav_msgs/Odometry) — feedback de posición

Publicaciones:
  /turtlebot_cmdVel   (geometry_msgs/Twist) — hacia el serial bridge
  /cargabot/move_status (std_msgs/String JSON) — estado de la acción en curso

Comandos JSON aceptados en /cargabot/move_cmd (std_msgs/String):
  {"cmd": "move",   "distance": 0.30}          # metros, negativo = atrás
  {"cmd": "rotate", "angle": 90.0}             # grados, positivo = CCW
  {"cmd": "rotate_to", "heading": 45.0}        # heading absoluto 0-315 (octales)
  {"cmd": "stop"}
  {"cmd": "calibrate_distance", "reps": 3}     # wizard de calibración lineal
  {"cmd": "calibrate_angle",    "reps": 4}     # wizard de calibración angular
  {"cmd": "set_cal", "dist_factor": 1.05, "angle_factor": 0.98}  # aplicar manual

Parámetros ROS2:
  linear_speed      (float, default 0.15 m/s)
  angular_speed     (float, default 0.5 rad/s)
  dist_factor       (float, default 1.0)   — corrección odometría lineal
  angle_factor      (float, default 1.0)   — corrección odometría angular
  settle_time       (float, default 0.3 s) — espera tras completar movimiento
  pos_tolerance     (float, default 0.008) — tolerancia posición [m]  (~8mm)
  angle_tolerance   (float, default 1.5)   — tolerancia ángulo [deg]
"""

import math
import json
import threading
import time

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from nav_msgs.msg import Odometry
from std_msgs.msg import String


# ── Utilidades angulares ─────────────────────────────────────────────

def normalize_angle(a: float) -> float:
    """Normaliza ángulo a [-π, π]."""
    while a >  math.pi: a -= 2 * math.pi
    while a < -math.pi: a += 2 * math.pi
    return a

def deg2rad(d: float) -> float: return d * math.pi / 180.0
def rad2deg(r: float) -> float: return r * 180.0 / math.pi

# Headings octales válidos (múltiplos de 45°)
OCTAL_HEADINGS = [0, 45, 90, 135, 180, 225, 270, 315]

def nearest_octal(deg: float) -> float:
    """Retorna el heading octal más cercano al ángulo dado (en grados)."""
    deg = deg % 360
    return min(OCTAL_HEADINGS, key=lambda h: abs(normalize_angle(deg2rad(h - deg))))


# ── Nodo principal ───────────────────────────────────────────────────

class MotionController(Node):

    # Estados internos de la máquina de estados
    IDLE     = "IDLE"
    MOVING   = "MOVING"
    ROTATING = "ROTATING"
    SETTLING = "SETTLING"
    CAL_LIN  = "CAL_LIN"
    CAL_ANG  = "CAL_ANG"

    def __init__(self):
        super().__init__("cargabot_motion_controller")

        # ── Parámetros ──────────────────────────────────────────────
        self.linear_speed    = self.declare_parameter('linear_speed',    0.15).value
        self.angular_speed   = self.declare_parameter('angular_speed',   0.5 ).value
        self.dist_factor     = self.declare_parameter('dist_factor',     1.0 ).value
        self.angle_factor    = self.declare_parameter('angle_factor',    1.0 ).value
        self.settle_time     = self.declare_parameter('settle_time',     0.3 ).value
        self.pos_tolerance   = self.declare_parameter('pos_tolerance',   0.008).value
        self.angle_tolerance = self.declare_parameter('angle_tolerance', 1.5 ).value  # deg

        # ── Estado odometría ────────────────────────────────────────
        self.odom_lock = threading.Lock()
        self.x  = 0.0
        self.y  = 0.0
        self.th = 0.0  # radianes

        # ── Máquina de estados ──────────────────────────────────────
        self.state      = self.IDLE
        self.state_lock = threading.Lock()

        # Objetivo activo
        self.goal_distance = 0.0   # metros
        self.goal_angle    = 0.0   # radianes
        self.start_x       = 0.0
        self.start_y       = 0.0
        self.start_th      = 0.0

        # Cola de comandos (thread-safe)
        self._cmd_queue  = []
        self._cmd_lock   = threading.Lock()

        # Calibración wizard
        self._cal_reps_total   = 0
        self._cal_reps_done    = 0
        self._cal_measurements = []
        self._cal_target       = 0.0

        # ── Pubs / Subs ─────────────────────────────────────────────
        self.cmd_pub    = self.create_publisher(Twist,  'turtlebot_cmdVel',      10)
        self.status_pub = self.create_publisher(String, '/cargabot/move_status', 10)

        self.create_subscription(Odometry, '/odom',              self._odom_cb,    10)
        self.create_subscription(String,   '/cargabot/move_cmd', self._move_cmd_cb, 10)

        # Loop de control a 20 Hz
        self.create_timer(0.05, self._control_loop)

        self.get_logger().info("CargaBot Motion Controller ready.")
        self._publish_status("READY", {})

    # ── ODOMETRY CALLBACK ────────────────────────────────────────────

    def _odom_cb(self, msg: Odometry):
        q = msg.pose.pose.orientation
        # yaw desde cuaternión
        siny = 2.0 * (q.w * q.z + q.x * q.y)
        cosy = 1.0 - 2.0 * (q.y * q.y + q.z * q.z)
        yaw  = math.atan2(siny, cosy)
        with self.odom_lock:
            self.x  = msg.pose.pose.position.x
            self.y  = msg.pose.pose.position.y
            self.th = yaw

    def _get_pose(self):
        with self.odom_lock:
            return self.x, self.y, self.th

    # ── COMMAND CALLBACK ─────────────────────────────────────────────

    def _move_cmd_cb(self, msg: String):
        try:
            cmd = json.loads(msg.data)
        except json.JSONDecodeError:
            self.get_logger().error(f"Invalid JSON command: {msg.data}")
            return

        with self._cmd_lock:
            self._cmd_queue.append(cmd)

    def _dispatch_command(self, cmd: dict):
        action = cmd.get("cmd", "")

        if action == "stop":
            self._stop_motors()
            with self.state_lock:
                self.state = self.IDLE
            self._publish_status("STOPPED", {})

        elif action == "move":
            dist = float(cmd.get("distance", 0.0))
            self._start_move(dist)

        elif action == "rotate":
            angle_deg = float(cmd.get("angle", 0.0))
            self._start_rotate(deg2rad(angle_deg))

        elif action == "rotate_to":
            heading_deg = float(cmd.get("heading", 0.0))
            # Snap al octal más cercano si no es exactamente múltiplo de 45
            snapped = nearest_octal(heading_deg)
            if abs(snapped - heading_deg) > 2:
                self.get_logger().warn(
                    f"rotate_to {heading_deg}° snapped to nearest octal {snapped}°"
                )
            x, y, th = self._get_pose()
            current_deg = rad2deg(th) % 360
            delta = normalize_angle(deg2rad(snapped - current_deg))
            self._start_rotate(delta)

        elif action == "calibrate_distance":
            reps = int(cmd.get("reps", 3))
            self._start_calibrate_distance(reps)

        elif action == "calibrate_angle":
            reps = int(cmd.get("reps", 4))
            self._start_calibrate_angle(reps)

        elif action == "set_cal":
            if "dist_factor" in cmd:
                self.dist_factor = float(cmd["dist_factor"])
            if "angle_factor" in cmd:
                self.angle_factor = float(cmd["angle_factor"])
            self.get_logger().info(
                f"Calibration set: dist_factor={self.dist_factor:.4f} "
                f"angle_factor={self.angle_factor:.4f}"
            )
            self._publish_status("CAL_SET", {
                "dist_factor": self.dist_factor,
                "angle_factor": self.angle_factor
            })

        else:
            self.get_logger().warn(f"Unknown command: {action}")

    # ── START ACTIONS ────────────────────────────────────────────────

    def _start_move(self, distance_m: float):
        """Inicia movimiento lineal. distance_m positivo = adelante."""
        with self.state_lock:
            if self.state != self.IDLE:
                self.get_logger().warn("Motion in progress, command ignored.")
                return
            self.state = self.MOVING

        x, y, th = self._get_pose()
        self.start_x = x
        self.start_y = y
        self.start_th = th
        # Aplica factor de calibración: si el robot recorre más de lo pedido,
        # dist_factor < 1 (le pedimos menos para que llegue exacto)
        self.goal_distance = distance_m / self.dist_factor

        v = math.copysign(self.linear_speed, self.goal_distance)
        self._send_vel(v, 0.0)
        self.get_logger().info(
            f"Moving {distance_m*100:.1f} cm "
            f"(corrected: {self.goal_distance*100:.1f} cm)"
        )
        self._publish_status("MOVING", {"distance_m": distance_m})

    def _start_rotate(self, angle_rad: float):
        """Inicia rotación. angle_rad positivo = CCW (izquierda)."""
        with self.state_lock:
            if self.state != self.IDLE:
                self.get_logger().warn("Motion in progress, command ignored.")
                return
            self.state = self.ROTATING

        x, y, th = self._get_pose()
        self.start_th = th
        # Aplica factor de calibración angular
        self.goal_angle = angle_rad / self.angle_factor

        w = math.copysign(self.angular_speed, self.goal_angle)
        self._send_vel(0.0, w)
        self.get_logger().info(
            f"Rotating {rad2deg(angle_rad):.1f}° "
            f"(corrected: {rad2deg(self.goal_angle):.1f}°)"
        )
        self._publish_status("ROTATING", {"angle_deg": rad2deg(angle_rad)})

    # ── CONTROL LOOP ─────────────────────────────────────────────────

    def _control_loop(self):
        # Despachar siguiente comando si estamos IDLE
        with self.state_lock:
            current_state = self.state

        # Despachar siguiente comando de la cola
        # cal_measure debe procesarse en estados CAL_LIN/CAL_ANG también
        cmd = None
        with self._cmd_lock:
            if self._cmd_queue:
                # En estados CAL, solo sacar cal_measure — otros comandos esperan
                if current_state in (self.CAL_LIN, self.CAL_ANG):
                    for i, c in enumerate(self._cmd_queue):
                        if c.get("cmd") == "cal_measure":
                            cmd = self._cmd_queue.pop(i)
                            break
                elif current_state == self.IDLE:
                    cmd = self._cmd_queue.pop(0)
        if cmd:
            self._dispatch_command(cmd)

        if current_state == self.IDLE:
            return

        x, y, th = self._get_pose()

        if current_state == self.MOVING:
            traveled = math.sqrt(
                (x - self.start_x) ** 2 +
                (y - self.start_y) ** 2
            ) * math.copysign(1, self.goal_distance)

            remaining = abs(self.goal_distance) - abs(traveled)

            # Rampa de desaceleración: últimos 3 cm
            if remaining < 0.03 and remaining > 0:
                factor = max(0.25, remaining / 0.03)
                v = math.copysign(self.linear_speed * factor, self.goal_distance)
                self._send_vel(v, 0.0)

            if abs(traveled) >= abs(self.goal_distance) - self.pos_tolerance:
                self._stop_motors()
                self.get_logger().info(
                    f"Move done. Traveled: {traveled*100:.1f} cm "
                    f"/ target: {self.goal_distance*100:.1f} cm"
                )
                self._enter_settle(self.MOVING)

        elif current_state == self.ROTATING:
            rotated = normalize_angle(th - self.start_th)
            remaining = abs(self.goal_angle) - abs(rotated)

            # Rampa de desaceleración: últimos 10°
            if remaining < deg2rad(10) and remaining > 0:
                factor = max(0.2, remaining / deg2rad(10))
                w = math.copysign(self.angular_speed * factor, self.goal_angle)
                self._send_vel(0.0, w)

            if abs(rotated) >= abs(self.goal_angle) - deg2rad(self.angle_tolerance):
                self._stop_motors()
                self.get_logger().info(
                    f"Rotate done. Rotated: {rad2deg(rotated):.1f}° "
                    f"/ target: {rad2deg(self.goal_angle):.1f}°"
                )
                self._enter_settle(self.ROTATING)

        elif current_state == self.SETTLING:
            # Espera settle_time y luego vuelve a IDLE
            # (implementado con timer one-shot en _enter_settle)
            pass

        elif current_state == self.CAL_LIN:
            self._calibration_linear_loop(x, y, th)

        elif current_state == self.CAL_ANG:
            self._calibration_angular_loop(x, y, th)

    # ── SETTLE ──────────────────────────────────────────────────────

    def _enter_settle(self, prev_state: str):
        with self.state_lock:
            self.state = self.SETTLING
        self._publish_status("SETTLING", {})
        self.create_timer(
            self.settle_time,
            lambda: self._finish_settle(prev_state)
        )

    def _finish_settle(self, prev_state: str):
        with self.state_lock:
            if self.state == self.SETTLING:
                self.state = self.IDLE
        self._publish_status("DONE", {"prev": prev_state})

    # ── CALIBRACIÓN LINEAL ───────────────────────────────────────────
    # Protocolo:
    #   1. El nodo mueve 1.00 m (o lo que diga _cal_target)
    #   2. Publica status {"state":"CAL_MEASURE","rep":N,"instruction":"..."}
    #   3. El operador mide físicamente y publica el resultado en /cargabot/move_cmd:
    #      {"cmd":"cal_measure","measured_m": 0.97}
    #   4. Repite reps veces, promedia, calcula dist_factor y lo aplica.

    def _start_calibrate_distance(self, reps: int):
        with self.state_lock:
            if self.state != self.IDLE:
                self.get_logger().warn("Busy, cannot start calibration.")
                return
            self.state = self.CAL_LIN

        self._cal_reps_total   = reps
        self._cal_reps_done    = 0
        self._cal_measurements = []
        self._cal_target       = 1.0   # 1 metro — fácil de medir
        self._cal_waiting_measure = False

        self.get_logger().info(
            f"=== LINEAR CALIBRATION — {reps} reps of {self._cal_target*100:.0f} cm ==="
        )
        self._cal_do_next_rep()

    def _cal_do_next_rep(self):
        """Lanza el siguiente rep de calibración lineal."""
        self._cal_waiting_measure = False
        x, y, th = self._get_pose()
        self.start_x = x; self.start_y = y; self.start_th = th
        self.goal_distance = self._cal_target  # sin factor — queremos medir el error real
        v = math.copysign(self.linear_speed, self.goal_distance)
        self._send_vel(v, 0.0)
        self._publish_status("CAL_MOVING", {
            "rep": self._cal_reps_done + 1,
            "total": self._cal_reps_total,
            "target_m": self._cal_target,
        })

    def _calibration_linear_loop(self, x, y, th):
        if self._cal_waiting_measure:
            return  # esperando input del operador

        traveled = math.sqrt(
            (x - self.start_x) ** 2 + (y - self.start_y) ** 2
        )
        if traveled >= self._cal_target - self.pos_tolerance:
            self._stop_motors()
            self._cal_waiting_measure = True
            self._publish_status("CAL_MEASURE", {
                "rep": self._cal_reps_done + 1,
                "total": self._cal_reps_total,
                "odom_m": traveled,
                "instruction": (
                    f"Mide la distancia física recorrida y publica: "
                    f'{{\"cmd\":\"cal_measure\",\"measured_m\": <valor>}}'
                ),
            })

    def _receive_cal_measure_linear(self, measured_m: float):
        """Recibe medición del operador durante calibración lineal."""
        odom_traveled = math.sqrt(
            (self.x - self.start_x) ** 2 + (self.y - self.start_y) ** 2
        )
        # factor = lo que el robot cree que recorrió / lo que realmente recorrió
        factor = odom_traveled / measured_m if measured_m > 0 else 1.0
        self._cal_measurements.append(factor)
        self._cal_reps_done += 1

        self.get_logger().info(
            f"Rep {self._cal_reps_done}/{self._cal_reps_total}: "
            f"odom={odom_traveled*100:.1f}cm  físico={measured_m*100:.1f}cm  "
            f"factor={factor:.4f}"
        )

        if self._cal_reps_done >= self._cal_reps_total:
            self._finish_calibrate_distance()
        else:
            # Volver al origen antes del siguiente rep
            time.sleep(0.5)
            self._cal_do_next_rep()

    def _finish_calibrate_distance(self):
        avg = sum(self._cal_measurements) / len(self._cal_measurements)
        self.dist_factor = avg
        with self.state_lock:
            self.state = self.IDLE
        self.get_logger().info(
            f"=== CALIBRACIÓN LINEAL COMPLETA ===\n"
            f"  Mediciones: {[f'{v:.4f}' for v in self._cal_measurements]}\n"
            f"  dist_factor aplicado: {self.dist_factor:.4f}\n"
            f"  Interpretar: el robot recorría {(1/self.dist_factor)*100:.1f}% de lo pedido"
        )
        self._publish_status("CAL_DONE", {
            "type": "distance",
            "dist_factor": self.dist_factor,
            "measurements": self._cal_measurements,
        })

    # ── CALIBRACIÓN ANGULAR ──────────────────────────────────────────
    # Protocolo:
    #   1. Rota 360° completos (reps veces)
    #   2. Mide el error acumulado con una marca en el suelo
    #   3. Calcula angle_factor

    def _start_calibrate_angle(self, reps: int):
        with self.state_lock:
            if self.state != self.IDLE:
                self.get_logger().warn("Busy, cannot start calibration.")
                return
            self.state = self.CAL_ANG

        self._cal_reps_total   = reps
        self._cal_reps_done    = 0
        self._cal_measurements = []
        self._cal_target       = deg2rad(360.0)
        self._cal_waiting_measure = False
        self._cal_total_odom_angle = 0.0
        self._cal_th_start = self._get_pose()[2]

        self.get_logger().info(
            f"=== ANGULAR CALIBRATION — {reps}×360° ==="
        )
        self._cal_do_next_angle_rep()

    def _cal_do_next_angle_rep(self):
        self._cal_waiting_measure = False
        x, y, th = self._get_pose()
        self.start_th = th
        self.goal_angle = self._cal_target
        self._send_vel(0.0, self.angular_speed)
        self._publish_status("CAL_ROTATING", {
            "rep": self._cal_reps_done + 1,
            "total": self._cal_reps_total,
        })

    def _calibration_angular_loop(self, x, y, th):
        if self._cal_waiting_measure:
            return

        rotated = abs(normalize_angle(th - self.start_th))
        if rotated >= self._cal_target - deg2rad(self.angle_tolerance):
            self._stop_motors()
            self._cal_reps_done += 1
            self._cal_total_odom_angle += rotated

            if self._cal_reps_done >= self._cal_reps_total:
                self._cal_waiting_measure = True
                self._publish_status("CAL_MEASURE", {
                    "rep": self._cal_reps_done,
                    "total": self._cal_reps_total,
                    "odom_total_deg": rad2deg(self._cal_total_odom_angle),
                    "instruction": (
                        f"Observa cuántos grados faltó o sobró el robot para cerrar "
                        f"{self._cal_reps_total} vueltas completas. "
                        f"Publica: {{\"cmd\":\"cal_measure\",\"measured_deg\": <valor_real>}}"
                    ),
                })
            else:
                time.sleep(0.3)
                self._cal_do_next_angle_rep()

    def _receive_cal_measure_angular(self, measured_deg: float):
        """Recibe medición del operador durante calibración angular."""
        measured_rad = deg2rad(measured_deg)
        expected_rad = self._cal_target * self._cal_reps_total
        factor = self._cal_total_odom_angle / measured_rad if measured_rad > 0 else 1.0
        self.angle_factor = factor
        with self.state_lock:
            self.state = self.IDLE
        self.get_logger().info(
            f"=== CALIBRACIÓN ANGULAR COMPLETA ===\n"
            f"  Odom total: {rad2deg(self._cal_total_odom_angle):.1f}°\n"
            f"  Físico medido: {measured_deg:.1f}°\n"
            f"  angle_factor aplicado: {self.angle_factor:.4f}"
        )
        self._publish_status("CAL_DONE", {
            "type": "angle",
            "angle_factor": self.angle_factor,
        })

    # ── RECEPCIÓN DE MEDICIONES DE CALIBRACIÓN ───────────────────────

    def _dispatch_command(self, cmd: dict):
        action = cmd.get("cmd", "")

        # Mediciones de calibración — se procesan en cualquier estado CAL
        if action == "cal_measure":
            with self.state_lock:
                state = self.state
            if state == self.CAL_LIN:
                self._receive_cal_measure_linear(float(cmd.get("measured_m", 0)))
            elif state == self.CAL_ANG:
                self._receive_cal_measure_angular(float(cmd.get("measured_deg", 0)))
            return

        if action == "stop":
            self._stop_motors()
            with self.state_lock:
                self.state = self.IDLE
            self._publish_status("STOPPED", {})

        elif action == "move":
            dist = float(cmd.get("distance", 0.0))
            self._start_move(dist)

        elif action == "rotate":
            angle_deg = float(cmd.get("angle", 0.0))
            self._start_rotate(deg2rad(angle_deg))

        elif action == "rotate_to":
            heading_deg = float(cmd.get("heading", 0.0))
            snapped = nearest_octal(heading_deg)
            if abs(snapped - heading_deg) > 2:
                self.get_logger().warn(
                    f"rotate_to {heading_deg}° → snapped to octal {snapped}°"
                )
            _, _, th = self._get_pose()
            current_deg = rad2deg(th) % 360
            delta = normalize_angle(deg2rad(snapped - current_deg))
            self._start_rotate(delta)

        elif action == "calibrate_distance":
            reps = int(cmd.get("reps", 3))
            self._start_calibrate_distance(reps)

        elif action == "calibrate_angle":
            reps = int(cmd.get("reps", 4))
            self._start_calibrate_angle(reps)

        elif action == "set_cal":
            if "dist_factor" in cmd:
                self.dist_factor = float(cmd["dist_factor"])
            if "angle_factor" in cmd:
                self.angle_factor = float(cmd["angle_factor"])
            self.get_logger().info(
                f"Cal set: dist={self.dist_factor:.4f} angle={self.angle_factor:.4f}"
            )
            self._publish_status("CAL_SET", {
                "dist_factor": self.dist_factor,
                "angle_factor": self.angle_factor,
            })

        else:
            self.get_logger().warn(f"Unknown command: {action}")

    # ── MOTOR HELPERS ────────────────────────────────────────────────

    def _send_vel(self, v: float, w: float):
        msg = Twist()
        msg.linear.x  = v
        msg.angular.z = w
        self.cmd_pub.publish(msg)

    def _stop_motors(self):
        self._send_vel(0.0, 0.0)

    # ── STATUS PUBLISHER ─────────────────────────────────────────────

    def _publish_status(self, state: str, data: dict):
        payload = {"state": state, **data}
        msg = String()
        msg.data = json.dumps(payload)
        self.status_pub.publish(msg)
        self.get_logger().info(f"[STATUS] {payload}")


def main(args=None):
    rclpy.init(args=args)
    node = MotionController()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node._stop_motors()
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()