"""
Filename: display.py
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
        frame_rgb = np.rot90(frame_rgb)  # Corriger l'orientation si nécessaire
        frame_surface = pygame.surfarray.make_surface(frame_rgb)# Convertir l'image en surface Pygame
        frame_surface = pygame.transform.scale(frame_surface, VIEW_PORT)
        screen.blit(frame_surface, (0, 0))
        return True


def display_measure(screen, sensor_analogread, Temperature=MIN_TEMPERATURE_VALUE):
    """
    Affiche l'écran de mesure avec une image adaptée à la température.
    """
    number_of_frame = len(SMOKE_FRAMES_PATHS) + len(FLAMES_FRAMES_PATHS)
    temperatures = np.linspace(MIN_TEMPERATURE_VALUE, MAX_TEMPERATURE_VALUE, number_of_frame)
    index = min(range(number_of_frame), key=lambda i: abs(temperatures[i] - Temperature))

    if index < len(SMOKE_FRAMES_PATHS):
        image = load_scaled_image(SMOKE_FRAMES_PATHS[index], VIEW_PORT)
        screen.blit(image, SMOKE_FRAME_POS)
    else:
        image = load_scaled_image(FLAMES_FRAMES_PATHS[index - len(SMOKE_FRAMES_PATHS)], RESOLUTION_FLAME)
        screen.blit(image, FLAME_FRAME_POS)

    display_text_values(screen, Temperature)



def display_text_values(screen, Temperature = MIN_TEMPERATURE_VALUE):
    """
    Display the text values on the screen
    :param screen: the screen to display the text values on
    :param calorie: the calorie to display
    """

    # sub function to reduce code duplication
    def display_text(screen, text, pos, size, color):
        """
        sub function to display the text on the screen
        """
        font = pygame.font.Font(None, size)
        text = font.render(f"{text:.2f}", True, color)
        text_rect = text.get_rect(center=pos)
        screen.blit(text, text_rect)

    display_text(screen, Temperature, FIRE_TEXT_POS, TEXT_SIZE, TEXT_COLOR)


def calculate_Temperature(sensor_value):
    # Calculate isothermal work
    # in Joules
    Temperature=sensor_value

    return Temperature

