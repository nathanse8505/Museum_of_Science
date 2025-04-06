"""
Filename: image_processing.py
Purpose: Display functions for the Light a Fire UI
"""
from consts import *
import numpy as np
import cv2
import time

def camera_init():
    while True:
        try:
            cap = cv2.VideoCapture(CAMERA_INDEX)  # open the camera
            cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)  # Réduit la taille du buffer de la caméra
            #cap.set(cv2.CAP_PROP_AUTO_EXPOSURE, 0 if not auto_exposure else 1)  # disable automatic exposure - IMPORTANT
            #cap.set(cv2.CAP_PROP_AUTO_WB,0 if not auto_white_balance else 1)  # disable automatic white balance - IMPORTANT
            #cap.set(cv2.CAP_PROP_EXPOSURE, fixed_exposure)  # set the exposure to a fixed value - IMPORTANT
            if cap.isOpened():
                print("Camera is ready\n")
                time.sleep(2)  # wait for 2 seconds before trying to open the camera again
                return cap # if the camera is connected and working, break the loop

        except Exception as e:
            print("Error: Could not open camera. please check if the camera is connected properly.\nRetrying in 2 seconds...\n")
            pass

def camera_setup(screen,cap):

        ret, frame = cap.read()
        if not ret:
            print("Erreur lors de la lecture de la caméra")
            return False

        frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB) # Convertir l'image BGR (OpenCV) en RGB (Pygame)
        #frame_rgb = np.rot90(frame_rgb)  # Corriger l'orientation si nécessaire
        #frame_surface = pygame.surfarray.make_surface(frame_rgb)# Convertir l'image en surface Pygame
        frame_surface = pygame.image.frombuffer(frame_rgb.tobytes(), frame_rgb.shape[1::-1], "RGB")
        frame_surface = pygame.transform.scale(frame_surface, VIEW_PORT)
        screen.blit(frame_surface, (0, 0))
        return True








def display_measure(screen, sensor_analogread, Temperature=MIN_TEMPERATURE_DEFAULT):
    """
    Affiche l'écran de mesure avec une image adaptée à la température.
    Fait boucler certaines images de flammes si la température dépasse le max.
    """

    number_of_frame = len(SMOKE_FRAMES_PATHS) + len(FLAMES_FRAMES_PATHS)
    temperatures = np.linspace(MIN_TEMPERATURE_VALUE, MAX_TEMPERATURE_VALUE, number_of_frame)
    index = min(range(number_of_frame), key=lambda i: abs(temperatures[i] - Temperature))

    if Temperature < MAX_TEMPERATURE_VALUE:
        if index < len(SMOKE_FRAMES_PATHS):
            screen.blit(smoke_images[index], SMOKE_FRAME_POS)
        else:
            screen.blit(flames_images[index - len(SMOKE_FRAMES_PATHS)], FLAME_FRAME_POS)
    else:
        # Choisir l’image à afficher selon l'écart de température
        delta_temp = Temperature - MAX_TEMPERATURE_VALUE
        frame_index = int(delta_temp // DEGREES_PER_FRAME) % LOOP_FLAME_COUNT
        last_flame_images = flames_images[-LOOP_FLAME_COUNT:]
        image_to_display = last_flame_images[frame_index]
        screen.blit(image_to_display, FLAME_FRAME_POS)

    display_text_values(screen, Temperature)
    screen.blit(empty_thermometer, THERMOMETER_FRAME_POS)
    display_bars(screen, Temperature)


def display_text_values(screen, temperature):
    """
    Display the text values on the screen
    :param screen: the screen to display the text values on
    :param temperature: the temperature to display
    """

    # sub function to reduce code duplication
    def display_text(screen, text, pos, size, color):
        """
        sub function to display the text on the screen
        """
        font = pygame.font.Font(None, size)
        text = font.render(f"{text:.1f}°C", True, color)
        text_rect = text.get_rect(center=pos)
        screen.blit(text, text_rect)

    display_text(screen, temperature, FIRE_TEXT_POS, TEXT_SIZE, TEXT_COLOR)


def calculate_Temperature(sensor_value):
    # Calculate isothermal work
    # in Joules
    Temperature=sensor_value

    return Temperature



def avg_batch(temperature_list, new_temp, previous_avg):
    """
    Ajoute une nouvelle température à la liste.
    Quand on atteint ROLLING_WINDOW_SIZE, on calcule la moyenne, on vide la liste,
    et on retourne la nouvelle moyenne.
    Sinon, on retourne la moyenne précédente.
    """
    temperature_list.append(new_temp)

    if len(temperature_list) >= ROLLING_WINDOW_SIZE:
        new_avg = sum(temperature_list) / len(temperature_list)
        temperature_list.clear()  # Réinitialise la liste pour le prochain batch
        return temperature_list, new_avg
    else:
        return temperature_list, previous_avg



def display_bars(screen, temperature=MIN_TEMPERATURE_VALUE):
    """
    Display the bar on the screen according to the values
    :param screen: the screen to display the bar on
    :param temperature: the voltage to display
    """

    # sub function to reduce code duplication
    def display_bar_from_values(screen, value, max, min, bar_image):
        """
        sub function to display the bar on the screen according to the value and the max and min values
        """
        bar_width = RESOLUTION_THERMOMETER[0]
        bar_height = RESOLUTION_THERMOMETER[1]

        fill_height = (value - min) / (max - min)
        if fill_height >= 1:
            fill_height =  THERMO_FULL_SIZE
        else:
            fill_height = int(fill_height * THERMO_FULL_SIZE)

        crop_rect = pygame.Rect(0, bar_height - fill_height, bar_width, fill_height)
        cropped_bar = bar_image.subsurface(crop_rect).copy()

        pos_x = 0
        pos_y = int(BOTTOM_THERMOMETER_POS - fill_height)

        screen.blit(cropped_bar, (pos_x, pos_y))


    display_bar_from_values(screen, temperature, MAX_TEMPERATURE_VALUE, MIN_TEMPERATURE_DEFAULT, full_thermometer)

def load_and_blit_picture(screen,index):

    # Initialiser les variables statiques si elles n'existent pas
    if not hasattr(display_measure, "current_image"):
        display_measure.current_image = None  # Image actuellement affichée
        display_measure.last_index = -1  # Dernier index d’image affiché
        display_measure.pos = SMOKE_FRAME_POS

    # Vérifier si l'image affichée doit changer
    if index != display_measure.last_index:
        # Supprimer l’ancienne image pour libérer la RAM
        if display_measure.current_image:
            del display_measure.current_image

        # Charger la nouvelle image
        if index < len(SMOKE_FRAMES_PATHS):
            new_image = load_scaled_image(SMOKE_FRAMES_PATHS[index], VIEW_PORT)
            display_measure.pos = SMOKE_FRAME_POS
        else:
            new_image = load_scaled_image(FLAMES_FRAMES_PATHS[index - len(SMOKE_FRAMES_PATHS)], RESOLUTION_FLAME)
            display_measure.pos = FLAME_FRAME_POS

        # Mettre à jour l'image actuelle et son index
        display_measure.current_image = new_image
        display_measure.last_index = index

    # Blitter uniquement l'image actuelle (sans rechargement inutile)
    if display_measure.current_image:
        screen.blit(display_measure.current_image, display_measure.pos)