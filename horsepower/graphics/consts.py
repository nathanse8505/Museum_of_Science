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
SWITCH_TO_MEASURE_SCREEN = 0  # above this current value, the screen will switch to the measure screen

# voltage, charge, and capacitor values
MIN_HORSEPOWER = 0.0  # in volts
MAX_HORSEPOWER = 0.5  # in volts


# languages
HEBREW = 0
ENGLISH = 1
ARABIC = 2
LANGUAGES = [HEBREW, ENGLISH, ARABIC]

#state
OPENING = 0
MEASURE = 1
STATES = [OPENING, MEASURE]

# logging values
LOG_FOLDER = os.path.join(os.path.dirname(__file__), "logs")  # get the path of the logs folder
MAX_SIZE_PER_LOG_FILE = 1 * 1024 * 1024  # 1MB
BACKUP_COUNT = 10  # max number of log files, if all 10 are full, the first one will be deleted, rotating the rest 


# pictures
pygame.init()
VIEW_PORT = pygame.display.Info().current_w, pygame.display.Info().current_h  # get the screen resolution
# VIEW_PORT = (607.5, 1080)  # for testing
# VIEW_PORT = (1080, 1920)  # for testing

# ======================== CONFIGURATION BALL ANIMATION ====================
DELAY_FRAME_BALL = 300
BALL_WIDTH, BALL_HEIGHT = int(600 * VIEW_PORT[0] / 1338), int(600 * VIEW_PORT[1] / 1190)


PICTURES = os.path.join(os.path.dirname(__file__), "pictures")  # get the path of the pictures folder
OPEN_HEB = os.path.join(PICTURES, "0_heb.png")
OPEN_ENG = os.path.join(PICTURES, "0_eng.png")
OPEN_ARB = os.path.join(PICTURES, "0_arb.png")
MEASURE_HEB = os.path.join(PICTURES, "1_heb.png")
MEASURE_ENG = os.path.join(PICTURES, "1_eng.png")
MEASURE_ARB = os.path.join(PICTURES, "1_arb.png")
HORSE_EMPTY = os.path.join(PICTURES, "horse_empty.png")
BAR_FULL_HORSEPOWER = os.path.join(PICTURES, "horse_full.png")

BALL_FRAMES_PATHS = [
    os.path.join(PICTURES, "ball_1.png"),
    os.path.join(PICTURES, "ball_2.png"),
    os.path.join(PICTURES, "ball_3.png"),
    os.path.join(PICTURES, "ball_4.png"),
]


# load the pictures
open_heb = pygame.image.load(OPEN_HEB)
open_eng = pygame.image.load(OPEN_ENG)
open_arb = pygame.image.load(OPEN_ARB)
measure_heb = pygame.image.load(MEASURE_HEB)
measure_eng = pygame.image.load(MEASURE_ENG)
measure_arb = pygame.image.load(MEASURE_ARB)
horse_empty = pygame.image.load(HORSE_EMPTY)
bar_full_horsepower = pygame.image.load(BAR_FULL_HORSEPOWER)
images_balls = [pygame.image.load(img) for img in BALL_FRAMES_PATHS]


# transform the pictures to the screen resolution
open_heb = pygame.transform.scale(open_heb,VIEW_PORT)
open_eng = pygame.transform.scale(open_eng,VIEW_PORT)
open_arb = pygame.transform.scale(open_arb,VIEW_PORT)
measure_heb = pygame.transform.scale(measure_heb, VIEW_PORT)
measure_eng = pygame.transform.scale(measure_eng, VIEW_PORT)
measure_arb = pygame.transform.scale(measure_arb,VIEW_PORT)
horse_empty = pygame.transform.scale(horse_empty,(int(800 * VIEW_PORT[0] / 1338), int(800*VIEW_PORT[1])/1190))
bar_full_horsepower = pygame.transform.scale(bar_full_horsepower,(int(800 * VIEW_PORT[0] / 1338), int(800*VIEW_PORT[1])/1190))
images_balls = [pygame.transform.scale(img, (BALL_WIDTH, BALL_HEIGHT)) for img in images_balls]

# positions - dont ask about the magic numbers
HORSEPOWER_TEXT_POS = (int(0.5 * VIEW_PORT[0]), int(0.8 * VIEW_PORT[1]))  # the position of the voltage text on the screen
HORSEPOWER_POS = (int(250 * VIEW_PORT[0]/1338), int(300 * VIEW_PORT[1]/1190))
HORSE_GRAPH_BOTTOM_HEIGHT = int(1100 * VIEW_PORT[1])/1190 # the bottom of the bar graphs (same for all 3)
HORSE_GRAPH_SHIFT = int(220 * VIEW_PORT[0])/1190 # the bottom of the bar graphs (same for all 3)
# height and position of the animation ball
BALL_CENTER_X = (VIEW_PORT[0] - BALL_WIDTH) // 2
BALL_CENTER_Y = (VIEW_PORT[1] - BALL_HEIGHT) // 1.7

# colors
BLACK = (0, 0, 0)
WHITE = (255,255,255)
TEXT_COLOR = BLACK

# sizes - dont ask about the magic numbers
TEXT_SIZE = int(50 * VIEW_PORT[0] / 1080)
BAR_SIZE = (int(800 * VIEW_PORT[0] / 1338), int(800*VIEW_PORT[1])/1190)