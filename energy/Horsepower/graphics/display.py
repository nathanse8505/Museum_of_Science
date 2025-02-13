"""
Filename: display.py
Purpose: Display functions for the Horse Power UI
"""
import pygame.display
from consts import *
import time


def display_state(screen, state=OPENING, language=HEBREW, horsepower=MIN_HORSEPOWER, deltatime = 0, index_images = 0 ):
    """
    Display the state screen
    :param screen: the screen to display the state screen on (right now there is only MEASURE state)
    :param language: the language to display the state screen in
    :param state: the state to display
    """
    if state == OPENING:
        display_opening(screen, language=language, index_images=index_images)

    if state == MEASURE:
        display_measure(screen, language=language, horsepower=horsepower,deltatime=deltatime)


def display_opening(screen, language, index_images):
    """
    Display the opening screen
    :param screen: the screen to display the opening screen on
    :param language: the language to display the opening screen in
    """
    if language == HEBREW:
        screen.blit(open_heb, (0,0))

    elif language == ENGLISH:
        screen.blit(open_eng, (0,0))

    elif language == ARABIC:
        screen.blit(open_arb, (0,0))

    screen.blit(images_balls[index_images], (BALL_CENTER_X, BALL_CENTER_Y))




def display_measure(screen, language=HEBREW, horsepower=MIN_HORSEPOWER,deltatime=0):
    """
    Display the measurement screen
    :param screen: the screen to display the measurement screen on
    :param language: the language to display the measurement screen in
    """
    horsepower_text_pos = HORSEPOWER_TEXT_POS
    deltatime_text_pos = TIME_TEXT_POS

    if language == HEBREW:
        screen.blit(measure_heb, (0,0))

    elif language == ENGLISH:
        screen.blit(measure_eng, (0,0))
        horsepower_text_pos = HORSEPOWER_TEXT_POS_ENG
        deltatime_text_pos = TIME_TEXT_POS_ENG

    elif language == ARABIC:
        screen.blit(measure_arb, (0,0))

    screen.blit(horse_empty, (0,0))
    display_bars(screen, horsepower)
    display_text_values(screen, horsepower,deltatime,horsepower_text_pos,deltatime_text_pos)


def display_bars(screen, horsepower=MIN_HORSEPOWER):
    """
    Display the bar on the screen according to the values
    :param screen: the screen to display the bar on
    :param voltage: the voltage to display
    """

    # sub function to reduce code duplication
    def display_bar_from_values(screen, value, max, min, bar_image):
        """
        sub function to display the bar on the screen according to the value and the max and min values
        """

        bar_width = bar_image.get_width()
        bar_height = bar_image.get_height()

        fill_height = int((value - min) / (max - min) * bar_height)
        crop_rect = pygame.Rect(0, bar_height - fill_height, bar_width, fill_height)
        cropped_bar = bar_image.subsurface(crop_rect).copy()
        pos_x = HORSE_GRAPH_SHIFT
        pos_y = int(HORSE_GRAPH_BOTTOM_HEIGHT - fill_height)

        screen.blit(cropped_bar, (pos_x, pos_y))



    display_bar_from_values(screen, horsepower, MAX_HORSEPOWER, MIN_HORSEPOWER, bar_full_horsepower)


def display_text_values(screen, horsepower=MIN_HORSEPOWER, deltatime=0, horsepower_text_pos=HORSEPOWER_TEXT_POS, deltatime_text_pos=TIME_TEXT_POS):
    """
    Display the text values on the screen
    :param screen: the screen to display the text values on
    :param voltage: the voltage to display
    :param charge: the charge to display
    :param energy: the energy to display
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


    display_text(screen,horsepower, horsepower_text_pos, TEXT_SIZE, TEXT_COLOR)
    display_text(screen, deltatime, deltatime_text_pos, TEXT_SIZE, TEXT_COLOR)
