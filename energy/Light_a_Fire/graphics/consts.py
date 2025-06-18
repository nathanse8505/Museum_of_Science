import os
import pygame
import shutil

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

# Températures
MIN_TEMPERATURE_VALUE = 26.0  # en Celsius for begin the smoke pic
MAX_TEMPERATURE_VALUE = 45.0  # en Celsius for end thermometer and last pic of fire
MIN_TEMPERATURE_DEFAULT = 15  #if arduino is not connected and for the begin of the thermometer
MAX_TEMPERATURE_DEFAULT = 250
TEMPERATURE_SENSOR_OFF = 5

# Logs
LOG_FOLDER = os.path.join(os.path.dirname(__file__), "logs")
MAX_SIZE_PER_LOG_FILE = 1 * 1024 * 1024  # 1MB
BACKUP_COUNT = 10  # Nombre max de fichiers log en rotation
TEMP_AFTER_PEAK = 2  # en Celsius
TEMP_NEW_USER= 28  # en Celsius

RISE_THRESHOLD = 7  # Nombre de hausses nécessaires pour détecter une tendance montante
WINDOWS_SIZE = 10


# Interface graphique
pygame.init()
# VIEW_PORT = (1920, 1080)
VIEW_PORT = pygame.display.Info().current_w, pygame.display.Info().current_h
#VIEW_PORT = (1000,1000)
print(VIEW_PORT)
RESOLUTION_FLAME = (850, 350)
RESOLUTION_SMOKE = (1200, 400)
RESOLUTION_THERMOMETER = (487, 183)

SIZE_THERMOMETER = (int(1 * RESOLUTION_THERMOMETER[0]), int(1 * RESOLUTION_THERMOMETER[1]))
SIZE_FLAME = (int(0.7 * RESOLUTION_FLAME[0]), int(0.7 * RESOLUTION_FLAME[1]))
SIZE_SMOKE = (int(0.7 * RESOLUTION_SMOKE[0]), int(0.7 * RESOLUTION_SMOKE[1]))

# Chemins des dossiers d'images
PICTURES_SMOKE = os.path.join(os.path.dirname(__file__), "image_smoke")
PICTURES_FLAMES = os.path.join(os.path.dirname(__file__), "image_flame")
PICTURES_THERMOMETER = os.path.join(os.path.dirname(__file__), "pictures_thermometer")
full_thermometer_path = os.path.join(PICTURES_THERMOMETER, "thermometer_full.png")
empty_thermometer_path = os.path.join(PICTURES_THERMOMETER, "thermometer_empty.png")


def extract_numbers(filename):
    """ Extrait les nombres d'un nom de fichier et retourne une clé de tri. """
    parts = filename.split('.')
    name_part = parts[0]  # On prend la partie avant l'extension
    numeric_part = ''.join([char for char in name_part if char.isdigit()])
    return int(numeric_part) if numeric_part else float('inf')  # Si pas de numéro, on met une valeur élevée


# load path images
SMOKE_FRAMES_PATHS = sorted(
    [os.path.join(PICTURES_SMOKE, i) for i in os.listdir(PICTURES_SMOKE) if
     os.path.isfile(os.path.join(PICTURES_SMOKE, i))],
    key=extract_numbers
)
FLAMES_FRAMES_PATHS = sorted(
    [os.path.join(PICTURES_FLAMES, i) for i in os.listdir(PICTURES_FLAMES) if
     os.path.isfile(os.path.join(PICTURES_FLAMES, i))],
    key=extract_numbers
)


# Fonction to load and transform scale image
def load_scaled_image(path, size):
    """load and change the scale pf the  image if nessesary."""
    img = pygame.image.load(path)
    img = pygame.transform.scale(img, size)
    return img


smoke_images = [load_scaled_image(path, SIZE_SMOKE) for path in SMOKE_FRAMES_PATHS]
flames_images = [load_scaled_image(path, SIZE_FLAME) for path in FLAMES_FRAMES_PATHS]
full_thermometer = load_scaled_image(full_thermometer_path, SIZE_THERMOMETER)
empty_thermometer = load_scaled_image(empty_thermometer_path, SIZE_THERMOMETER)

# Positions of éléments to the screen
FLAME_FRAME_POS = (int(0.12 * VIEW_PORT[0]), int(0.31 * VIEW_PORT[1]))
SMOKE_FRAME_POS = (int(0.05 * VIEW_PORT[0]), int(0.31 * VIEW_PORT[1]))
THERMOMETER_POS = (int(0.3 * VIEW_PORT[0]), int(0.75 * VIEW_PORT[1]))
TEMPERATURE_TEXT_POS = (int(0.25 * VIEW_PORT[0]), int(0.87 * VIEW_PORT[1]))

THERMO_FULL_SIZE = int(0.97 * SIZE_THERMOMETER[0])

# Couleurs
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
TEXT_COLOR = (255, 0, 0)

# Tailles de texte
TEXT_SIZE = int(60 * VIEW_PORT[0] / 1080)

LOOP_FLAME_COUNT = 25  # Nombre d'images de flammes pour la boucle
DEGREES_PER_FRAME = 0.5  # Tous les X degrés, on passe à l’image suivante dans la boucle