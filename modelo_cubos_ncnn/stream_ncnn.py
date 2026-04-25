"""
stream_ncnn.py
--------------
Runs YOLO NCNN inference on Pi Camera Module 3 and serves an MJPEG stream
viewable from any browser on your PC over the local network.

Usage (on the Pi):
    python3 stream_ncnn.py

Then on your PC open:
    http://<PI_IP>:8080/

Find Pi IP with: hostname -I
"""

# ── Fixes for Ubuntu on Pi 5 ──────────────────────────────
# 1. Mock pykms so picamera2 doesn't crash looking for it
# 2. Add apt libcamera path so picamera2 finds the libcamera module
import sys
from unittest.mock import MagicMock
sys.modules['pykms'] = MagicMock()
sys.path.insert(0, '/usr/lib/python3/dist-packages')
# ─────────────────────────────────────────────────────────

import cv2
import threading
import time
from http.server import BaseHTTPRequestHandler, HTTPServer
from ultralytics import YOLO
from picamera2 import Picamera2
from libcamera import controls as libcamera_controls

# ── CONFIG ────────────────────────────────────────────────
MODEL_PATH   = "best_ncnn_model"   # NCNN export folder
CONF_THRESH  = 0.5
STREAM_PORT  = 8080
FRAME_WIDTH  = 640
FRAME_HEIGHT = 480
CLASS_COLORS = {
    "Cubo Azul":  (255, 100,   0),
    "Cubo Rojo":  (  0,   0, 220),
    "Cubo Verde": (  0, 200,   0),
}
# ─────────────────────────────────────────────────────────

latest_frame = None
frame_lock   = threading.Lock()


def inference_loop():
    global latest_frame

    # Load YOLO model
    model = YOLO(MODEL_PATH, task="detect")
    print(f"[INFO] Model loaded: {MODEL_PATH}")

    # Init Pi Camera Module 3
    picam2 = Picamera2()
    config = picam2.create_preview_configuration(
        main={"format": "BGR888", "size": (FRAME_WIDTH, FRAME_HEIGHT)}
    )
    picam2.configure(config)
    picam2.start(show_preview=False)
    time.sleep(1)  # warm up

    # Enable continuous autofocus (Module 3 supports this)
    try:
        picam2.set_controls({"AfMode": libcamera_controls.AfModeEnum.Continuous})
        print("[INFO] Autofocus: continuous")
    except Exception:
        print("[WARN] Autofocus not available, continuing without it")

    print(f"[INFO] Camera ready. Starting inference loop...")

    fps_time  = time.time()
    fps_count = 0
    fps_disp  = 0.0

    while True:
        # Grab frame from Pi Camera
        frame = picam2.capture_array()
        frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)  
        # desinvertir colores

        # Run YOLO inference
        results = model(frame, conf=CONF_THRESH, verbose=False)[0]

        # Draw detections
        for box in results.boxes:
            x1, y1, x2, y2 = map(int, box.xyxy[0])
            conf  = float(box.conf[0])
            cls   = results.names[int(box.cls[0])]
            color = CLASS_COLORS.get(cls, (200, 200, 200))
            label = f"{cls}  {conf*100:.0f}%"

            cv2.rectangle(frame, (x1, y1), (x2, y2), color, 2)
            (tw, th), _ = cv2.getTextSize(label, cv2.FONT_HERSHEY_SIMPLEX, 0.55, 2)
            cv2.rectangle(frame, (x1, y1 - th - 8), (x1 + tw + 4, y1), color, -1)
            cv2.putText(frame, label, (x1 + 2, y1 - 4),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.55, (255, 255, 255), 2)

        # FPS overlay
        fps_count += 1
        elapsed = time.time() - fps_time
        if elapsed >= 1.0:
            fps_disp  = fps_count / elapsed
            fps_count = 0
            fps_time  = time.time()

        cv2.putText(frame, f"FPS: {fps_disp:.1f}", (8, 22),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)

        # Encode to JPEG and store for HTTP server
        _, jpeg = cv2.imencode(".jpg", frame, [cv2.IMWRITE_JPEG_QUALITY, 80])
        with frame_lock:
            latest_frame = jpeg.tobytes()


class MJPEGHandler(BaseHTTPRequestHandler):
    def log_message(self, fmt, *args):
        pass  # suppress per-request console spam

    def do_GET(self):
        if self.path == "/stream":
            self.send_response(200)
            self.send_header("Content-Type", "multipart/x-mixed-replace; boundary=frame")
            self.end_headers()
            try:
                while True:
                    with frame_lock:
                        frame = latest_frame
                    if frame is None:
                        time.sleep(0.05)
                        continue
                    self.wfile.write(b"--frame\r\n")
                    self.wfile.write(b"Content-Type: image/jpeg\r\n\r\n")
                    self.wfile.write(frame)
                    self.wfile.write(b"\r\n")
                    time.sleep(0.033)  # ~30 fps cap
            except (BrokenPipeError, ConnectionResetError):
                pass  # client disconnected, no problem

        elif self.path == "/":
            html = """<!DOCTYPE html>
<html>
<head>
  <title>CargaBot — Cubos YOLO</title>
  <style>
    body { background:#111; display:flex; flex-direction:column;
           align-items:center; justify-content:center; height:100vh; margin:0; }
    h2   { color:#eee; font-family:sans-serif; margin-bottom:12px; }
    img  { border:2px solid #444; border-radius:6px; max-width:95vw; }
  </style>
</head>
<body>
  <h2>🟥 🟦 🟩 CargaBot — Live Stream</h2>
  <img src="/stream" />
</body>
</html>"""
            self.send_response(200)
            self.send_header("Content-Type", "text/html")
            self.end_headers()
            self.wfile.write(html.encode())
        else:
            self.send_response(404)
            self.end_headers()


if __name__ == "__main__":
    # Start inference in background thread
    t = threading.Thread(target=inference_loop, daemon=True)
    t.start()

    # Wait for first frame before accepting connections
    print("[INFO] Waiting for first frame...")
    while latest_frame is None:
        time.sleep(0.1)

    server = HTTPServer(("0.0.0.0", STREAM_PORT), MJPEGHandler)
    print(f"[INFO] Server ready!")
    print(f"[INFO] Open on your PC: http://<PI_IP>:{STREAM_PORT}/")
    print(f"[INFO] Find Pi IP with: hostname -I")
    print(f"[INFO] Press Ctrl+C to stop.\n")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\n[INFO] Stopped.")