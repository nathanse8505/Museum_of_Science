import cv2
import os

DEVICE = "/dev/video0"


previous_controls = {"exposure": -1, "wb_temp": -1, "gain": -1}

def set_manual_controls(exposure, wb_temp, gain):
    if exposure != previous_controls["exposure"]:
        os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=exposure_absolute={exposure}")
        previous_controls["exposure"] = exposure
    if wb_temp != previous_controls["wb_temp"]:
        os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=white_balance_temperature={wb_temp}")
        previous_controls["wb_temp"] = wb_temp
    if gain != previous_controls["gain"]:
        os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=gain={gain}")
        previous_controls["gain"] = gain


# Callback pour les sliders
def nothing(x):
    pass


# Créer la fenêtre
cv2.namedWindow("Camera Controls", cv2.WINDOW_NORMAL)
cv2.createTrackbar("Exposure", "Camera Controls", 157, 5000, nothing)
cv2.createTrackbar("WhiteBalance", "Camera Controls", 4600, 6500, nothing)
cv2.createTrackbar("Gain", "Camera Controls", 0, 255, nothing)
cv2.createTrackbar("Mirror", "Camera Controls", 0, 1, nothing)

# Ouvrir la caméra
cap = cv2.VideoCapture(0)

try:
    while True:
        exposure = cv2.getTrackbarPos("Exposure", "Camera Controls")
        wb_temp = cv2.getTrackbarPos("WhiteBalance", "Camera Controls")
        gain = cv2.getTrackbarPos("Gain", "Camera Controls")
        mirror = cv2.getTrackbarPos("Mirror", "Camera Controls")

        # Appliquer les réglages
        set_manual_controls(exposure, wb_temp, gain)

        ret, frame = cap.read()
        if not ret:
            print("Erreur de lecture vidéo.")
            break

        if mirror:
            frame = cv2.flip(frame, 1)

        cv2.imshow("Camera Controls", frame)

        if cv2.waitKey(100) & 0xFF == ord('q'):
            break

except KeyboardInterrupt:
    print("Arrêt manuel détecté")

finally:
    cap.release()
    cv2.destroyAllWindows()
