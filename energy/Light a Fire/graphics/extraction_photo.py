# Définir les chemins
import os
import  cv2
import numpy as np
import shutil
input_mp4_path = r"C:\Users\nathans\Desktop\Museum_of_Science\energy\Light a Fire\graphics\video\smoke.mp4"
output_frames_dir = r"C:\Users\nathans\Desktop\Museum_of_Science\energy\Light a Fire\graphics\video"
os.makedirs(output_frames_dir, exist_ok=True)

# Charger la vidéo
video_capture = cv2.VideoCapture(input_mp4_path)

# Vérifier si la vidéo a été chargée correctement
if not video_capture.isOpened():
    raise ValueError("Erreur lors du chargement de la vidéo.")

# Obtenir les propriétés de la vidéo
fps = int(video_capture.get(cv2.CAP_PROP_FPS))
total_frames = int(video_capture.get(cv2.CAP_PROP_FRAME_COUNT))

# Définir le nombre de frames à extraire sur les 2 premières secondes
num_frames_to_extract = 50
max_time = 2  # secondes

# Calculer les indices des frames à extraire dans les 2 premières secondes
frame_indices = np.linspace(0, min(fps * max_time, total_frames) - 1, num_frames_to_extract, dtype=int)

# Extraire et enregistrer les images
extracted_images = []
for i, frame_index in enumerate(frame_indices):
    video_capture.set(cv2.CAP_PROP_POS_FRAMES, frame_index)
    success, frame = video_capture.read()
    if success:
        img_path = os.path.join(output_frames_dir, f"frame_{i}.png")
        cv2.imwrite(img_path, frame)
        extracted_images.append(img_path)

# Libérer la capture vidéo
video_capture.release()

# Compresser les images extraites en un ZIP
smoke_frames_zip = output_frames_dir + "/smoke_frames.zip"
shutil.make_archive(smoke_frames_zip.replace(".zip", ""), 'zip', output_frames_dir)

# Retourner le lien de téléchargement
smoke_frames_zip
