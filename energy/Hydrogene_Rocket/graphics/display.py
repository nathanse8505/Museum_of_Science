"""
Filename: image_processing.py
Purpose: Display functions for the Hydrogen Rocket UI
"""
from consts import *


def display_state(screen, state, language=HEBREW, charge=0, current=0):
    """
    Display the state screen
    :param screen: the screen to display the state screen on
    :param language: the language to display the state screen in
    :param state: the state to display
    """
    if state == OPENING:
        display_opening(screen, language=language)

    elif state == MEASURE:
        display_measure(screen, language=language, charge=charge, current=current)


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


def display_measure(screen, language, charge=0, current=0):
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

    display_bar(screen, charge)
    screen.blit(bar_empty, (0,0))
    
    screen.blit(gauge, (0,0))
    # calculate the angle of the niddle (mapping the current to the angle 0-180)
    display_rotated_niddle(screen, current)
    display_current(screen, current)
    display_charge(screen, charge)


def display_rotated_niddle(screen, current):
    """
    Display the rotated niddle based on the current value
    :param screen: the screen to display the bar on
    :param current: the current value
    """
    angle = 180 * (current - MIN_CURRENT) / (MAX_CURRENT - MIN_CURRENT)
    rotated_niddle = pygame.transform.rotate(niddle, -angle)
    rect = rotated_niddle.get_rect(center=NIDDLE_POS)
    screen.blit(rotated_niddle, rect.topleft)


def display_bar(screen, charge):
    """
    Display the bar on the screen according to the charge
    :param screen: the screen to display the bar on
    :param charge: the charge value
    """
    height = int((charge - MIN_CHARGE) / (MAX_CHARGE - MIN_CHARGE) * BAR_SIZE[1])
    crop_rect = pygame.Rect(0, BAR_GRAPH_BOTTOM_HEIGHT - height, VIEW_PORT[0], VIEW_PORT[1] - BAR_GRAPH_BOTTOM_HEIGHT + height)
    cropped_bar = bar_full.subsurface(crop_rect).copy()
    screen.blit(cropped_bar, (0, BAR_GRAPH_BOTTOM_HEIGHT - height))


def display_current(screen, current):
    """
    Display the current on the screen
    :param screen: the screen to display the current on
    :param current: the current to display
    """
    font = pygame.font.Font(None, CURRENT_TEXT_SIZE)
    text = font.render(f"{current:.2f}", True, CURRENT_TEXT_COLOR)
    text_rect = text.get_rect(center=CURRENT_TEXT_POS)
    screen.blit(text, text_rect)


def display_charge(screen, charge):
    """
    Display the charge on the screen
    :param screen: the screen to display the charge on
    :param charge: the charge to display
    """
    font = pygame.font.Font(None, CHARGE_TEXT_SIZE)
    text = font.render(f"{charge:.2f}", True, CHARGE_TEXT_COLOR)
    text_rect = text.get_rect(center=CHARGE_TEXT_POS)
    screen.blit(text, text_rect)


def play_audio(file_path):
    """Lit un fichier audio avec pygame."""
    pygame.mixer.init()
    pygame.mixer.music.load(file_path)
    pygame.mixer.music.play()