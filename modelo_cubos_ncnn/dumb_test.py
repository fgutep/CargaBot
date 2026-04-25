import cv2

cap = cv2.VideoCapture("/dev/video13", cv2.CAP_V4L2)

while True:
    ret, frame = cap.read()
    if not ret:
        print("No se pudo leer frame")
        break
    cv2.imshow("Camara", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()