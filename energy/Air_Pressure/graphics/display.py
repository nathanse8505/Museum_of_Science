"""
Filename: image_processing.py
Purpose: Display functions for the Air Pressure UI
"""
from consts import *
import numpy as np


def display_state(screen, state=MEASURE, language=HEBREW, pressure_value=MIN_PRESSURE_VALUE):
    """
    Display the state screen
    :param screen: the screen to display the state screen on (right now there is only MEASURE state)
    :param language: the language to display the state screen in
    :param state: the state to display
    """
    if state == MEASURE:
        display_measure(screen, language=language, pressure_value=pressure_value)

    if state == OPENING:
        display_opening(screen, language=language)


def display_opening(screen, language):
    """
    Display the opening screen
    :param screen: the screen to display the opening screen on
    :param language: the language to display the opening screen in
    """
    if language == HEBREW:
        screen.blit(open_heb, (0, 0))

    elif language == ENGLISH:
        screen.blit(open_eng, (0, 0))

    elif language == ARABIC:
        screen.blit(open_arb, (0, 0))


def display_measure(screen, language=HEBREW, pressure_value=MIN_PRESSURE_VALUE):
    """
    Display the measurement screen
    :param screen: the screen to display the measurement screen on
    :param language: the language to display the measurement screen in
    :param pressure_value: pressure value to display and calculate the measurement screen in
    """
    if language == HEBREW:
        screen.blit(measure_heb, (0, 0))

    elif language == ENGLISH:
        screen.blit(measure_eng, (0, 0))

    elif language == ARABIC:
        screen.blit(measure_arb, (0, 0))

    # sub function to calculate the Patm, calorie and energy from the pressure_value in Pa
    Patm, energy, calorie = calculate_work_and_Patm(pressure_value)

    display_bars(screen, calorie, energy)
    display_rotated_niddle(screen, Patm)
    display_text_values(screen, Patm, calorie, energy)


def display_bars(screen, calorie=MIN_CALORIE, energy=MIN_ENERGY):
    """
    Display the bar on the screen according to the values
    :param screen: the screen to display the bar on
    :param calorie: the calorie to display
    :param energy: the energy to display
    """

    # sub function to reduce code duplication
    def display_bar_from_values(screen, value, max, min, bar_image):
        """
        sub function to display the bar on the screen according to the value and the max and min values
        """
        height = int((value - min) / (max - min) * BAR_SIZE)
        crop_rect = pygame.Rect(0, BAR_GRAPH_BOTTOM_HEIGHT - height, VIEW_PORT[0],
                                VIEW_PORT[1] - BAR_GRAPH_BOTTOM_HEIGHT + height)
        cropped_bar = bar_image.subsurface(crop_rect).copy()
        screen.blit(cropped_bar, (0, BAR_GRAPH_BOTTOM_HEIGHT - height))

    display_bar_from_values(screen, calorie, MAX_CALORIE, MIN_CALORIE, bar_full_calorie)
    display_bar_from_values(screen, energy, MAX_ENERGY, MIN_ENERGY, bar_full_energy)


def display_text_values(screen, Patm=MIN_P_ATM, calorie=MIN_CALORIE, energy=MIN_ENERGY):
    """
    Display the text values on the screen
    :param screen: the screen to display the text values on
    :param calorie: the calorie to display
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

    display_text(screen, Patm, P_ATM_TEXT_POS, TEXT_SIZE, TEXT_COLOR)
    display_text(screen, calorie, CALORIE_TEXT_POS, TEXT_SIZE, TEXT_COLOR)
    display_text(screen, energy, ENERGY_TEXT_POS, TEXT_SIZE, TEXT_COLOR)


def display_rotated_niddle(screen, Patm):
    """
    Display the rotated niddle based on the current value
    :param screen: the screen to display the bar on
    :param Patm: the pressure in atm value
    """
    angle = 180 * (Patm - MIN_P_ATM) / (MAX_P_ATM - MIN_P_ATM)
    rotated_niddle = pygame.transform.rotate(niddle, -angle)
    rect = rotated_niddle.get_rect(center=NIDDLE_POS)
    screen.blit(rotated_niddle, rect.topleft)


def calculate_work_and_Patm(pressure_value):
    # Calculate isothermal work in Joules
    Patm = max(min(pressure_value / ATM_TO_KPa, MAX_P_ATM), MIN_P_ATM)

    energy_val = ATM_TO_KPa * (Patm + 1) * V * np.log((Patm + 1))
    energy = max(min(energy_val, MAX_ENERGY), MIN_ENERGY)

    calorie = max(min(energy / ENERGY_TO_CAL, MAX_CALORIE), MIN_CALORIE)

    return Patm, energy, calorie
