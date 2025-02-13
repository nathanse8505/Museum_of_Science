"""
Filename: display.py
Purpose: Display functions for the Jumping Ring UI
"""
from consts import *


def display_state(screen, state=MEASURE, language=HEBREW, voltage=MIN_VOLTAGE):
    """
    Display the state screen
    :param screen: the screen to display the state screen on (right now there is only MEASURE state)
    :param language: the language to display the state screen in
    :param state: the state to display
    """
    if state == MEASURE:
        display_measure(screen, language=language, voltage=voltage)


def display_measure(screen, language=HEBREW, voltage=MIN_VOLTAGE):
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
    calculate_charge_and_energy = lambda voltage: (max(min(voltage * CAPACITANCE, MAX_CHARGE), MIN_CHARGE), max(min(0.5 * CAPACITANCE * voltage ** 2, MAX_ENERGY), MIN_ENERGY))

    charge, energy = calculate_charge_and_energy(voltage)
    display_bars(screen, voltage, charge, energy)
    display_text_values(screen, voltage, charge, energy)


def display_bars(screen, voltage=MIN_VOLTAGE, charge=MIN_CHARGE, energy=MIN_ENERGY):
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
        height = int((value - min) / (max - min) * BAR_SIZE[1])
        crop_rect = pygame.Rect(0, BAR_GRAPH_BOTTOM_HEIGHT - height, VIEW_PORT[0], VIEW_PORT[1] - BAR_GRAPH_BOTTOM_HEIGHT + height)
        cropped_bar = bar_image.subsurface(crop_rect).copy()
        screen.blit(cropped_bar, (0, BAR_GRAPH_BOTTOM_HEIGHT - height))


    display_bar_from_values(screen, voltage, MAX_VOLTAGE, MIN_VOLTAGE, bar_full_voltage)
    display_bar_from_values(screen, charge, MAX_CHARGE, MIN_CHARGE, bar_full_charge)
    display_bar_from_values(screen, energy, MAX_ENERGY, MIN_ENERGY, bar_full_energy)


def display_text_values(screen, voltage=MIN_VOLTAGE, charge=MIN_CHARGE, energy=MIN_ENERGY):
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

    display_text(screen, voltage, VOLTAGE_TEXT_POS, TEXT_SIZE, TEXT_COLOR)
    display_text(screen, charge, CHARGE_TEXT_POS, TEXT_SIZE, TEXT_COLOR)
    display_text(screen, energy, ENERGY_TEXT_POS, TEXT_SIZE, TEXT_COLOR)


class VoltageMonitor:
    def __init__(self, noise_threshold=10):
        self.previous_voltage = 0
        self.noise_threshold = noise_threshold

    def detect_drop(self, voltage ,logger):
        #print(f"Voltage: {voltage}V")
        if self.previous_voltage > voltage + self.noise_threshold and voltage < VOLTAGE_AFTER_DROP:
            logger.info(f"Ring jumped!")
        self.previous_voltage = voltage
