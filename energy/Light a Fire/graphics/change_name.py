import os

# Dossier contenant les images
image_folder = r"C:\Users\nathans\Desktop\Museum_of_Science\energy\Light a Fire\graphics\video\fire_frames_50_new_transparent"

# Lister les fichiers image triés par nom
image_files = sorted(os.listdir(image_folder))

# Vérifier qu'il y a au moins 50 images
if len(image_files) < 50:
    raise ValueError("Il doit y avoir au moins 50 images dans le dossier.")

# Renommer les fichiers de frame_0.png à frame_49.png en frame_50.png à frame_99.png
for i, file_name in enumerate(image_files[:50]):
    old_path = os.path.join(image_folder, file_name)
    new_file_name = f"frame_{i + 50}.png"
    new_path = os.path.join(image_folder, new_file_name)
    os.rename(old_path, new_path)

print("Renommage terminé !")
