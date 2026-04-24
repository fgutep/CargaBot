# test_ncnn.py
from ultralytics import YOLO
import cv2

# Apunta a la carpeta extraída del zip
model = YOLO("best_ncnn_model", task="detect")  # carpeta, no archivo


# --- Opción B: webcam en vivo ---
cap = cv2.VideoCapture(0)
while True:
    ret, frame = cap.read()
    if not ret:
        break
    results = model(frame, conf=0.5, verbose=False)
    annotated = results[0].plot()
    cv2.imshow("Cubos - NCNN", annotated)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()