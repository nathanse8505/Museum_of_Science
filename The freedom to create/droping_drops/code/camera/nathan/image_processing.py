import os
import cv2
from const import *
import time
import numpy as np
from datetime import datetime
import pygame



def process_and_save_image(input_path, output_path ,log):
    image = cv2.imread(input_path)  # read the image from the input path
    if image is not None:
        resized_image = cv2.resize(image, (output_width, output_height))  # resize the image to (output_width) x (output_height) pixels. probably 64 x (25 or 30) pixels
        gray_image = cv2.cvtColor(resized_image, cv2.COLOR_BGR2GRAY)  # convert the image to grayscale
        _, bw_image = cv2.threshold(gray_image, threshold, 255, cv2.THRESH_BINARY)  # Apply binary thresholding to create a black and white image
        cv2.imwrite(output_path, bw_image)  # save the black and white image to the output path
        flattened_array = bw_image.flatten()  # flatten the image to a 1D array
        arr = []
        black_pixels = 0  # count the number of black pixels in the image (black pixels are the pixels with value 0). This is to calculate the percentage of black pixels in the image
        for i in range(0, len(flattened_array), 8):
            if i % 64 == 0 and log:
                print()
            # for each 8 pixels, create a byte value from the 8 pixels. The byte value is the binary representation of the 8 pixels
            byte_value = 0
            for j in range(8):
                pixel_value = flattened_array[i + j]  # get the value of the pixel
                byte_value |= (pixel_value & 1) << (7 - j)  # shift the pixel value to the left by (7 - j) bits, then add it to the byte value
                if pixel_value == 0:  # if the pixel is black, increment the black pixels counter
                    black_pixels += 1
                if log:
                    print(" " if (pixel_value & 1) else "#", end="")
            arr.append(byte_value)  # add the byte value to the array
        byte_array = bytearray(arr)  # convert the array to a byte array
        if log:
            print('\n\nImage processed and saved successfully.')
            # print("\n\n len(byte_array): ", len(byte_array))
        return byte_array, black_pixels / len(flattened_array)  # return the byte array and the percentage of black pixels in the image
    else:
        print('Error loading the image.')
        return None,None

def msg_on_screen(screen,camera_working, found_arduino):
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

def msg_on_screen2(screen):
    text0 = "Press P for information"
    text_surface = font2.render(text0, True, (255, 255, 255))
    text_rect = text_surface.get_rect()
    text_rect.center = (screen_width * 3 // 4, screen_height - 30)
    screen.blit(text_surface, text_rect)

def camera_init():
    try:
        cap = cv2.VideoCapture(camera_index)
        min_exposure = cap.get(cv2.CAP_PROP_EXPOSURE)
        max_exposure = cap.get(cv2.CAP_PROP_EXPOSURE)
        print(f"Valeur d'exposition actuelle : {min_exposure}")
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
        return False ,None

def take_picture(cap ,camera_working):
    if not camera_working:
        return None
    ret, frame = cap.read()
    if ret:
        return frame
    return None

def take_new_pic(cap ,camera_working):
    img = take_picture(cap ,camera_working)  # take a picture
    if img is None:
        print("Error: Could not take picture")
        return False , False
    else:
        return img ,True , True

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
    return  img

def save_pictures(img , is_folder_created):
    time_stamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")  # get the current time to use it as the name of the image
    if not is_folder_created:  # if the folder to save the images in is not created, create it
        os.makedirs(folder_name, exist_ok=True)
    in_path = os.path.join(folder_name, time_stamp + ".png")  # the path to save the original image
    out_path = os.path.join(folder_name, time_stamp + "_bw.png")  # the path to save the black and white image
    cv2.imwrite(in_path, img)  # save the original image
    return in_path , out_path

def display_camera_and_process_image(screen,in_path,out_path):
    screen.fill((0, 0, 0))
    image_display = pygame.image.load(in_path)
    image_bw_display = pygame.image.load(out_path)
    image_bw_display = pygame.transform.rotate(image_bw_display, -90)
    image_display = pygame.transform.rotate(image_display, -90)
    image_bw_display = pygame.transform.scale(image_bw_display, (screen_width // 2, screen_height))
    image_display = pygame.transform.scale(image_display, (screen_width // 2, screen_height,))
    screen.blit(image_display, (0, 0))
    screen.blit(image_bw_display, (screen_width // 2, 0))

def delete_image(in_path,out_path):
    # delete images if not saving, or if the time to take the picture is less than 2 seconds for not saving too many images in a short time
    if not save_picture or space_time / 1000 <= 2:
        try:
            os.remove(in_path)  # delete the original image
            os.remove(out_path)  # delete the black and white image
            return in_path ,out_path
        except Exception as e:
            print(f"Error deleting images: {e}")