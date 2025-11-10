"""
Filename: const.py
Purpose: const for the Droping Drops UI
"""

import platform
import os
import pygame

camera_index = 0 # 0 is the default camera on the computer, change it to the camera you want to use
DEVICE = "/dev/video0"
CONFIG_FILE = "camera_config.json"

MIN_EXPOSURE = 1
MAX_EXPOSURE = 5000
MIN_GAIN = 0
MAX_GAIN = 100
MIN_WB = 2800
MAX_WB = 6500
# Camera configuration
empty_image_threshold = 0.15 # if the percentage of black pixels in the image is less than this value, the image is considered empty (0-1)
empty_captures_before_idle = 3  # if this many empty images are taken in a row, the camera will go to idle mode
crop_percentage = [90,95]  # the percentage of the image that WILL STAY after cropping the sides (the first value is the percentage of the width, the second value is the percentage of the height)
# adjust these values if the image is not being cropped correctly. 100% means no cropping, 0% means the image will be cropped completely
auto_exposure = False
auto_white_balance = False
fixed_exposure = -4

# logging values
LOG_FOLDER = os.path.join(os.path.dirname(__file__), "logs")  # get the path of the logs folder
MAX_SIZE_PER_LOG_FILE = 1 * 1024 * 1024  # 1MB
BACKUP_COUNT = 10  # max number of log files, if all 10 are full, the first one will be deleted, rotating the rest


# Arduino configuration
output_height = 40 # the height of the image that will be sent to the arduino (max value is 40 - DO NOT OVERPASS THIS VALUE)
output_width = 64 # DO NOT CHANGE - the width of the image that will be sent to the arduino (the arduino can only receive images with a width of 64 pixels - DO NOT CHANGE)
valve_on_time = 1 # time in milliseconds the valve will be open for each row in the image
space_time = 34/1000 # time in seconds the arduino will wait after receiving the 's' key before starting the process of dropping the image
drawing_depth = 1 # the amount of layers each picture will be (1 is the default value)
cassettes_num = 10 # the number of cassettes available in the exhibit


# Light configuration - currently not used
full_light = True # if True, the lights will be on all the time. If False, the lights will be on only after 'led_start_time' milliseconds and for 'led_on_time' milliseconds  - currently not used
led_start_time = 350 # RELEVANT ONLY if FULL LIGHT is False. time in milliseconds the arduino will wait after dropping the first row before turning on the lights  - currently not used
led_on_time = 1000 # RELEVANT ONLY if FULL LIGHT is False. time in milliseconds the lights will be on after 'led_start_time' milliseconds  - currently not used
led_power = 255 # the power of the lights (0-255) - currently not used
colors = [[0,0,0],[0,0,0],[0,0,0]] # the colors of the lights, max 8 colors (each color is 3 bytes - RGB)  - currently not used
colors_num = len(colors) # the number of colors of the lights  - currently not used

# Serial configuration
BAUDRATE = 115200  # DO NOT CHANGE the baudrate of the serial communication with the arduino

# Other configuration
save_picture = False # change this to True if you want to save the pictures taken by the camera - not recommended for long periods of time.
folder_name_for_saved_pictures = "pictures_from_camera"  # if save_picture is True, the pictures will be saved in this folder

# Idle configuration
folder_name_for_idle_pictures = "images_for_idle"  # this folder should be in the same folder as this file 'main.py'
idle_images = ["diamond.png", "hand1.png", "hand6.png", "heart.png", "star0.png"]
# these images will be used if the picture taken from camera is 'empty' (i.e. no object is detected on the picture).
# The images should be in the folder 'images_for_idle' and their paths should be relative to the folder 'images_for_idle'

# Keys configuration
drop_key = 'd'  # the key to drop (in water) the image in the arduino's buffer
start_key = 's'  # the key to send a new image to the arduino's buffer and start the process of dropping it
end_key = 'e'  # the key to stop the arduino's process of collecting data from computer and dropping the image
check_key = 'g'  # the key to check the arduino's response


# Réglage des variables globales
folder_name = os.path.join(os.path.dirname(os.path.abspath(__file__)), folder_name_for_saved_pictures)
idle_folder_name = os.path.join(os.path.dirname(os.path.abspath(__file__)), folder_name_for_idle_pictures)
is_folder_created = os.path.exists(folder_name)

pygame.init()
screen_width, screen_height = pygame.display.Info().current_w, pygame.display.Info().current_h
font_size = 40
font = pygame.font.Font(None, font_size)
