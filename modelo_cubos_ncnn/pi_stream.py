"""
pi_stream.py
------------
Captures from Pi Camera Module 3 and serves a raw MJPEG stream.
No inference here — keeps the Pi light.
The PC connects and does all the YOLO work.

Usage (on the Pi):
    python3 pi_stream.py

Stream URL for the PC:
    http://<PI_IP>:8888/stream
"""

import sys
from unittest.mock import MagicMock
sys.modules['pykms'] = MagicMock()
sys.path.insert(0, '/usr/lib/python3/dist-packages')

import cv2
import threading
import time
from http.server import BaseHTTPRequestHandler, HTTPServer
from picamera2 import Picamera2
from libcamera import controls as libcamera_controls

# ── CONFIG ─────────────────────────────────────────────
STREAM_PORT  = 8888
FRAME_WIDTH  = 640
FRAME_HEIGHT = 480
JPEG_QUALITY = 80
# ───────────────────────────────────────────────────────

latest_frame = None
frame_lock   = threading.Lock()


def capture_loop():
    global latest_frame

    picam2 = Picamera2()
    config = picam2.create_preview_configuration(
        main={"format": "BGR888", "size": (FRAME_WIDTH, FRAME_HEIGHT)}
    )
    picam2.configure(config)
    picam2.start(show_preview=False)
    time.sleep(1)

    try:
        picam2.set_controls({"AfMode": libcamera_controls.AfModeEnum.Continuous})
        print("[INFO] Autofocus: continuous")
    except Exception:
        print("[WARN] Autofocus not available")

    print("[INFO] Camera ready, capturing...")

    while True:
        frame = picam2.capture_array()
        frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)  # fix daltonism
        _, jpeg = cv2.imencode(".jpg", frame, [cv2.IMWRITE_JPEG_QUALITY, JPEG_QUALITY])
        with frame_lock:
            latest_frame = jpeg.tobytes()


class MJPEGHandler(BaseHTTPRequestHandler):
    def log_message(self, fmt, *args):
        pass

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
                    time.sleep(0.033)
            except (BrokenPipeError, ConnectionResetError):
                pass
        else:
            self.send_response(404)
            self.end_headers()


if __name__ == "__main__":
    t = threading.Thread(target=capture_loop, daemon=True)
    t.start()

    print("[INFO] Waiting for first frame...")
    while latest_frame is None:
        time.sleep(0.1)

    server = HTTPServer(("0.0.0.0", STREAM_PORT), MJPEGHandler)
    print(f"[INFO] Streaming at http://<PI_IP>:{STREAM_PORT}/stream")
    print(f"[INFO] Pi IP: run 'hostname -I'")
    print(f"[INFO] Ctrl+C to stop.\n")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\n[INFO] Stopped.")