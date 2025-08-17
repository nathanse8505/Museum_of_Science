"""
Filename: image_processing.py
Purpose: functions for the Droping Drops UI
"""

import cv2
from const import *
import time
import numpy as np
from datetime import datetime
import pygame
import json


def main_process(cap, screen, camera_working, log_arduino, threshold, logger):
    # 1. Prendre une photo
    img, camera_working = take_pic(cap, camera_working)
    if not camera_working or img is None:
        return None, False, None, None # continuer à la prochaine boucle si erreur

    # Configurer l'image (redimensionnement, flip, rotation…)
    img = config_cam(img)

    # 2. Traitement de l’image
    byte_list, black_percentage, B_W_image = process_image(img, log_arduino, logger, threshold)
    if B_W_image is None:
        return None, False, None, None

    # 3. Affichage
    display_images(screen, img, B_W_image)

    # 4. Sauvegarde les images si besoin
    save_images(img, B_W_image, is_folder_created)

    return img,camera_working, byte_list, black_percentage




def camera_init():
    try:
        cap = cv2.VideoCapture(camera_index)
        cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
        cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)
        #min_exposure = cap.get(cv2.CAP_PROP_EXPOSURE)
        #max_exposure = cap.get(cv2.CAP_PROP_EXPOSURE)
        #print(f"Valeur d'exposition actuelle : {min_exposure}")
        #cap.set(cv2.CAP_PROP_AUTO_EXPOSURE, 0 if not auto_exposure else 1)
        #cap.set(cv2.CAP_PROP_AUTO_WB, 0 if not auto_white_balance else 1)
        #cap.set(cv2.CAP_PROP_EXPOSURE, fixed_exposure)
        if cap.isOpened():
            print("Camera is ready")
            camera_working = True
            return camera_working, cap
        print("\nError: Could not open camera. please check if the camera is connected properly.\nRetrying in 2 seconds...\n\n")
        time.sleep(2)
    except Exception:
        return False, None


# Initialisation des réglages manuels
def set_manual_controls(exposure, wb_temp, gain):
    # 1. Passer en mode manuel pour exposition et white balance
    os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=auto_exposure=1")  # 1 = Manual Mode
    os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=white_balance_automatic=0")

    # 2. Appliquer les réglages
    os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=exposure_time_absolute={exposure}")
    os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=white_balance_temperature={wb_temp}")
    os.system(f"v4l2-ctl -d {DEVICE} --set-ctrl=gain={gain}")


# Callback pour les sliders
def nothing(x):
    pass


def load_config():
    if os.path.exists(CONFIG_FILE):
        with open(CONFIG_FILE, 'r') as f:
            try:
                config = json.load(f)
                print("Configuration chargée :", config)
                return config
            except Exception as e:
                print("Erreur lecture config:", e)
    return {"exposure": 157, "wb_temp": 4600, "gain": 0}


def save_config(exposure, wb_temp, gain):
    config = {
        "exposure": exposure,
        "wb_temp": wb_temp,
        "gain": gain,
    }
    with open(CONFIG_FILE, 'w') as f:
        json.dump(config, f)
    print("Configuration sauvegardée :", config)


def take_pic(cap, camera_working):
    ret, img = cap.read()
    if ret:
        return img, True
    elif img is None or not camera_working:
        print("Error: Could not take picture")
        return img, False


def config_cam(img):
    #img = cropped_img(img)
    img = cv2.flip(img, 1)  # flip the image vertically (the camera is upside down)
    img = cv2.rotate(img, cv2.ROTATE_90_COUNTERCLOCKWISE)
    # img = cv2.rotate(img, cv2.ROTATE_90_CLOCKWISE)
    return img


