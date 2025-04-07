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
        threshold = 80
        mask = (img[:, :, 0] < threshold) & (img[:, :, 1] < threshold) & (img[:, :, 2] < threshold)
        img[mask, 3] = 0  # Appliquer la transparence

        # Sauvegarder avec le même nom
        transparent_img_path = os.path.join(output_folder_path, file_name)
        cv2.imwrite(transparent_img_path, img)

    print("✅ Toutes les images ont été traitées avec succès sans modifier leur nom !")


def extraction_pic(input_mp4_path, output_folder_path):
    """
    Extrait des images d'une vidéo MP4 et les enregistre dans un sous-dossier.
    """
    # Créer le dossier principal s'il n'existe pas
    os.makedirs(output_folder_path, exist_ok=True)

    # Créer un sous-dossier pour les images extraites
    extracted_frames_folder = os.path.join(output_folder_path, "extracted_frames")
    os.makedirs(extracted_frames_folder, exist_ok=True)

    # Charger la vidéo
    video_capture = cv2.VideoCapture(input_mp4_path)

    # Vérifier si la vidéo a été chargée correctement
    if not video_capture.isOpened():
        raise ValueError("Erreur lors du chargement de la vidéo.")

    # Obtenir les propriétés de la vidéo
    fps = int(video_capture.get(cv2.CAP_PROP_FPS))
    total_frames = int(video_capture.get(cv2.CAP_PROP_FRAME_COUNT))

    # Définir le nombre de frames à extraire
    num_frames_to_extract = 50
    max_time = 2  # secondes

    # Calculer les indices des frames à extraire
    frame_indices = np.linspace(0, min(fps * max_time, total_frames) - 1, num_frames_to_extract, dtype=int)

    # Extraire et enregistrer les images
    extracted_images = []
    for i, frame_index in enumerate(frame_indices):
        video_capture.set(cv2.CAP_PROP_POS_FRAMES, frame_index)
        success, frame = video_capture.read()
        if success:
            img_path = os.path.join(extracted_frames_folder, f"frame_{i}.png")
            cv2.imwrite(img_path, frame)
            extracted_images.append(img_path)

    # Libérer la capture vidéo
    video_capture.release()

    # Retourner le chemin du dossier contenant les images extraites
    return extracted_frames_folder




def crop_image_center_down(input_folder, output_folder, crop_size=(400, 1200)):
    """
    Recadre chaque image du dossier à 500x1000 centré horizontalement et positionné en bas.

    :param input_folder: Dossier contenant les images originales
    :param output_folder: Dossier où enregistrer les images recadrées
    :param crop_size: (largeur, hauteur) du crop
    """
    os.makedirs(output_folder, exist_ok=True)
    target_width, target_height = crop_size

    for filename in os.listdir(input_folder):
        if filename.lower().endswith((".png", ".jpg", ".jpeg")):
            input_path = os.path.join(input_folder, filename)
            output_path = os.path.join(output_folder, filename)

            image = cv2.imread(input_path, cv2.IMREAD_UNCHANGED)
            if image is None:
                print(f"[!] Impossible de charger {filename}")
                continue

            h, w = image.shape[:2]

            if h < target_height or w < target_width:
                print(f"[!] Image trop petite pour crop : {filename}")
                continue

            # Coordonnées du crop : centré horizontalement, en bas verticalement
            x_start = (w - target_width) // 2
            y_start = h - target_height

            cropped = image[y_start:y_start + target_height, x_start:x_start + target_width]

            cv2.imwrite(output_path, cropped)
            print(f"[✓] Crop effectué : {filename}")


def apply_column_blur(image, window_size=60):
    """
    Applique un flou vertical basé sur une moyenne glissante par colonne.
    Conserve le canal alpha (transparence).
    """
    h, w = image.shape[:2]
    blurred = np.zeros_like(image)

    for x in range(w):
        for c in range(3):  # R, G, B uniquement
            # Moyenne glissante verticale
            blurred[:, x, c] = np.convolve(
                image[:, x, c],
                np.ones(window_size) / window_size,
                mode='same'
            )

    # Copier le canal alpha tel quel
    if image.shape[2] == 4:
        blurred[:, :, 3] = image[:, :, 3]

    return blurred.astype(np.uint8)


def process_smoke_folder(input_folder, output_folder, window_size=60):
    """
    Applique le flou 'new_smoke' à toutes les images PNG d’un dossier.

    :param input_folder: Dossier contenant les images originales
    :param output_folder: Dossier de sortie pour les images floutées
    :param window_size: Taille de la fenêtre verticale pour la moyenne
    """
    os.makedirs(output_folder, exist_ok=True)

    for filename in os.listdir(input_folder):
        if filename.lower().endswith(".png"):
            in_path = os.path.join(input_folder, filename)
            out_path = os.path.join(output_folder, filename)

            image = cv2.imread(in_path, cv2.IMREAD_UNCHANGED)
            if image is None or image.shape[2] != 4:
                print(f"[!] Image ignorée ou invalide : {filename}")
                continue

            blurred = apply_column_blur(image, window_size=window_size)
            cv2.imwrite(out_path, blurred)
            print(f"[✓] Fichier traité : {filename}")


# Exemple d'utilisation :
# process_smoke_folder("chemin/vers/images_fumee", "chemin/vers/images_fumee_floues")


if __name__ == "__main__":
    path_in = r"C:\Users\nathans\Desktop\Museum_of_Science\energy\Light_a_Fire\graphics\crop_image_smoke"
    #path_in = r"C:\Users\nathans\Desktop\Museum_of_Science\energy\Light_a_Fire\graphics\video\smoke_frames_transparent"
    path_out = r"C:\Users\nathans\Desktop\Museum_of_Science\energy\Light_a_Fire\graphics\video\crop_image_smoke_blur"

    process_smoke_folder(path_in, path_out, window_size=15)

