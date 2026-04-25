from ultralytics import YOLO
from picamera2 import Picamera2, Preview
from libcamera import controls
import cv2

model = YOLO("best_ncnn_model", task="detect")

picam2 = Picamera2()
config = picam2.create_preview_configuration(
    main={"format": "BGR888", "size": (640, 480)}
)
picam2.configure(config)
picam2.start(show_preview=False)  # Sin preview DRM
picam2.set_controls({"AfMode": controls.AfModeEnum.Continuous})

print("Presiona 'q' para salir.")

while True:
    frame = picam2.capture_array()
    results = model(frame, conf=0.5, verbose=False)
    annotated = results[0].plot()
    cv2.imshow("Cubos - NCNN", annotated)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

picam2.stop()
cv2.destroyAllWindows()