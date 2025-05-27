"""
Filename: Button.py
Purpose: Button class for creating buttons on the screen
"""

import pygame
import os
from consts import ASSETS_DIR

class Button(object):
    """
    Button class for creating buttons on the screen
    """
    def __init__(self, screen, pos, size, images_paths, function, execute_on_release=True):
        """
        Constructor for the Button class
        :param screen: screen to draw the button on
        :param pos: position of the button (x, y)
        :param size: size of the button (width, height)
        :param images: images of the button (when not clicked and clicked)
        :param function: function to call when the button is clicked
        """
        self.screen = screen
        self.pos = pos
        self.size = [size[0], size[0]] if size[1] is None else [size[1], size[1]] if size[0] is None else size  # compatable for [x, y], [x, None], or [None, y]
        self.function = function
        self.execute_on_release = execute_on_release  # if the function should be executed on release

        self.image = pygame.image.load(os.path.join(ASSETS_DIR, images_paths[0])).convert_alpha()  # load the image when not clicked
        self.image = pygame.transform.scale(self.image, (self.size[0], self.size[1]))
        self.image_clicked = pygame.image.load(os.path.join(ASSETS_DIR, images_paths[1])).convert_alpha()
        self.image_clicked = pygame.transform.scale(self.image_clicked, (self.size[0], self.size[1]))
        self.another_image = pygame.image.load(os.path.join(ASSETS_DIR, images_paths[2])).convert_alpha() if len(images_paths) > 2 else None
        self.another_image = pygame.transform.scale(self.another_image, (self.size[0], self.size[1])) if self.another_image else None

        self.enabled = True  # if the button is enabled
        self.clicked = False  # if the button is clicked
        self.current_image = self.image  # the image to draw on the button (this will be changed when the button is clicked)

    def update(self):
        """
        Update the button state, draw the button on the screen and call the function if clicked
        """

        mouse = pygame.mouse.get_pos()
        click = pygame.mouse.get_pressed()
        rect = pygame.Rect(self.pos[0], self.pos[1], self.size[0], self.size[1])
        on_button = rect.collidepoint(mouse)
        
        if self.enabled and on_button and click[0] == 1 and not self.clicked:
            self.clicked = True
            self.current_image = self.image_clicked
            
            if not self.execute_on_release and self.function:
                self.function()

        elif on_button and click[0] == 0 and self.clicked:
            self.clicked = False
            self.current_image = self.image
            
            if self.execute_on_release and self.function:
                self.function()

        elif not on_button and click[0] == 0 and self.clicked:
            self.clicked = False

            if self.execute_on_release:
                self.current_image = self.image

        elif not on_button and click[0] == 1:
            self.enabled = False

        elif click[0] == 0:
            self.clicked = False
            self.current_image = self.image
            self.enabled = True

    def render(self):
        self.screen.blit(self.current_image, (self.pos[0], self.pos[1]))
