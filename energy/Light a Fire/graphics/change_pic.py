import os
import cv2
import numpy as np
import shutil



def rename_png(source_folder_path):
    # Lister les fichiers image triés par nom
    image_files = sorted(os.listdir(source_folder_path),key=extract_numbers)

    # Renommer les fichiers de frame_0.png à frame_49.png en frame_50.png à frame_99.png
    for i, file_name in enumerate(image_files[:len(image_files)]):
        old_path = os.path.join(source_folder_path, file_name)
        new_file_name = f"frame_{i + len(image_files)}.png"
        new_path = os.path.join(source_folder_path, new_file_name)
        os.rename(old_path, new_path)

    print("Renommage terminé !")

def extract_numbers(filename):
    """ Extrait les nombres d'un nom de fichier et retourne une clé de tri. """
    parts = filename.split('.')
    name_part = parts[0]  # On prend la partie avant l'extension
    numeric_part = ''.join([char for char in name_part if char.isdigit()])
    return int(numeric_part) if numeric_part else float('inf')  # Si pas de numéro, on met une valeur élevée


def remove_black_bg(source_folder_path, output_folder_path):
    os.makedirs(output_folder_path, exist_ok=True)

    # Lister et trier les fichiers du dossier source
    image_files = sorted(os.listdir(source_folder_path))

    # Appliquer la transparence aux images avec OpenCV
    for file_name in image_files:
        img_path = os.path.join(source_folder_path, file_name)
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
        transparent_img_path = os.path.join(output_folder_path, file_name)
        cv2.imwrite(transparent_img_path, img)

    print("✅ Toutes les images ont été traitées avec succès sans modifier leur nom !")

def extraction_pic(input_mp4_path, output_folder_path):
    os.makedirs(output_folder_path, exist_ok=True)

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
            img_path = os.path.join(output_folder_path, f"frame_{i}.png")
            cv2.imwrite(img_path, frame)
            extracted_images.append(img_path)

    # Libérer la capture vidéo
    video_capture.release()

    # Compresser les images extraites en un ZIP
    smoke_frames_zip = output_folder_path + "/smoke_frames.zip"
    shutil.make_archive(smoke_frames_zip.replace(".zip", ""), 'zip', output_folder_path)

    # Retourner le lien de téléchargement
    smoke_frames_zip