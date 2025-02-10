"""
Filename: consts.py
Purpose: Constants for the Horse Power UI
"""
import os
import pygame

FPS = 100

# arduino
BAUDRATE = 115200
SERIAL_ERROR = -1
RECONNECT_INTERVAL = 1  # seconds

# voltage, charge, and capacitor values
MIN_HORSEPOWER = 0.0  # in volts
MAX_HORSEPOWER = 1.0  # in volts


# languages
HEBREW = 0
ENGLISH = 1
ARABIC = 2
LANGUAGES = [HEBREW, ENGLISH, ARABIC]

#states (only one state for this project)
MEASURE = 1

# logging values
LOG_FOLDER = os.path.join(os.path.dirname(__file__), "logs")  # get the path of the logs folder
MAX_SIZE_PER_LOG_FILE = 1 * 1024 * 1024  # 1MB
BACKUP_COUNT = 10  # max number of log files, if all 10 are full, the first one will be deleted, rotating the rest 


# pictures
pygame.init()
VIEW_PORT = pygame.display.Info().current_w, pygame.display.Info().current_h  # get the screen resolution
# VIEW_PORT = (607.5, 1080)  # for testing
# VIEW_PORT = (1080, 1920)  # for testing

PICTURES = os.path.join(os.path.dirname(__file__), "pictures")  # get the path of the pictures folder
MEASURE_HEB = os.path.join(PICTURES, "1_heb.png")
MEASURE_ENG = os.path.join(PICTURES, "1_eng.png")
MEASURE_ARB = os.path.join(PICTURES, "1_arb.png")
BAR_FULL_HORSEPOWER = os.path.join(PICTURES, "bar_full_middle.png")


# load the pictures
measure_heb = pygame.image.load(MEASURE_HEB)
measure_eng = pygame.image.load(MEASURE_ENG)
measure_arb = pygame.image.load(MEASURE_ARB)
bar_full_horsepower = pygame.image.load(BAR_FULL_HORSEPOWER)


# transform the pictures to the screen resolution
measure_heb = pygame.transform.scale(measure_heb, VIEW_PORT)
measure_eng = pygame.transform.scale(measure_eng, VIEW_PORT)
measure_arb = pygame.transform.scale(measure_arb, VIEW_PORT)
bar_full_horsepower = pygame.transform.scale(bar_full_horsepower, VIEW_PORT)

# positions - dont ask about the magic numbers
HORSEPOWER_TEXT_POS = (int(0.5 * VIEW_PORT[0]), int(0.725 * VIEW_PORT[1]))  # the position of the voltage text on the screen
BAR_GRAPH_BOTTOM_HEIGHT = int(VIEW_PORT[1])  # the bottom of the bar graphs (same for all 3)

# colors
BLACK = (0, 0, 0)
WHITE = (255,255,255)
TEXT_COLOR = BLACK

# sizes - dont ask about the magic numbers
TEXT_SIZE = int(50 * VIEW_PORT[0] / 1080)
BAR_SIZE = (int(1338 * VIEW_PORT[0] / 1338), int(1190*VIEW_PORT[1])/1190)