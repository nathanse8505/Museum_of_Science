"""
Filename: image_processing.py
Purpose: functions for the Droping Drops UI
"""
import os
import cv2
from const import *
import time
import numpy as np
from datetime import datetime
import pygame


# def process_and_save_image(input_path, output_path, log, threshold):
#     image = cv2.imread(input_path)  # read the image from the input path
#     if image is not None:
#         resized_image = cv2.resize(image, (output_width,
#                                            output_height))  # resize the image to (output_width) x (output_height) pixels. probably 64 x (25 or 30) pixels
#         gray_image = cv2.cvtColor(resized_image, cv2.COLOR_BGR2GRAY)  # convert the image to grayscale
#         _, bw_image = cv2.threshold(gray_image, threshold, 255,
#                                     cv2.THRESH_BINARY)  # Apply binary thresholding to create a black and white image
#         cv2.imwrite(output_path, bw_image)  # save the black and white image to the output path
#         flattened_array = bw_image.flatten()  # flatten the image to a 1D array
#         arr = []
#         black_pixels = 0  # count the number of black pixels in the image (black pixels are the pixels with value 0). This is to calculate the percentage of black pixels in the image
#         for i in range(0, len(flattened_array), 8):
#             if i % 64 == 0 and log:
#                 print()
#             # for each 8 pixels, create a byte value from the 8 pixels. The byte value is the binary representation of the 8 pixels
#             byte_value = 0
#             for j in range(8):
#                 pixel_value = flattened_array[i + j]  # get the value of the pixel
#                 byte_value |= (pixel_value & 1) << (
#                             7 - j)  # shift the pixel value to the left by (7 - j) bits, then add it to the byte value
#                 if pixel_value == 0:  # if the pixel is black, increment the black pixels counter
#                     black_pixels += 1
#                 if log:
#                     print(" " if (pixel_value & 1) else "#", end="")
#             arr.append(byte_value)  # add the byte value to the array
#         byte_array = bytearray(arr)  # convert the array to a byte array
#         if log:
#             print('\n\nImage processed and saved successfully.')
#             # print("\n\n len(byte_array): ", len(byte_array))
#         return byte_array, black_pixels / len(
#             flattened_array)  # return the byte array and the percentage of black pixels in the image
#     else:
#         print('Error loading the image.')
#         return None, None
#



# def save_pictures(img, is_folder_created):
#     time_stamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")  # get the current time to use it as the name of the image
#     if not is_folder_created:  # if the folder to save the images in is not created, create it
#         os.makedirs(folder_name, exist_ok=True)
#     in_path = os.path.join(folder_name, time_stamp + ".png")  # the path to save the original image
#     out_path = os.path.join(folder_name, time_stamp + "_bw.png")  # the path to save the black and white image
#     cv2.imwrite(in_path, img)  # save the original image
#     return in_path, out_path
#
#
# def display_camera_and_process_image(screen, img, in_path, out_path):
#     screen.fill((0, 0, 0))
#     image_display = pygame.image.load(in_path)
#     image_bw_display = pygame.image.load(out_path)
#     image_bw_display = pygame.transform.rotate(image_bw_display, -90)
#     image_display = pygame.transform.rotate(image_display, -90)
#     image_bw_display = pygame.transform.scale(image_bw_display, (screen_width // 2, screen_height))
#     image_display = pygame.transform.scale(image_display, (screen_width // 2, screen_height,))
#     screen.blit(image_display, (0, 0))
#     screen.blit(image_bw_display, (screen_width // 2, 0))

# def delete_image(in_path, out_path):
#     # delete images if not saving, or if the time to take the picture is less than 2 seconds for not saving too many images in a short time
#     if not save_picture or space_time / 1000 <= 2:
#         try:
#             os.remove(in_path)  # delete the original image
#             os.remove(out_path)  # delete the black and white image
#             return in_path, out_path
#         except Exception as e:
#             print(f"Error deleting images: {e}")
def main_process(cap, screen, camera_working, log_arduino, threshold, is_folder_created):
    # 1. Prendre une photo
    img, camera_working = take_pic(cap, camera_working)
    if not camera_working or img is None:
        return None, False, None, None, None, None  # continuer à la prochaine boucle si erreur

    # Configurer l'image (redimensionnement, flip, rotation…)
    #img = config_cam(img)

    # 2. Traitement de l’image
    byte_list, black_percentage, processed_image = process_image(img, log_arduino, threshold)
    if processed_image is None:
        return None, False, None, None, None, None

    # 3. Affichage
    display_images(screen, img, processed_image)

    # 4. Sauvegarde des images
    in_path, out_path = save_images(img, processed_image, is_folder_created)

    return img,camera_working, byte_list, black_percentage,in_path,out_path