def msg_on_screen(screen, camera_working, found_arduino,threshold, exposure, wb_temp, gain):
    text0 = "Press P to start/stop camera mode"
    text1 = "Press RIGHT/LEFT to change threshold for black and white filter"
    text2 = "threshold: " + str(threshold)
    text3 = "exposure: " + str(exposure)
    text4 = "white balance temperature: " + str(wb_temp)
    text5 = "gain: " + str(gain)
    text6 = "----------------------------------------------------------------------"
    text7 = "Arduino not found" if not found_arduino else "Arduino found"
    text8 = "Camera is connected" if camera_working else "Camera is NOT connected - reconnect the camera and press P to start"
    text = [text0, text1, text2, text3, text4, text5, text6, text7,text8]
    y_position = screen_height // 2 - len(text) * font_size // 2
    for line in text:
        text_surface = font.render(line, True, (255, 255, 255))
        text_rect = text_surface.get_rect()
        text_rect.center = (screen_width // 2, y_position)
        screen.blit(text_surface, text_rect)
        y_position += font_size

def cropped_img(img):
    # Calculate the dimensions of the central region to keep after cropping
    height, width = img.shape[:2]
    crop_height = int(height * crop_percentage[1] / 100)
    crop_width = int(width * crop_percentage[0] / 100)
    # Calculate the starting point for cropping
    start_x = (width - crop_width) // 2
    start_y = (height - crop_height) // 2
    # Crop the central region of the image
    cropped_image = img[start_y:start_y + crop_height, start_x:start_x + crop_width]
    # Create a new image with the same dimensions as the original, filled with white
    new_image = np.full_like(img, 255, dtype=np.uint8)
    # Place the cropped image in the center of the new image
    new_image[start_y:start_y + crop_height, start_x:start_x + crop_width] = cropped_image
    img = new_image
    return img


def display_images(screen, original_img, bw_img):
    # Clear the screen (black background)
    screen.fill((0, 0, 0))

    # Convert OpenCV images to RGB format compatible with pygame
    original_img_pygame = cv2.cvtColor(original_img, cv2.COLOR_BGR2RGB)
    bw_img_pygame = cv2.cvtColor(bw_img, cv2.COLOR_GRAY2RGB)

    # # Convert cv2 arrays to pygame surfaces and rotate them
    original_surface = pygame.image.frombuffer(original_img_pygame.tobytes(), original_img_pygame.shape[1::-1], "RGB")
    bw_surface = pygame.image.frombuffer(bw_img_pygame.tobytes(), bw_img_pygame.shape[1::-1], "RGB")

    # Resize the images to fit the screen (side-by-side layout)
    original_surface = pygame.transform.scale(original_surface, (screen_width // 2, screen_height))
    bw_surface = pygame.transform.scale(bw_surface, (screen_width // 2, screen_height))

    # Display original image on the left, processed image on the right
    screen.blit(original_surface, (0, 0))
    screen.blit(bw_surface, (screen_width // 2, 0))

    # Update the screen
    pygame.display.flip()






def save_images(original_img, bw_img, is_folder_created):
    # Only save the image if saving is enabled AND enough time has passed between captures
    if save_picture and space_time > 2:
        time_stamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")

        if not is_folder_created:
            os.makedirs(folder_name, exist_ok=True)

        in_path = os.path.join(folder_name, time_stamp + ".png")
        out_path = os.path.join(folder_name, time_stamp + "_bw.png")

        cv2.imwrite(in_path, original_img)
        cv2.imwrite(out_path, bw_img)


def process_image(image, log, logger, threshold):
    # Resize the image to expected output size (e.g. 64x30)
    resized_image = cv2.resize(image, (output_width, output_height))

    # Convert the image to grayscale
    gray_image = cv2.cvtColor(resized_image, cv2.COLOR_BGR2GRAY)

    # Convert grayscale to black and white using a threshold
    _, bw_image = cv2.threshold(gray_image, threshold, 255, cv2.THRESH_BINARY)

    # Flatten the image into a 1D array
    flattened_array = bw_image.flatten()
    arr = []
    black_pixels = 0  # count the number of black pixels in the image (black pixels are the pixels with value 0). This is to calculate the percentage of black pixels in the image

    # Convert each group of 8 pixels into a byte (bitwise)
    for i in range(0, len(flattened_array), 8):
        if i % 64 == 0 and log:
            print()
        byte_value = 0  # for each 8 pixels, create a byte value from the 8 pixels. The byte value is the binary representation of the 8 pixels
        for j in range(8):
            pixel_value = flattened_array[i + j] # get the value of the pixel
            byte_value |= (pixel_value & 1) << (7 - j)  # shift the pixel value to the left by (7 - j) bits, then add it to the byte value
            if pixel_value == 0:   # if the pixel is black, increment the black pixels counter
                black_pixels += 1
            if log:
                print(" " if (pixel_value & 1) else "#", end="")
        arr.append(byte_value)  # add the byte value to the array

    if log:
        print('\n\nImage processed successfully.')
        logger.info('\n\nImage processed successfully.')


    # Return the arr byte array, black pixel ratio, and the processed image
    return bytearray(arr), black_pixels / len(flattened_array), bw_image
