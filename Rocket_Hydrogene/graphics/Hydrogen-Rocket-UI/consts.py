import os
import pygame

USE_FULL_SCREEN = True

# arduino
BAUDRATE = 115200
FORCE_ARDUINO = False  # run the program ONLY if you find arduino successfully - otherwise quit.
TRY_AGAIN_ARDUINO = True  # try again if arduino is 
SERIAL_ERROR = -1

# languages
HEBREW = 0
ENGLISH = 1
ARABIC = 2
LANGUAGES = [HEBREW, ENGLISH, ARABIC]

#states
OPENING = 0
MEASURE = 1
STATES = [OPENING, MEASURE]

# charge and current values
MIN_CHARGE = 0.0
MAX_CHARGE = 120.0
MIN_CURRENT = 0.0
MAX_CURRENT = 8.0
SWITCH_TO_MEASURE_SCREEN_CURRENT_THRESHOLD = 1


# pictures
pygame.init()
VIEW_PORT = pygame.display.Info().current_w, pygame.display.Info().current_h  # get the screen resolution
# VIEW_PORT = (607.5, 1080)  # for testing
# VIEW_PORT = (1080, 1920)  # for testing

PICTURES = os.path.join(os.path.dirname(__file__), "pictures")  # get the path of the pictures folder
OPEN_HEB = os.path.join(PICTURES, "open_heb.png")
OPEN_ENG = os.path.join(PICTURES, "open_eng.png")
OPEN_ARB = os.path.join(PICTURES, "open_arb.png")
MEASURE_HEB = os.path.join(PICTURES, "main_heb.png")
MEASURE_ENG = os.path.join(PICTURES, "main_eng.png")
MEASURE_ARB = os.path.join(PICTURES, "main_arb.png")
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
CURRENT_TEXT_POS = (int(0.5 * VIEW_PORT[0]), int(0.487 * VIEW_PORT[1]))  # the position of the current text on the screen
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