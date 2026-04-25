import sys
from unittest.mock import MagicMock
sys.modules['pykms'] = MagicMock()
sys.path.insert(0, '/usr/lib/python3/dist-packages')

from picamera2 import Picamera2
import cv2

picam2 = Picamera2()
config = picam2.create_preview_configuration(main={'format': 'BGR888', 'size': (640, 480)})
picam2.configure(config)
picam2.start(show_preview=False)

import time; time.sleep(1)
print("Camara lista, abriendo ventana...")

while True:
    frame = picam2.capture_array()
    cv2.imshow('Test Camara', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

picam2.stop()
cv2.destroyAllWindows()
