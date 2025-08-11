import cv2
import os
import json

DEVICE = "/dev/video0"
CONFIG_FILE = "camera_config.json"

previous_controls = {"exposure": -1, "wb_temp": -1, "gain": -1}

def set_manual_controls(exposure, wb_temp, gain):
    if exposure != previous_controls["exposure"]:
        os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=exposure_auto=1")
        os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=exposure_absolute={exposure}")
        print(exposure)
        previous_controls["exposure"] = exposure
    if wb_temp != previous_controls["wb_temp"]:
        os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=white_balance_temperature_auto=0")
        os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=white_balance_temperature={wb_temp}")
        print(wb_temp)
        previous_controls["wb_temp"] = wb_temp
    if gain != previous_controls["gain"]:
        os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=gain={gain}")
        print(gain)
        previous_controls["gain"] = gain

def load_config():
    if os.path.exists(CONFIG_FILE):
        with open(CONFIG_FILE, 'r') as f:
            try:
                config = json.load(f)
                print("Configuration chargée :", config)
                return config
            except Exception as e:
                print("Erreur lecture config:", e)
    return {"exposure": 157, "wb_temp": 4600, "gain": 0, "mirror": 0}

def save_config(exposure, wb_temp, gain, mirror):
    config = {
        "exposure": exposure,
        "wb_temp": wb_temp,
        "gain": gain,
        "mirror": mirror
    }
    with open(CONFIG_FILE, 'w') as f:
        json.dump(config, f)
    print("Configuration sauvegardée :", config)

def nothing(x):
    pass

# Charger la configuration initiale
config = load_config()

# Fenêtre et sliders
cv2.namedWindow("Camera Controls", cv2.WINDOW_NORMAL)
cv2.createTrackbar("Exposure", "Camera Controls", config["exposure"], 5000, nothing)
cv2.createTrackbar("WhiteBalance", "Camera Controls", config["wb_temp"], 6500, nothing)
cv2.createTrackbar("Gain", "Camera Controls", config["gain"], 255, nothing)
cv2.createTrackbar("Mirror", "Camera Controls", config["mirror"], 1, nothing)

cap = cv2.VideoCapture(0)

try:
    while True:
        exposure = cv2.getTrackbarPos("Exposure", "Camera Controls")
        wb_temp = cv2.getTrackbarPos("WhiteBalance", "Camera Controls")
        gain = cv2.getTrackbarPos("Gain", "Camera Controls")
        mirror = cv2.getTrackbarPos("Mirror", "Camera Controls")

        set_manual_controls(exposure, wb_temp, gain)

        ret, frame = cap.read()
        if not ret:
            print("Erreur de lecture vidéo.")
            break

        if mirror:
            frame = cv2.flip(frame, 1)

        cv2.imshow("Camera Controls", frame)

        key = cv2.waitKey(100) & 0xFF
        if key == ord('q'):
            break
        elif key == ord('s'):
            save_config(exposure, wb_temp, gain, mirror)

except KeyboardInterrupt:
    print("Arrêt manuel détecté")

finally:
    cap.release()
    cv2.destroyAllWindows()
