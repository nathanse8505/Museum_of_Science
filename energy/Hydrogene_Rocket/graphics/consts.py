"""
Filename: consts.py
Purpose: Constants for the Hydrogen Rocket UI
"""
import os
import pygame

FPS = 100

# arduino
BAUDRATE = 115200
SERIAL_ERROR = -1
RECONNECT_INTERVAL = 1  # seconds

PARSE_VALID = 0
PARSE_ERROR = 1

# languages
HEBREW = 0
ENGLISH = 1
ARABIC = 2
LANGUAGES = [HEBREW, ENGLISH, ARABIC]
dic_lang = {HEBREW: "hebrew",
            ENGLISH: "english",
            ARABIC: "arabic"}

# states
OPENING = 0
MEASURE = 1
STATES = [OPENING, MEASURE]

# charge and current values
MIN_CHARGE = 0.0
MAX_CHARGE = 120.0
MIN_CURRENT = 0.0
MAX_CURRENT = 8.0
SWITCH_TO_MEASURE_SCREEN_CURRENT_THRESHOLD = 1  # above this current value, the screen will switch to the measure screen

# logging values
LOG_FOLDER = os.path.join(os.path.dirname(__file__), "logs")  # get the path of the logs folder
MAX_SIZE_PER_LOG_FILE = 1 * 1024 * 1024  # 1MB
BACKUP_COUNT = 10  # max number of log files, if all 10 are full, the first one will be deleted, rotating the rest

# sound
SOUND = os.path.join(os.path.dirname(__file__), "music")  # get the path of the pictures folder
SOUND_HEB = os.path.join(SOUND, "heb.mp3")
SOUND_ENG = os.path.join(SOUND, "eng.mp3")
SOUND_ARB = os.path.join(SOUND, "arb.mp3")

dic_sound = {HEBREW: SOUND_HEB,
             ENGLISH: SOUND_ENG,
             ARABIC: SOUND_ARB}

# pictures
pygame.init()
VIEW_PORT = pygame.display.Info().current_w, pygame.display.Info().current_h  # get the screen resolution
# VIEW_PORT = (607.5, 1080)  # for testing
# VIEW_PORT = (1080, 1920)  # for testing

PICTURES = os.path.join(os.path.dirname(__file__), "pictures")  # get the path of the pictures folder
OPEN_HEB = os.path.join(PICTURES, "0_heb.png")
OPEN_ENG = os.path.join(PICTURES, "0_eng.png")
OPEN_ARB = os.path.join(PICTURES, "0_arb.png")
MEASURE_HEB = os.path.join(PICTURES, "1_heb.png")
MEASURE_ENG = os.path.join(PICTURES, "1_eng.png")
MEASURE_ARB = os.path.join(PICTURES, "1_arb.png")
BAR_EMPTY = os.path.join(PICTURES, "bar_empty.png")
BAR_FULL = os.path.join(PICTURES, "bar_full.png")
NIDDLE = os.path.join(PICTURES, "niddle.png")
GAUGE = os.path.join(PICTURES, "gauge.png")

# load the pictures
open_heb = pygame.image.load(OPEN_HEB)
open_eng = pygame.image.load(OPEN_ENG)
open_arb = pygame.image.load(OPEN_ARB)
measure_heb = pygame.image.load(MEASURE_HEB)
measure_eng = pygame.image.load(MEASURE_ENG)
measure_arb = pygame.image.load(MEASURE_ARB)
bar_empty = pygame.image.load(BAR_EMPTY)
bar_full = pygame.image.load(BAR_FULL)
niddle = pygame.image.load(NIDDLE)
gauge = pygame.image.load(GAUGE)

# transform the pictures to the screen resolution
open_heb = pygame.transform.scale(open_heb, VIEW_PORT)
open_eng = pygame.transform.scale(open_eng, VIEW_PORT)
open_arb = pygame.transform.scale(open_arb, VIEW_PORT)
measure_heb = pygame.transform.scale(measure_heb, VIEW_PORT)
measure_eng = pygame.transform.scale(measure_eng, VIEW_PORT)
measure_arb = pygame.transform.scale(measure_arb, VIEW_PORT)
bar_empty = pygame.transform.scale(bar_empty, VIEW_PORT)
bar_full = pygame.transform.scale(bar_full, VIEW_PORT)
gauge = pygame.transform.scale(gauge, VIEW_PORT)
# niddle is a little tricky:
niddle = pygame.transform.scale(niddle, (int(niddle.get_size()[0] * VIEW_PORT[0] / 1080),
                                         int(niddle.get_size()[1] * VIEW_PORT[1] / 1920)))

# positions
NIDDLE_POS = (int(0.5 * VIEW_PORT[0]), int(0.43 * VIEW_PORT[1]))  # the position of the niddle on the screen
CURRENT_TEXT_POS = (
int(0.5 * VIEW_PORT[0]), int(0.487 * VIEW_PORT[1]))  # the position of the current text on the screen
CHARGE_TEXT_POS = (int(0.51 * VIEW_PORT[0]), int(0.92 * VIEW_PORT[1]))  # the position of the charge text on the screen
BAR_GRAPH_BOTTOM_HEIGHT = int(0.875 * VIEW_PORT[1])  # the bottom of the bar graph

# colors
BLACK = (0, 0, 0)
CURRENT_TEXT_COLOR = BLACK
CHARGE_TEXT_COLOR = BLACK

# sizes
CURRENT_TEXT_SIZE = int(50 * VIEW_PORT[0] / 1080)
CHARGE_TEXT_SIZE = int(80 * VIEW_PORT[0] / 1080)
BAR_SIZE = (int(146 * VIEW_PORT[0] / 1080), int(522 * VIEW_PORT[1] / 1920))
