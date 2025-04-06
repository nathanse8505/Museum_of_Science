import cv2
import numpy as np
import tkinter as tk

# 📏 Fonction pour détecter la résolution de l'écran
def get_screen_resolution():
    root = tk.Tk()
    root.withdraw()  # Ne pas afficher de fenêtre
    width = root.winfo_screenwidth()
    height = root.winfo_screenheight()
    root.destroy()
    return width, height

# 📐 Obtenir la taille de l'écran
screen_width, screen_height = get_screen_resolution()
print(f"Résolution écran : {screen_width}x{screen_height}")

# 🌫 Charger une image de fumée avec fond noir
smoke = cv2.imread(r"C:\Users\nathans\Desktop\Museum_of_Science\energy\Light_a_Fire\graphics\video\flame 50\extracted_frames\frame_49.png")  # remplace par le chemin correct
smoke = cv2.resize(smoke, (screen_width, screen_height))

# 🧼 Supprimer le fond noir → créer un canal alpha
threshold = 30
black_mask = cv2.inRange(smoke, (0, 0, 0), (threshold, threshold, threshold))
alpha_mask = 255 - black_mask

# ➕ Ajouter le canal alpha à l’image de fumée
smoke_rgba = cv2.cvtColor(smoke, cv2.COLOR_BGR2BGRA)
smoke_rgba[:, :, 3] = alpha_mask

# 📷 Ouvrir la caméra
cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, screen_width)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, screen_height)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    # Redimensionner le frame à l'écran
    frame = cv2.resize(frame, (screen_width, screen_height))
    frame_rgba = cv2.cvtColor(frame, cv2.COLOR_BGR2BGRA)

    # 🎭 Fusion alpha manuelle
    alpha_smoke = smoke_rgba[:, :, 3:] / 255.0
    alpha_frame = 1.0 - alpha_smoke
    blended = (alpha_smoke * smoke_rgba[:, :, :3] + alpha_frame * frame_rgba[:, :, :3]).astype(np.uint8)

    # 🎥 Afficher la fusion
    cv2.imshow("Caméra + Fumée (Fusion Réaliste)", blended)

    # Quitter avec la touche 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# 🔚 Libérer les ressources
cap.release()
cv2.destroyAllWindows()
