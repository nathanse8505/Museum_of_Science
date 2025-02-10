"""
Filename: display.py
Purpose: Display functions for the Horse Power UI
"""
from consts import *


def display_state(screen, state=MEASURE, language=HEBREW, horsepower=MIN_HORSEPOWER):
    """
    Display the state screen
    :param screen: the screen to display the state screen on (right now there is only MEASURE state)
    :param language: the language to display the state screen in
    :param state: the state to display
    """
    if state == MEASURE:
        display_measure(screen, language=language, horsepower=horsepower)


def display_measure(screen, language=HEBREW, horsepower=MIN_HORSEPOWER):
    """
    Display the measurement screen
    :param screen: the screen to display the measurement screen on
    :param language: the language to display the measurement screen in
    """
    if language == HEBREW:
        screen.blit(measure_heb, (0,0))

    elif language == ENGLISH:
        screen.blit(measure_eng, (0,0))

    elif language == ARABIC:
        screen.blit(measure_arb, (0,0))


    display_bars(screen, horsepower)
    display_text_values(screen, horsepower)


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
        height = int((value - min) / (max - min) * BAR_SIZE[1])
        crop_rect = pygame.Rect(0, BAR_GRAPH_BOTTOM_HEIGHT - height, VIEW_PORT[0], VIEW_PORT[1] - BAR_GRAPH_BOTTOM_HEIGHT + height)
        cropped_bar = bar_image.subsurface(crop_rect).copy()
        screen.blit(cropped_bar, (0, BAR_GRAPH_BOTTOM_HEIGHT - height))


    display_bar_from_values(screen, horsepower, MAX_HORSEPOWER, MIN_HORSEPOWER, bar_full_horsepower)


def display_text_values(screen, horsepower=MIN_HORSEPOWER):
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

    display_text(screen,horsepower, HORSEPOWER_TEXT_POS, TEXT_SIZE, TEXT_COLOR)

