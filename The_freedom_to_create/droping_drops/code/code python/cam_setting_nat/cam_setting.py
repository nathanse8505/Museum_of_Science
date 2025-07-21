import cv2
import os
#sudo apt update
#sudo apt install v4l-utils
#v4l2-ctl --list-devices
#v4l2-ctl -d /dev/video0 --list-ctrls

DEVICE = "/dev/video0"

# Initialisation des réglages manuels
def set_manual_controls(exposure, wb_temp, gain):
    os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=exposure_auto=1")  # Mode manuel
    os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=white_balance_temperature_auto=0")
    os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=exposure_absolute={exposure}")
    os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=white_balance_temperature={wb_temp}")
    os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=gain={gain}")

# Fonctions de callback (obligatoires mais inutiles ici)
def nothing(x):
    pass

# Créer une fenêtre avec sliders
cv2.namedWindow("Camera Controls", cv2.WINDOW_NORMAL)
cv2.createTrackbar("Exposure", "Camera Controls", 150, 1000, nothing)
cv2.createTrackbar("WhiteBalance", "Camera Controls", 4500, 6500, nothing)
cv2.createTrackbar("Gain", "Camera Controls", 10, 255, nothing)
cv2.createTrackbar("Mirror", "Camera Controls", 0, 1, nothing)

# Démarrer la capture
cap = cv2.VideoCapture(0)

while True:
    # Lire valeurs sliders
    exposure = cv2.getTrackbarPos("Exposure", "Camera Controls")
    wb_temp = cv2.getTrackbarPos("WhiteBalance", "Camera Controls")
    gain = cv2.getTrackbarPos("Gain", "Camera Controls")
    mirror = cv2.getTrackbarPos("Mirror", "Camera Controls")

    # Appliquer les réglages
    set_manual_controls(exposure, wb_temp, gain)

    ret, frame = cap.read()
    if not ret:
        break

    if mirror:
        frame = cv2.flip(frame, 1)

    cv2.imshow("Camera Controls", frame)

    if cv2.waitKey(100) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
