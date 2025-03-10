import os
import numpy as np
import cv2

def remove_background(image_path):
    """Supprime l'arrière-plan d'une image et retourne une image avec transparence."""
    image = cv2.imread(image_path, cv2.IMREAD_UNCHANGED)
    if image is None:
        print(f"Erreur : Impossible de charger {image_path}")
        return None

    # Vérifier si l'image a un canal alpha, sinon ajouter un
    if image.shape[-1] == 3:
        image = cv2.cvtColor(image, cv2.COLOR_BGR2BGRA)

    mask = np.zeros(image.shape[:2], np.uint8)
    bgdModel = np.zeros((1, 65), np.float64)
    fgdModel = np.zeros((1, 65), np.float64)
    rect = (10, 10, image.shape[1] - 20, image.shape[0] - 20)

    cv2.grabCut(image[:, :, :3], mask, rect, bgdModel, fgdModel, 5, cv2.GC_INIT_WITH_RECT)

    mask2 = np.where((mask == 2) | (mask == 0), 0, 1).astype("uint8")
    result = image * mask2[:, :, np.newaxis]

    # Convertir le fond noir en transparent
    b, g, r, a = cv2.split(result)
    alpha = np.where((b == 0) & (g == 0) & (r == 0), 0, 255).astype(np.uint8)
    result = cv2.merge([b, g, r, alpha])  # Ajouter canal alpha

    return result


def process_folder(input_folder="flame_images", output_folder="flame_no_bg"):
    """Parcourt toutes les images du dossier et applique remove_background."""
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)

    for filename in os.listdir(input_folder):
        input_path = os.path.join(input_folder, filename)

        if filename.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp', '.tiff')):
            print(f"Traitement de : {filename}")
            result = remove_background(input_path)

            if result is not None:
                output_path = os.path.join(output_folder, filename.split('.')[0] + ".png")  # Toujours enregistrer en PNG
                cv2.imwrite(output_path, result, [cv2.IMWRITE_PNG_COMPRESSION, 9])  # Enregistrer avec compression
                print(f"✅ Image sauvegardée avec transparence : {output_path}")
        else:
            print(f"⏩ Ignoré (pas une image) : {filename}")


def main():
    process_folder()  # Traite toutes les images dans "flame" et les sauvegarde dans "output"


if __name__ == "__main__":
    main()
