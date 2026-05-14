#!/usr/bin/env python3
"""
CargaBot Calibration CLI — sin race conditions
/usr/bin/python3.12 cargabot_cal.py
"""
import json, threading, sys, math, queue
import rclpy
from rclpy.node import Node
from std_msgs.msg import String
from nav_msgs.msg import Odometry

_input_q = queue.Queue()

def _stdin_reader():
    while True:
        try:
            line = sys.stdin.readline()
            if line == "":
                _input_q.put(None); break
            _input_q.put(line.strip())
        except Exception:
            break

class CalCLI(Node):
    PROMPT  = "\033[1;32mcargabot>\033[0m "
    MEASURE = "\033[1;33m  valor>\033[0m  "

    def __init__(self):
        super().__init__("cargabot_cal_cli")
        self.pub = self.create_publisher(String, "/cargabot/move_cmd", 10)
        self.create_subscription(String,   "/cargabot/move_status", self.on_status, 10)
        self.create_subscription(Odometry, "/odom",                 self.on_odom,   10)
        self.x = self.y = self.th = 0.0
        self.cal_type        = ""
        self.waiting_measure = False
        self.create_timer(0.05, self.tick)
        print("\n\033[1;32m=== CARGABOT CAL CLI ===\033[0m")
        print("  move <m>          mover X metros")
        print("  rot  <deg>        rotar X grados (+ CCW)")
        print("  cal_lin <n>       calibración lineal N reps de 1m")
        print("  cal_ang <n>       calibración angular N x 360°")
        print("  set_cal <df> <af> aplicar factores manuales")
        print("  odom              posición actual")
        print("  stop / q\n")
        self._prompt()

    def _prompt(self):
        p = self.MEASURE if self.waiting_measure else self.PROMPT
        print(p, end="", flush=True)

    def send(self, d):
        msg = String(); msg.data = json.dumps(d)
        self.pub.publish(msg)
        print(f"\033[2m  → {json.dumps(d)}\033[0m")

    def tick(self):
        try:
            raw = _input_q.get_nowait()
        except queue.Empty:
            return

        if raw is None:
            self.send({"cmd": "stop"}); rclpy.shutdown(); return

        raw = raw.strip()
        if not raw:
            self._prompt(); return

        if self.waiting_measure:
            try:
                val = float(raw)
            except ValueError:
                print("\033[31m  Valor inválido\033[0m")
                self._prompt(); return
            self.waiting_measure = False
            key = "measured_m" if self.cal_type == "distance" else "measured_deg"
            self.send({"cmd": "cal_measure", key: val})
            self._prompt(); return

        parts = raw.split()
        cmd   = parts[0].lower()

        if cmd in ("q", "exit"):
            self.send({"cmd": "stop"}); rclpy.shutdown(); return
        elif cmd == "stop":
            self.send({"cmd": "stop"})
        elif cmd == "odom":
            print(f"  X={self.x:+.4f}m  Y={self.y:+.4f}m  HDG={self.th:.1f}°")
        elif cmd == "move" and len(parts) > 1:
            self.send({"cmd": "move", "distance": float(parts[1])})
        elif cmd == "rot" and len(parts) > 1:
            self.send({"cmd": "rotate", "angle": float(parts[1])})
        elif cmd == "cal_lin":
            self.send({"cmd": "calibrate_distance", "reps": int(parts[1]) if len(parts)>1 else 3})
        elif cmd == "cal_ang":
            self.send({"cmd": "calibrate_angle", "reps": int(parts[1]) if len(parts)>1 else 4})
        elif cmd == "set_cal" and len(parts) >= 3:
            self.send({"cmd": "set_cal", "dist_factor": float(parts[1]), "angle_factor": float(parts[2])})
        else:
            print(f"  Comando no reconocido: '{raw}'")
        self._prompt()

    def on_odom(self, msg):
        q = msg.pose.pose.orientation
        self.x  = msg.pose.pose.position.x
        self.y  = msg.pose.pose.position.y
        self.th = math.degrees(math.atan2(2*(q.w*q.z+q.x*q.y), 1-2*(q.y*q.y+q.z*q.z))) % 360

    def on_status(self, msg):
        try: s = json.loads(msg.data)
        except: return
        state = s.get("state","")

        if state == "CAL_MOVING":
            print(f"\n\033[33m  [CAL] Moviendo... rep {s.get('rep')}/{s.get('total')}\033[0m")
            self._prompt()
        elif state == "CAL_ROTATING":
            print(f"\n\033[33m  [CAL] Rotando... rep {s.get('rep')}/{s.get('total')}\033[0m")
            self._prompt()
        elif state == "CAL_MEASURE":
            is_lin = "odom_m" in s
            oval   = s.get("odom_m", s.get("odom_total_deg","?"))
            unit   = "m" if is_lin else "°"
            self.cal_type        = "distance" if is_lin else "angle"
            self.waiting_measure = True
            print(f"\n\033[1;33m  ⚠  MIDE AHORA — Rep {s.get('rep')}/{s.get('total')}\033[0m")
            print(f"     Odom: {oval:.4f}{unit}")
            print(f"     Ingresa {'metros' if is_lin else 'grados'} físicos medidos:")
            self._prompt()
        elif state == "CAL_DONE":
            t = s.get("type","")
            if t == "distance":
                print(f"\n\033[1;32m  ✓ CAL LINEAL LISTA — dist_factor={s.get('dist_factor',0):.4f}\033[0m\n")
            else:
                print(f"\n\033[1;32m  ✓ CAL ANGULAR LISTA — angle_factor={s.get('angle_factor',0):.4f}\033[0m\n")
            self._prompt()
        elif state == "CAL_SET":
            print(f"\n\033[32m  ✓ dist={s.get('dist_factor','?'):.4f}  angle={s.get('angle_factor','?'):.4f}\033[0m")
            self._prompt()
        elif state == "DONE":
            print(f"\n\033[32m  ✓ Completado\033[0m"); self._prompt()
        elif state == "STOPPED":
            print(f"\n\033[31m  ■ STOP\033[0m"); self._prompt()
        elif state in ("MOVING","ROTATING"):
            print(f"\n\033[36m  [{state}]\033[0m"); self._prompt()

def main():
    t = threading.Thread(target=_stdin_reader, daemon=True)
    t.start()
    rclpy.init()
    node = CalCLI()
    try:
        rclpy.spin(node)
    except (KeyboardInterrupt, SystemExit):
        pass
    finally:
        try: node.destroy_node()
        except: pass
        try: rclpy.shutdown()
        except: pass

if __name__ == "__main__":
    main()