def camera_init():
    try:
        cap = cv2.VideoCapture(camera_index)
        min_exposure = cap.get(cv2.CAP_PROP_EXPOSURE)
        max_exposure = cap.get(cv2.CAP_PROP_EXPOSURE)
        print(f"Valeur d'exposition actuelle : {min_exposure}")
        # cap.set(cv2.CAP_PROP_AUTO_EXPOSURE, 0 if not auto_exposure else 1)
        cap.set(cv2.CAP_PROP_AUTO_WB, 0 if not auto_white_balance else 1)
        # cap.set(cv2.CAP_PROP_EXPOSURE, fixed_exposure)
        if cap.isOpened():
            print("Camera is ready")
            camera_working = True
            return camera_working, cap
        print(
            "\nError: Could not open camera. please check if the camera is connected properly.\nRetrying in 2 seconds...\n\n")
        time.sleep(2)
    except Exception:
        return False, None



def take_pic(cap, camera_working):
    ret, img = cap.read()
    if ret:
        return img, True
    elif img is None or not camera_working:
        print("Error: Could not take picture")
        return img, False


def config_cam(img):
    img = cropped_img(img)
    img = cv2.flip(img, 0)  # flip the image vertically (the camera is upside down)
    img = cv2.rotate(img, cv2.ROTATE_90_COUNTERCLOCKWISE)
    # img = cv2.rotate(img, cv2.ROTATE_90_CLOCKWISE)
    return img

def msg_on_screen(screen, camera_working, found_arduino,threshold):
    text0 = "Press P to start/stop camera mode"
    text1 = "Press RIGHT/LEFT to change threshold for black and white filter"
    text2 = "threshold: " + str(threshold)
    text3 = "----------------------------------------------------------------------"
    text4 = "Arduino not found" if not found_arduino else "Arduino found"
    text5 = "Camera is connected" if camera_working else "Camera is NOT connected - reconnect the camera and press P to start"
    text = [text0, text1, text2, text3, text4, text5]
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

    # Convert NumPy arrays to pygame surfaces and rotate them
    original_surface = pygame.surfarray.make_surface(np.rot90(original_img_pygame))
    bw_surface = pygame.surfarray.make_surface(np.rot90(bw_img_pygame))

    # Resize the images to fit the screen (side-by-side layout)
    original_surface = pygame.transform.scale(original_surface, (screen_width // 2, screen_height))
    bw_surface = pygame.transform.scale(bw_surface, (screen_width // 2, screen_height))

    # Display original image on the left, processed image on the right
    screen.blit(original_surface, (0, 0))
    screen.blit(bw_surface, (screen_width // 2, 0))

    # Update the screen
    pygame.display.flip()


def process_image(image, log, threshold):
    # Resize the image to expected output size (e.g. 64x30)
    resized_image = cv2.resize(image, (output_width, output_height))

    # Convert the image to grayscale
    gray_image = cv2.cvtColor(resized_image, cv2.COLOR_BGR2GRAY)

    # Convert grayscale to black and white using a threshold
    _, bw_image = cv2.threshold(gray_image, threshold, 255, cv2.THRESH_BINARY)

    # Flatten the image into a 1D array
    flattened_array = bw_image.flatten()
    arr = []
    black_pixels = 0

    # Convert each group of 8 pixels into a byte (bitwise)
    for i in range(0, len(flattened_array), 8):
        if i % 64 == 0 and log:
            print()
        byte_value = 0
        for j in range(8):
            pixel_value = flattened_array[i + j]
            byte_value |= (pixel_value & 1) << (7 - j)
            if pixel_value == 0:
                black_pixels += 1
            if log:
                print(" " if (pixel_value & 1) else "#", end="")
        arr.append(byte_value)

    if log:
        print('\n\nImage processed successfully.')

    # Return the byte array, black pixel ratio, and the processed image
    return bytearray(arr), black_pixels / len(flattened_array), bw_image


def save_images(original_img, bw_img, is_folder_created):
    # Generate a unique timestamp for file naming
    time_stamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")

    # Create the folder if it doesn't already exist
    if not is_folder_created:
        os.makedirs(folder_name, exist_ok=True)

    # Define file paths
    in_path = os.path.join(folder_name, time_stamp + ".png")      # original image path
    out_path = os.path.join(folder_name, time_stamp + "_bw.png")  # black & white image path

    # Save both images to disk
    cv2.imwrite(in_path, original_img)
    cv2.imwrite(out_path, bw_img)

    return in_path, out_path


def delete_image(in_path, out_path):
    # Delete images if saving is disabled or if the time between captures is too short
    if not save_picture or space_time / 1000 <= 2:
        try:
            if os.path.exists(in_path):
                os.remove(in_path)
            if os.path.exists(out_path):
                os.remove(out_path)
            return None, None  # Images deleted successfully
        except Exception as e:
            print(f"Error deleting images: {e}")
    else:
        pass  # Keep the images if saving is enabled and timing is OK
