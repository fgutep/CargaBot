"""
stream_ncnn.py
--------------
Runs YOLO NCNN inference on the Pi camera and serves an MJPEG stream
you can watch from any browser on your PC.

Usage (on the Pi):
    source .venv/bin/activate
    python3 stream_ncnn.py

Then on your PC open:
    http://<PI_IP>:8080/stream

Find your Pi IP with: hostname -I
"""

import cv2
import threading
import time
from http.server import BaseHTTPRequestHandler, HTTPServer
from ultralytics import YOLO

# ── CONFIG ────────────────────────────────────────────────
MODEL_PATH   = "best_ncnn_model"   # folder from the NCNN export
CAMERA_INDEX = 0                   # 0 = first camera; try 1 if Pi Camera v3
CONF_THRESH  = 0.5
STREAM_PORT  = 8080
FRAME_WIDTH  = 640
FRAME_HEIGHT = 480
# Colors per class (BGR) — adjust class names to match yours
CLASS_COLORS = {
    "Cubo Azul":  (255, 100,   0),
    "Cubo Rojo":  (  0,   0, 220),
    "Cubo Verde": (  0, 200,   0),
}
# ─────────────────────────────────────────────────────────


# Shared latest frame (thread-safe via lock)
latest_frame = None
frame_lock   = threading.Lock()


def inference_loop():
    """Captures frames, runs YOLO, stores annotated result."""
    global latest_frame

    model = YOLO(MODEL_PATH, task="detect")
    cap   = cv2.VideoCapture(CAMERA_INDEX)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH,  FRAME_WIDTH)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT)

    if not cap.isOpened():
        print(f"[ERROR] Cannot open camera {CAMERA_INDEX}.")
        print("  Try: sudo usermod -aG video $USER  then re-login")
        return

    print(f"[INFO] Camera open. Streaming on http://0.0.0.0:{STREAM_PORT}/stream")

    fps_time = time.time()
    fps_count = 0
    fps_display = 0.0

    while True:
        ret, frame = cap.read()
        if not ret:
            print("[WARN] Frame grab failed, retrying...")
            time.sleep(0.1)
            continue

        results = model(frame, conf=CONF_THRESH, verbose=False)[0]

        # Draw detections manually so we control style
        for box in results.boxes:
            x1, y1, x2, y2 = map(int, box.xyxy[0])
            conf  = float(box.conf[0])
            cls   = results.names[int(box.cls[0])]
            color = CLASS_COLORS.get(cls, (200, 200, 200))
            label = f"{cls}  {conf*100:.0f}%"

            cv2.rectangle(frame, (x1, y1), (x2, y2), color, 2)
            # Label background
            (tw, th), _ = cv2.getTextSize(label, cv2.FONT_HERSHEY_SIMPLEX, 0.55, 2)
            cv2.rectangle(frame, (x1, y1 - th - 8), (x1 + tw + 4, y1), color, -1)
            cv2.putText(frame, label, (x1 + 2, y1 - 4),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.55, (255, 255, 255), 2)

        # FPS counter
        fps_count += 1
        if time.time() - fps_time >= 1.0:
            fps_display = fps_count / (time.time() - fps_time)
            fps_count = 0
            fps_time  = time.time()

        cv2.putText(frame, f"FPS: {fps_display:.1f}", (8, 22),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)

        # Encode to JPEG and store
        _, jpeg = cv2.imencode(".jpg", frame, [cv2.IMWRITE_JPEG_QUALITY, 80])
        with frame_lock:
            latest_frame = jpeg.tobytes()

    cap.release()


class MJPEGHandler(BaseHTTPRequestHandler):
    def log_message(self, fmt, *args):
        pass  # suppress per-request console noise

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
                    time.sleep(0.033)  # ~30 fps cap to not overload network
            except (BrokenPipeError, ConnectionResetError):
                pass  # client disconnected

        elif self.path == "/":
            # Simple viewer page — open this in your browser
            html = f"""<!DOCTYPE html>
<html>
<head>
  <title>Cubos YOLO Stream</title>
  <style>
    body {{ background:#111; display:flex; flex-direction:column;
            align-items:center; justify-content:center; height:100vh; margin:0; }}
    h2   {{ color:#eee; font-family:sans-serif; margin-bottom:12px; }}
    img  {{ border: 2px solid #444; border-radius:6px; max-width:95vw; }}
  </style>
</head>
<body>
  <h2>🟥 🟦 🟩 Cubos YOLO — Live Stream</h2>
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

    # Wait for first frame
    print("[INFO] Waiting for first frame...")
    while latest_frame is None:
        time.sleep(0.1)

    # Start HTTP server
    server = HTTPServer(("0.0.0.0", STREAM_PORT), MJPEGHandler)
    print(f"[INFO] Server ready.")
    print(f"[INFO] Open on your PC:  http://<PI_IP>:{STREAM_PORT}/")
    print(f"[INFO] Find Pi IP with:  hostname -I")
    print(f"[INFO] Press Ctrl+C to stop.\n")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\n[INFO] Stopped.")
