import os
import cv2
import numpy as np
import pygame

def take_picture():
    global cap, camera_working
    if not camera_working:
        return None
    ret, frame = cap.read()
    if ret:
        return frame
    return None

def process_and_save_image(input_path, output_path):
    global log
    image = cv2.imread(input_path)
    if image is not None:
        resized_image = cv2.resize(image, (output_width, output_height))
        gray_image = cv2.cvtColor(resized_image, cv2.COLOR_BGR2GRAY)
        _, bw_image = cv2.threshold(gray_image, threshold, 255, cv2.THRESH_BINARY)
        cv2.imwrite(output_path, bw_image)
        flattened_array = bw_image.flatten()
        arr = []
        black_pixels = 0
        for i in range(0, len(flattened_array), 8):
            byte_value = 0
            for j in range(8):
                pixel_value = flattened_array[i + j]
                byte_value |= (pixel_value & 1) << (7 - j)
                if pixel_value == 0:
                    black_pixels += 1
            arr.append(byte_value)
        byte_array = bytearray(arr)
        return byte_array, black_pixels / len(flattened_array)
    else:
        print('Error loading the image.')
        return None

def msg_on_screen():
    global font, screen, screen_width, screen_height, font_size, found_arduino, camera_working, threshold
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

def msg_on_screen2():
    global font2, screen
    text0 = "Press P for information"
    text_surface = font2.render(text0, True, (255, 255, 255))
    text_rect = text_surface.get_rect()
    text_rect.center = (screen_width * 3 // 4, screen_height - 30)
    screen.blit(text_surface, text_rect)

def camera_init():
    camera_working = False
    cap = None
    while True:
        try:
            cap = cv2.VideoCapture(camera_index)
            cap.set(cv2.CAP_PROP_AUTO_EXPOSURE, 0 if not auto_exposure else 1)
            cap.set(cv2.CAP_PROP_AUTO_WB, 0 if not auto_white_balance else 1)
            cap.set(cv2.CAP_PROP_EXPOSURE, fixed_exposure)
            if cap.isOpened():
                print("Camera is ready")
                camera_working = True
                return camera_working, cap
            print("\nError: Could not open camera. please check if the camera is connected properly.\nRetrying in 2 seconds...\n\n")
            time.sleep(2)
        except Exception:
            pass
