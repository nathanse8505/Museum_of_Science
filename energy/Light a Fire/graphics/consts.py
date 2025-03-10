"""
Filename: consts.py
Purpose: Constants for the Light a Fire UI
"""
import os
import pygame

FPS = 100
#
CAMERA_INDEX = 0
auto_exposure = False
auto_white_balance = False
fixed_exposure = -6

# arduino
BAUDRATE = 115200
SERIAL_ERROR = -1
RECONNECT_INTERVAL = 1  # seconds
PARSE_ERROR = 1
PARSE_VALID = 0
SWITCH_TO_MEASURE_SCREEN_TEMPERATURE_THRESHOLD = 30 #in Celsius

# voltage, charge, and capacitor values
MIN_TEMPERATURE_VALUE = 20.0  # in Celsius
MAX_TEMPERATURE_VALUE = 80.0  # in Celsius

#states (only one state for this project)
MEASURE = 1
# logging values
LOG_FOLDER = os.path.join(os.path.dirname(__file__), "logs")  # get the path of the logs folder
MAX_SIZE_PER_LOG_FILE = 1 * 1024 * 1024  # 1MB
BACKUP_COUNT = 10  # max number of log files, if all 10 are full, the first one will be deleted, rotating the rest 


# pictures
pygame.init()
#VIEW_PORT = pygame.display.Info().current_w, pygame.display.Info().current_h  # get the screen resolution
#VIEW_PORT = (607.5, 1080)  # for testing
#VIEW_PORT = (1080, 1920)  # for testing
VIEW_PORT =(800,800)
RESOLUTION_FLAME = (640,330)
SIZE_FLAME = (int(0.4 * RESOLUTION_FLAME[0]),int(0.4 * RESOLUTION_FLAME[1]))



PICTURES = os.path.join(os.path.dirname(__file__), "pictures")  # get the path of the pictures folder
FIRE_FRAMES_PATHS = [os.path.join(PICTURES, i) for i in os.listdir(PICTURES) if os.path.isfile(os.path.join(PICTURES, i))]
print(FIRE_FRAMES_PATHS)

# load the pictures
images_fire = [pygame.image.load(img) for img in FIRE_FRAMES_PATHS]

# transform the pictures to the screen resolution
images_fire = [pygame.transform.scale(img,SIZE_FLAME) for img in images_fire]
images_fire = [pygame.transform.scale(img,RESOLUTION_FLAME) for img in images_fire]

# positions - dont ask about the magic numbers
FIRE_FRAME_POS = (int(0.1 * VIEW_PORT[0]), int(0.5*VIEW_PORT[1]))  # the position of the energy text on the screen
#FIRE_FRAME_POS = (0,0)  # the position of the energy
FIRE_TEXT_POS = (int(0.35 * VIEW_PORT[0]), int(0.9*VIEW_PORT[1]))  # the position of the energy text on the screen

# colors
BLACK = (0, 0, 0)
WHITE = (255,255,255)
TEXT_COLOR = WHITE

# sizes - dont ask about the magic numbers
TEXT_SIZE = int(50 * VIEW_PORT[0] / 1080)
