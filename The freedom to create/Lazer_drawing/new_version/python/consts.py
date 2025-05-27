"""
Filename: consts.py
Purpose: Constants for the exhibit
"""
import os

# serial communication values
BAUDRATE = 115200

# screen dimensions
VIEWPORT = (1920 * 0.7, 1080 * 0.7)  # default viewport size
FULLSCREEN = True  # if True, the game will run in fullscreen mode (ignoring the viewport size)

# colors
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
YELLOW = (255, 255, 0)
GRAY = (180, 180, 180)
ORANGE = (255, 165, 0)

BACKGROUND_COLOR = WHITE
CUTTING_AREA_COLOR = WHITE
COLOR_OUTSIDE_BORDER = (226, 233, 241)
THE_RED_THAT_IS_ON_THE_BUTTONS = (199, 33, 47)
CUTTING_AREA_COLOR = WHITE

# drawing values
DRAWING_COLOR = RED
DRAWING_WIDTH = 10
MIN_DISTANCE_BETWEEN_POINTS = 5
MAX_LENGTH = 5000
DRAWING_MODE = 0
ERASING_MODE = 1

# frame values
FRAME_COLOR = BLACK
FRAME_WIDTH = 10
HEART_FRAME = 0
DROP_FRAME = 1
SQUARE_FRAME = 2

# assets
ASSETS_DIR = os.path.join(os.path.dirname(__file__), "assets")  # directory where the assets are stored
FRAMES_DIR = os.path.join(os.path.dirname(__file__), "frames")  # directory where the frames are stored (as text files)
DRAWINGS_DIR = os.path.join(os.path.dirname(__file__), "drawings")  # directory where the drawing images are saved (when printing)

# logging values
LOG_FOLDER = os.path.join(os.path.dirname(__file__), "logs")  # get the path of the logs folder
MAX_SIZE_PER_LOG_FILE = 1 * 1024 * 1024  # 1MB
BACKUP_COUNT = 10  # max number of log files, if all 10 are full, the first one will be deleted, rotating the rest 

# idle
IDLE_TIME = 180  # seconds

# laser values
ARDUINO_DRAWING_BATCH_TIMEOUT = 20  # for each batch of points, wait at most this time for the laser to draw
INIT_PARAMS_TIMEOUT = 2  # ack for params timeout
POINTS_BATCH_SIZE = 8  # each time send this many points to the laser to draw

LASER_POWER = 255  # (0 <= x <= 255)
FRAME_POWER = 255  # (0 <= x <= 255)
LASER_OFF_RATE = 6
LASER_ON_RATE = 6
FRAME_RATE = 50 # for green/yellow paper
MAX_DC_MOTOR_TIME = 1.8  # seconds

MM_PER_PULSE = [0.2, 0.2]  # mm per pulse for each motor
LASER_BOARD_SIZE = [83.0, 83.0]  # size of the board in mm