import os
import cv2
import numpy as np

# Dossier contenant les images originales
source_folder = r"C:\Users\nathans\Desktop\Museum_of_Science\energy\Light a Fire\graphics\video\smoke_frame"
# Dossier où enregistrer les images avec transparence
output_folder = r"C:\Users\nathans\Desktop\Museum_of_Science\energy\Light a Fire\graphics\video"
os.makedirs(output_folder, exist_ok=True)

# Lister et trier les fichiers du dossier source
image_files = sorted(os.listdir(source_folder))

# Appliquer la transparence aux images avec OpenCV
for file_name in image_files:
    img_path = os.path.join(source_folder, file_name)
    img = cv2.imread(img_path, cv2.IMREAD_UNCHANGED)

    # Vérifier que l'image est bien chargée
    if img is None:
        print(f"⚠️ Impossible de lire l'image : {file_name}")
        continue

    # Convertir en RGBA si l'image n'a pas de canal alpha
    if img.shape[-1] != 4:
        img = cv2.cvtColor(img, cv2.COLOR_BGR2BGRA)

    # Détecter et rendre le fond noir transparent
    threshold = 50
    mask = (img[:, :, 0] < threshold) & (img[:, :, 1] < threshold) & (img[:, :, 2] < threshold)
    img[mask, 3] = 0  # Appliquer la transparence

    # Sauvegarder avec le même nom
    transparent_img_path = os.path.join(output_folder, file_name)
    cv2.imwrite(transparent_img_path, img)

print("✅ Toutes les images ont été traitées avec succès sans modifier leur nom !")
