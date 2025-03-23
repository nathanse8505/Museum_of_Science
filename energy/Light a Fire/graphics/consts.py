import os
import pygame
from change_pic import extract_numbers

# Constantes générales
FPS = 40
CAMERA_INDEX = 0
auto_exposure = False
auto_white_balance = False
fixed_exposure = -6

# Arduino
BAUDRATE = 115200
SERIAL_ERROR = -1
RECONNECT_INTERVAL = 1  # secondes
PARSE_ERROR = 1
PARSE_VALID = 0
SWITCH_TO_MEASURE_SCREEN_TEMPERATURE_THRESHOLD = 30  # en Celsius
TIME_AVG = 2 #secondes

# Températures
MIN_TEMPERATURE_VALUE = 24.0  # en Celsius
MAX_TEMPERATURE_VALUE = 50.0  # en Celsius
MIN_TEMPERATURE_DEFAULT = 15

# États
MEASURE = 1

# Logs
LOG_FOLDER = os.path.join(os.path.dirname(__file__), "logs")
MAX_SIZE_PER_LOG_FILE = 1 * 1024 * 1024  # 1MB
BACKUP_COUNT = 10  # Nombre max de fichiers log en rotation

# Interface graphique
pygame.init()
#VIEW_PORT = (1920, 1080)
VIEW_PORT = pygame.display.Info().current_w , pygame.display.Info().current_h
RESOLUTION_FLAME = (1920, 1080)
RESOLUTION_SMOKE = (3840, 2160)
SIZE_FLAME = (int(0.4 * RESOLUTION_FLAME[0]), int(0.4 * RESOLUTION_FLAME[1]))

# Chemins des dossiers d'images
PICTURES_SMOKE = os.path.join(os.path.dirname(__file__), "pictures_smoke")
PICTURES_FLAMES = os.path.join(os.path.dirname(__file__), "pictures_flame")

# Chargement des chemins des images
SMOKE_FRAMES_PATHS = sorted(
    [os.path.join(PICTURES_SMOKE, i) for i in os.listdir(PICTURES_SMOKE) if os.path.isfile(os.path.join(PICTURES_SMOKE, i))],
    key=extract_numbers
)
FLAMES_FRAMES_PATHS = sorted(
    [os.path.join(PICTURES_FLAMES, i) for i in os.listdir(PICTURES_FLAMES) if os.path.isfile(os.path.join(PICTURES_FLAMES, i))],
    key=extract_numbers
)

# Fonction pour charger une image à la demande
def load_scaled_image(path, size):
    """load and change the scale pf the  image if nessesary."""
    img = pygame.image.load(path)
    img = pygame.transform.scale(img, size)
    return img

smoke_images = [load_scaled_image(path , VIEW_PORT) for path in SMOKE_FRAMES_PATHS]
flames_images = [load_scaled_image(path , (int(VIEW_PORT[0]//2), int(VIEW_PORT[1]//2))) for path in FLAMES_FRAMES_PATHS]

# Positions des éléments à l'écran
#print(VIEW_PORT)
FLAME_FRAME_POS = (int(0.242*VIEW_PORT[0]), int(0.4*VIEW_PORT[1]))
#FLAME_FRAME_POS = (0, 0)
SMOKE_FRAME_POS = (0, 0)
FIRE_TEXT_POS = (int(0.2 * VIEW_PORT[0]), int(0.8 * VIEW_PORT[1]))

# Couleurs
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
TEXT_COLOR = (255, 0, 0)

# Tailles de texte
TEXT_SIZE = int(60 * VIEW_PORT[0] / 1080)




