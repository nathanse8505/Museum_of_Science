import cv2
import os

DEVICE = "/dev/video0"


# Initialisation des réglages manuels
def set_manual_controls(exposure, wb_temp, gain):
    # Passer en mode manuel pour l'exposition et la balance des blancs
    os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=auto_exposure=1")  # 1 = Manual Mode
    os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=white_balance_automatic=0")

    # Appliquer les valeurs
    os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=exposure_time_absolute={exposure}")
    os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=white_balance_temperature={wb_temp}")
    os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=gain={gain}")


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
