import cv2
import numpy as np
import tkinter as tk

# Obtenir la taille de l'écran
def get_screen_resolution():
    root = tk.Tk()
    root.withdraw()
    width = root.winfo_screenwidth()
    height = root.winfo_screenheight()
    root.destroy()
    return width, height

screen_width, screen_height = get_screen_resolution()

# Charger l'image de fumée
smoke = cv2.imread(r"C:\Users\nathans\Desktop\Museum_of_Science\energy\Light_a_Fire\graphics\video\new_smoke.png")
if smoke is None:
    raise FileNotFoundError("L’image de fumée est introuvable.")

# Redimensionner la fumée
smoke_width, smoke_height = 300, 300
smoke = cv2.resize(smoke, (smoke_width, smoke_height))

# ✅ Créer un canal alpha à partir du fond noir (même très sombre)
lower_black = np.array([0, 0, 0])
upper_black = np.array([50, 50, 50])  # tolérance du noir
mask = cv2.inRange(smoke, lower_black, upper_black)
alpha = 255 - mask  # inverser : le noir devient transparent

# Ajouter l’alpha à l’image
smoke_rgba = cv2.cvtColor(smoke, cv2.COLOR_BGR2BGRA)
smoke_rgba[:, :, 3] = alpha

# Ouvrir la caméra
cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, screen_width)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, screen_height)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    frame = cv2.resize(frame, (screen_width, screen_height))
    frame_rgba = cv2.cvtColor(frame, cv2.COLOR_BGR2BGRA)

    # Position de la fumée (centre)
    x_offset = (screen_width - smoke_width) // 2
    y_offset = (screen_height - smoke_height) // 2

    roi = frame_rgba[y_offset:y_offset + smoke_height, x_offset:x_offset + smoke_width]
    alpha_smoke = smoke_rgba[:, :, 3:] / 255.0
    alpha_frame = 1.0 - alpha_smoke

    for c in range(3):
        roi[:, :, c] = (alpha_smoke[:, :, 0] * smoke_rgba[:, :, c] +
                        alpha_frame[:, :, 0] * roi[:, :, c])

    frame_rgba[y_offset:y_offset + smoke_height, x_offset:x_offset + smoke_width] = roi

    final = cv2.cvtColor(frame_rgba, cv2.COLOR_BGRA2BGR)
    cv2.imshow("Caméra + Fumée (sans carré blanc)", final)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
