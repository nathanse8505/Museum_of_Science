# NOT USED IN THE FINAL IMPLEMENTATION

import os
import serial
from typing import List
from datetime import datetime
os.environ["OPENCV_LOG_LEVEL"]="SILENT" # to suppress OpenCV warnings and errors
os.environ["PYGAME_HIDE_SUPPORT_PROMPT"] = "hide" # to suppress Pygame welcome message
import cv2
import time
import pygame
import numpy as np
from pygame.locals import *
from consts import *

# Function to process and save the image in black and white.
def process_and_save_image(input_path, output_path):
    global time_per_capture, log
    image = cv2.imread(input_path)
    if image is not None:
        resized_image = cv2.resize(image, (64, 20))
        gray_image = cv2.cvtColor(resized_image, cv2.COLOR_BGR2GRAY)
        _, bw_image = cv2.threshold(gray_image, threshold, 255, cv2.THRESH_BINARY)
        cv2.imwrite(output_path, bw_image)
        flattened_array = bw_image.flatten()
        arr = []
        black_pixels = 0
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
        byte_array = bytearray(arr)
        if log:
            print('\n\nImage processed and saved successfully.')
            # print("\n\n len(byte_array): ", len(byte_array))
        return byte_array, black_pixels / len(flattened_array)
    else:
        print('Error loading the image.')
        return None


input_path = "pictures_from_camera2/hand_new.png"
output_path = "pictures_from_camera2/hand_new_bw.png"
byte_array, black_pixels_percentage = process_and_save_image(input_path, output_path)
print("byte_array: ", byte_array)
print("\nblack_pixels_percentage: ", black_pixels_percentage)

# THIS FILE IS NOT USED IN THE FINAL IMPLEMENTATION