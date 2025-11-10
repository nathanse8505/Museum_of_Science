"""
Filename: graph.py
Purpose: Graph class for the car plotter exhibit.
The graph is drawn on a sub-surface of the screen, and the function is defined by the user.
The function can be any callable that takes a single argument (x) and returns a single value (y).
"""

import pygame
from pygame.locals import *
import numpy as np
from consts import GRAPH_COLOR, GRAPH_LINE_WIDTH


class Graph:

    def __init__(self, screen, function, x_range, y_range, sub_surface, title, color=GRAPH_COLOR, width=GRAPH_LINE_WIDTH, step=1):
        """
        Initialize the graph with a function, x and y ranges, sub-surface, color and width.
        :param screen: The screen to draw on.
        :param function: The function to graph.
        :param x_range: The range of x values (x_min, x_max) for the graph.
        :param y_range: The range of y values (y_min, y_max) for the graph.
        :param sub_surface: (pos_x, pos_y, width, height) of the sub-surface (the area where the graph will be drawn).
        :param title: The title of the graph.
        :param color: The color of the graph.
        :param width: The width of the graph line.
        :param step: The step size for the x values.
        """
        self.screen = screen
        self.function = function
        self.x_range = x_range
        self.y_range = y_range
        self.sub_surface = sub_surface
        self.title = title
        self.color = color
        self.width = width
        self.step = step
        

    def draw(self):
        """
        Draw the graph on the screen.
        """

        pos_x, pos_y, width, height = self.sub_surface  
        graph_surface = pygame.Surface((width, height), pygame.SRCALPHA)  # Create a new surface with size of the sub-surface
        graph_surface.fill((0, 0, 0, 0))  # Fill with transparent color

        x_scale = width / (self.x_range[1] - self.x_range[0])  # Scale factor for x-axis
        y_scale = height / (self.y_range[1] - self.y_range[0])  # Scale factor for y-axis

        # Draw the graph
        points = []
        for x in np.arange(self.x_range[0], self.x_range[1], self.step):
            y = self.function(x)  # Calculate y value using the function
            points.append((x, y))  # Append the point to the list
        if len(points) < 2:
            return  # Not enough points to draw a line

        screen_points = [(int((x - self.x_range[0]) * x_scale),
                          int((y - self.y_range[0]) * y_scale)) for x, y in points]  # Convert to screen coordinates
        pygame.draw.lines(graph_surface, self.color, False, screen_points, self.width)  # Draw the line on the graph surface

        font = pygame.font.Font(None, 24)
        text_x_min = font.render(f"X: {round(self.x_range[0], 2)}", True, (0, 0, 0))
        text_x_max = font.render(f"X: {round(self.x_range[1], 2)}", True, (0, 0, 0))
        text_y_min = font.render(f"Y: {round(self.y_range[0], 2)}", True, (0, 0, 0))
        text_y_max = font.render(f"Y: {round(self.y_range[1], 2)}", True, (0, 0, 0))

        graph_surface.blit(text_x_min, (5, height - 15))
        graph_surface.blit(text_x_max, (width - text_x_max.get_width() - 5, height - 20))
        graph_surface.blit(text_y_min, (5, height - 30))
        graph_surface.blit(text_y_max, (5, 5))

        self.screen.blit(graph_surface, (pos_x, pos_y))  # Blit the graph surface onto the main screen

        # Draw the title
        font = pygame.font.Font(None, 45)
        title_surface = font.render(self.title, True, (0, 0, 0))
        title_rect = title_surface.get_rect(center=(pos_x + width // 2, pos_y + 20))
        self.screen.blit(title_surface, title_rect)