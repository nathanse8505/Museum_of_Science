"""
Filename: user.py
Purpose: User class for the car plotter exhibit. It handles the user movement, drawing of the user's graph and calculating the score based on the user's graph.
"""

import pygame
from pygame.locals import *
from consts import USER_GRAPH_COLOR, USER_GRAPH_MAX_POINTS, USER_GRAPH_STEP, USER_GRAPH_LINE_WIDTH, BLUE, YELLOW
from asset_loader import convert_to_pixels

class User:
    def __init__(self, screen, x_range, y_range, sub_surface, max_points=USER_GRAPH_MAX_POINTS, color=USER_GRAPH_COLOR, graph_line_width=USER_GRAPH_LINE_WIDTH, step=[USER_GRAPH_STEP, 10]):
        """
        Initialize the user with a position, a list of points, and a step size.
        :param screen: The screen to draw on.
        :param x_range: The range of x values (min_x, max_x) (to normalize the graph).
        :param y_range: The range of y values (min_y, max_y) (to normalize the graph).
        :param sub_surface: (pos_x, pos_y, width, height) of the sub-surface (the area where the graph will be drawn).
        :param max_points: The maximum number of points to consider for the score calculation.
        :param color: The color of the graph.
        """
        self.screen = screen  # The screen to draw on
        self.x_range = x_range  # Range of x values (min_x, max_x) (to normalize the graph)
        self.y_range = y_range  # Range of y values (min_y, max_y) (to normalize the graph)
        self.sub_surface = sub_surface  # (pos_x, pos_y, width, height) of the sub-surface
        self.max_points = max_points  # Maximum number of points to consider for the score calculation
        self.color = color  # Color of the graph
        self.graph_line_width = graph_line_width
        self.step = step  # Step size for x and y movements

        self.score = 0  # Initialize score to 0
        self.position = [self.x_range[0], self.y_range[1]]  # Initial position of the user
        self.user_points = []  # List to store user points

        _, _, width, height = self.sub_surface
        self.scale = [width / (self.x_range[1] - self.x_range[0]), height / (self.y_range[1] - self.y_range[0])]  # Scale factors for x and y axes

    def reset(self):
        """
        Reset the user position and points.
        """
        self.position[0] = self.x_range[0]  # Reset x position to min_x
        self.user_points.clear()  # Clear user points
    
    def move_x(self):
        """
        Move the user in the x direction.
        :return: True if the user has wrapped around the x-axis, False otherwise.
        """
        self.position[0] += self.step[0]

        if self.position[0] > self.x_range[1]:
            self.reset()  # Reset the user position if it exceeds max_x (wrap around)
            return True
        
        return False

    def move_y(self, direction_up=True):
        self.position[1] += -self.step[1] if direction_up else self.step[1]

        if self.position[1] > self.y_range[1]:
            self.position[1] = self.y_range[1]

        elif self.position[1] < self.y_range[0]:
            self.position[1] = self.y_range[0]
    
    def set_y(self, y):
        self.position[1] = y

    def add_point(self):
        self.user_points.append(self.position.copy())

    def calc_score(self, graph, max_error=100000):
        """
        Calculate the score based on the mean squared error (MSE) between the user points and the graph function.
        The score is calculated as:
        score = 100 * (1 - mse / max_error)
        where mse is the mean squared error between the user points and the graph function.
        :param graph: The graph object.
        :param max_error: The maximum error allowed, used to normalize the score.
                            If the MSE is greater than max_error, the score will be 0.
        :return: The score as a percentage (0 to 100).
        """
        if len(self.user_points) < 2:
            return 0
        errors = [(y - graph.function(x))**2 for x, y in self.user_points[-self.max_points:]]  # Calculate the squared errors for the last max_points points
        mse = sum(errors) / len(errors)
        self.score = round(max(0, 100 * (1 - mse / max_error)))
        return self.score
    
    def show_score(self, bar):
        """
        Display the score on the screen.
        :param bar: (pos_x, pos_y, width, height) of the bar to draw the score.
        """

        def draw_gradient_bar(screen, pos_x, pos_y, width, height, value):
            """
            Draws a vertical bar with a green-to-red gradient based on the value (0–100).
            :param screen: The screen to draw on.
            :param pos_x: The x position of the bar.
            :param pos_y: The y position of the bar.
            :param width: The width of the bar.
            :param height: The height of the bar.
            :param value: The value to represent (0–100).
            """
            # Clamp value between 0 and 100
            value = max(0, min(100, value))
            
            # Height to draw based on value
            visible_height = int(height * (value / 100))

            # Draw gradient from bottom to top (red to green)
            for i in range(visible_height):
                # Interpolate color: red (255,0,0) to green (0,255,0)
                ratio = i / height
                r = int(255 * (1 - ratio))
                g = int(255 * ratio)
                color = (r, g, 0)

                # Draw horizontal line (1-pixel high rect)
                pygame.draw.rect(
                    screen,
                    color,
                    pygame.Rect(pos_x, pos_y + height - i - 1, width, 1)
                )

        # Draw the gradient bar
        pos_x = convert_to_pixels(bar[0], self.screen.get_width())
        pos_y = convert_to_pixels(bar[1], self.screen.get_height())
        width = convert_to_pixels(bar[2], self.screen.get_width())
        height = convert_to_pixels(bar[3], self.screen.get_height())        
        draw_gradient_bar(self.screen, pos_x, pos_y, width, height, self.score)

        font = pygame.font.Font(None, 70)
        text_surface = font.render(f"{self.score}%", True, (0, 0, 0))
        text_rect = text_surface.get_rect(center=(pos_x + width // 2, pos_y - 30))
        self.screen.blit(text_surface, text_rect)  # Blit the text surface onto the main screen


    def draw_graph(self):
        """
        Draw the user's graph on the screen.
        """
        if len(self.user_points) < 2:
            return

        pos_x, pos_y, width, height = self.sub_surface  
        graph_surface = pygame.Surface((width, height), pygame.SRCALPHA)  # Create a new surface with size of the sub-surface
        graph_surface.fill((0, 0, 0, 0))  # Fill with transparent color

        x_scale = width / (self.x_range[1] - self.x_range[0])  # Scale factor for x-axis
        y_scale = height / (self.y_range[1] - self.y_range[0])  # Scale factor for y-axis

        screen_points = [(int((x - self.x_range[0]) * x_scale),
                          int((y - self.y_range[0]) * y_scale)) for x, y in self.user_points]  # Convert to screen coordinates
        pygame.draw.lines(graph_surface, self.color, False, screen_points, self.graph_line_width)  # Draw the line on the graph surface

        self.screen.blit(graph_surface, (pos_x, pos_y))  # Blit the graph surface onto the main screen

    def draw_user_lines(self):
        """
        Draw the lines from the user position to the edges of the graph.
        """
        pos_x, pos_y, width, height = self.sub_surface
        
        pygame.draw.line(self.screen, BLUE, (pos_x + int((self.position[0] - self.x_range[0]) * self.scale[0]),
                                                pos_y + int((self.position[1] - self.y_range[0]) * self.scale[1])),
                                                (pos_x, pos_y + int((self.position[1] - self.y_range[0]) * self.scale[1])), 2)
        pygame.draw.line(self.screen, BLUE, (pos_x + int((self.position[0] - self.x_range[0]) * self.scale[0]),
                                                pos_y + int((self.position[1] - self.y_range[0]) * self.scale[1])),
                                                (pos_x + int((self.position[0] - self.x_range[0]) * self.scale[0]),
                                                pos_y + height), 2)
        
        pygame.draw.circle(self.screen, YELLOW, (pos_x + int((self.position[0] - self.x_range[0]) * self.scale[0]),
                                                pos_y + int((self.position[1] - self.y_range[0]) * self.scale[1])), 8)
        
        if len(self.user_points) >= self.max_points:
            pygame.draw.line(self.screen, (128, 128, 128), (pos_x + int((self.user_points[-self.max_points][0] - self.x_range[0]) * self.scale[0]),
                                                    pos_y + height),
                                                    (pos_x + int((self.position[0] - self.x_range[0]) * self.scale[0]),
                                                    pos_y + height), 8)
            
        elif len(self.user_points) > 0:
            pygame.draw.line(self.screen, (128, 128, 128), (pos_x + int((self.user_points[0][0] - self.x_range[0]) * self.scale[0]),
                                                    pos_y + height),
                                                    (pos_x + int((self.position[0] - self.x_range[0]) * self.scale[0]),
                                                    pos_y + height), 8)
            
    def render_all(self):
        """
        Render all user elements on the screen.
        """
        self.draw_graph()
        self.show_score(bar=("10%", "35%", "7%", "40%"))
        self.draw_user_lines()