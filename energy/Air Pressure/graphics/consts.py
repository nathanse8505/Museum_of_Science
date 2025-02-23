"""
Filename: consts.py
Purpose: Constants for the Air Pressure UI
"""
import os
import pygame

FPS = 100

# arduino
BAUDRATE = 115200
SERIAL_ERROR = -1
RECONNECT_INTERVAL = 1  # seconds
PARSE_ERROR = 1
PARSE_VALID = 0
SWITCH_TO_MEASURE_SCREEN_PRESSURE_THRESHOLD = 15000 #in Pa

# voltage, charge, and capacitor values
MIN_PRESSURE_VALUE = 0.0  # in PA
MAX_PRESSURE_VALUE = 140000.0  # in PA
MIN_P_ATM = 0.0  # in atm
MAX_P_ATM = 1.4  # in atm
MIN_CALORIE = 0.0  # in cal
MAX_CALORIE = 80.0  # in cal
MIN_ENERGY = 0.0  # in joules
MAX_ENERGY = 300.0  # in joules

NOISE_THRESHOLD = 1  # voltage noise threshold
MIN_DROP_DISTANCE = 20000  # if the pressure drops (from the max it has been in, not neccesarly last measure) by this amount, this is potentially a drop
PRESSURE_AFTER_DROP = 5000  # pressure in Pa below this value is considered for a drop

ENERGY_TO_CAL = 4.184
ATM_TO_KPa = 101325 #in PA
kPa_TO_Pa = 1000

V = 1.5 / 1000  # Volume in m³ (converted from L)
# languages
HEBREW = 0
ENGLISH = 1
ARABIC = 2
LANGUAGES = [HEBREW, ENGLISH, ARABIC]
dic_lang = {HEBREW:"hebrew",
            ENGLISH:"english",
            ARABIC:"arabic"}

#states (only one state for this project)
MEASURE = 1
OPENING = 0
STATES =[OPENING, MEASURE]

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
OPENING_HEB = os.path.join(PICTURES, "0_heb.png")
OPENING_ENG = os.path.join(PICTURES, "0_eng.png")
OPENING_ARB = os.path.join(PICTURES, "0_arb.png")
MEASURE_HEB = os.path.join(PICTURES, "1_heb.png")
MEASURE_ENG = os.path.join(PICTURES, "1_eng.png")
MEASURE_ARB = os.path.join(PICTURES, "1_arb.png")
BAR_FULL_ENERGY = os.path.join(PICTURES, "bar_full_left.png")
BAR_FULL_CALORIE = os.path.join(PICTURES, "bar_full_right.png")
NIDDLE = os.path.join(PICTURES,"1_niddle.png")


# load the pictures
open_heb = pygame.image.load(OPENING_HEB)
open_arb = pygame.image.load(OPENING_ARB)
open_eng = pygame.image.load(OPENING_ENG)
measure_heb = pygame.image.load(MEASURE_HEB)
measure_eng = pygame.image.load(MEASURE_ENG)
measure_arb = pygame.image.load(MEASURE_ARB)
bar_full_energy = pygame.image.load(BAR_FULL_ENERGY)
bar_full_calorie = pygame.image.load(BAR_FULL_CALORIE)
niddle = pygame.image.load(NIDDLE)



# transform the pictures to the screen resolution
open_heb = pygame.transform.scale(open_heb, VIEW_PORT)
open_arb = pygame.transform.scale(open_arb, VIEW_PORT)
open_eng = pygame.transform.scale(open_eng, VIEW_PORT)
measure_heb = pygame.transform.scale(measure_heb, VIEW_PORT)
measure_eng = pygame.transform.scale(measure_eng, VIEW_PORT)
measure_arb = pygame.transform.scale(measure_arb, VIEW_PORT)
bar_full_energy = pygame.transform.scale(bar_full_energy, VIEW_PORT)
bar_full_calorie = pygame.transform.scale(bar_full_calorie, VIEW_PORT)
niddle = pygame.transform.scale(niddle, (int(niddle.get_size()[0] * VIEW_PORT[0] / 1080),
                                         int(niddle.get_size()[1] * VIEW_PORT[1] / 1920)))


# positions - dont ask about the magic numbers
ENERGY_TEXT_POS = (int(3.7 / 11.0 * VIEW_PORT[0]), int(0.935 * VIEW_PORT[1]))  # the position of the energy text on the screen
CALORIE_TEXT_POS = (int(7.32/11 * VIEW_PORT[0]), int(0.935 * VIEW_PORT[1]))  # the position of the charge text on the screen
P_ATM_TEXT_POS = (int(0.5 * VIEW_PORT[0]), int(0.5 * VIEW_PORT[1]))  # the position of the voltage text on the screen
NIDDLE_POS = (int(0.5 * VIEW_PORT[0]), int(0.45 * VIEW_PORT[1]))  # the position of the niddle on the screen

BAR_GRAPH_BOTTOM_HEIGHT = int(0.89 * VIEW_PORT[1]) # the bottom of the bar graphs (same for all 2)
BAR_GRAPH_SHIFT = 0
BAR_SIZE = int(0.27 * VIEW_PORT[1])

# colors
BLACK = (0, 0, 0)
TEXT_COLOR = BLACK

# sizes - dont ask about the magic numbers
TEXT_SIZE = int(50 * VIEW_PORT[0] / 1080)
