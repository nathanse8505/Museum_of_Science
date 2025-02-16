"""
Filename: display.py
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
        screen.blit(open_heb, (0,0))

    elif language == ENGLISH:
        screen.blit(open_eng, (0,0))

    elif language == ARABIC:
        screen.blit(open_arb, (0,0))


def display_measure(screen, language=HEBREW, pressure_value=MIN_PRESSURE_VALUE):
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


    # sub function to calculate the current, charge and energy from the voltage and capacitance
    Patm = max(min(pressure_value / AIR_PRESSURE_TO_P_ATM , MAX_P_ATM),MIN_P_ATM)
    energy = max(min(pressure_value * 1000 * V,MAX_ENERGY),MIN_ENERGY)
    calorie = max(min(energy / ENERGY_TO_CAL,MAX_CALORIE),MIN_CALORIE)

    display_bars(screen, calorie, energy)
    display_rotated_niddle(screen, Patm)
    display_text_values(screen, Patm, calorie, energy)


def display_bars(screen, calorie=MIN_CALORIE, energy=MIN_ENERGY):
    """
    Display the bar on the screen according to the values
    :param screen: the screen to display the bar on
    :param voltage: the voltage to display
    :param charge: the charge to display
    :param energy: the energy to display
    """

    # sub function to reduce code duplication
    def display_bar_from_values(screen, value, max, min, bar_image):
        """
        sub function to display the bar on the screen according to the value and the max and min values
        """
        bar_width = bar_image.get_width()
        bar_height = bar_image.get_height()

        fill_height = int((value - min) / (max - min) * BAR_SIZE)
        crop_rect = pygame.Rect(0, bar_height - fill_height, bar_width, fill_height)
        cropped_bar = bar_image.subsurface(crop_rect).copy()
        pos_x = BAR_GRAPH_SHIFT
        pos_y = int(BAR_GRAPH_BOTTOM_HEIGHT - fill_height)
        screen.blit(cropped_bar, (pos_x, pos_y))


    display_bar_from_values(screen, calorie, MAX_CALORIE, MIN_CALORIE, bar_full_calorie)
    display_bar_from_values(screen, energy, MAX_ENERGY, MIN_ENERGY, bar_full_energy)


def display_text_values(screen, Patm=MIN_P_ATM, calorie=MIN_CALORIE, energy=MIN_ENERGY):
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

    display_text(screen, Patm, P_ATM_TEXT_POS, TEXT_SIZE, TEXT_COLOR)
    display_text(screen, calorie, CALORIE_TEXT_POS, TEXT_SIZE, TEXT_COLOR)
    display_text(screen, energy, ENERGY_TEXT_POS, TEXT_SIZE, TEXT_COLOR)

def display_rotated_niddle(screen, Patm):
    """
    Display the rotated niddle based on the current value
    :param screen: the screen to display the bar on
    :param current: the current value
    """
    angle = 180 * (Patm - MIN_P_ATM) / (MAX_P_ATM - MIN_P_ATM)
    rotated_niddle = pygame.transform.rotate(niddle, -angle)
    rect = rotated_niddle.get_rect(center=NIDDLE_POS)
    screen.blit(rotated_niddle, rect.topleft)

def calculate_work(pressure_value):
    # Calculate the number of moles of air added
    n_added = (pressure_value * V) / (R * T) - (P_i * V) / (R * T)

    # Calculate isothermal work in Joules
    energy = n_added * R * T * np.log(pressure_value / P_i)


    return energy