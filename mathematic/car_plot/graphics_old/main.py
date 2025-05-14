"""
File: main.py
Purpose: Main function for the car plotter exhibit
"""

import math
import pygame
from pygame.locals import *
from consts import *
from asset_loader import *
from graph import Graph
from user import User
from logs import *
from joystick import Joystick


def main():

    logger = get_logger()
    logger.info("Starting Car Plotter Exhibit")
    
    pygame.init()
    pygame.display.set_caption("Car Plot")
    clock = pygame.time.Clock()
    pygame.mouse.set_visible(False)

    joystick = Joystick(logger=logger)
    
    if FULLSCREEN:
        screen = pygame.display.set_mode((0, 0), pygame.FULLSCREEN)
        screen_width, screen_height = pygame.display.Info().current_w, pygame.display.Info().current_h
        view_port = (screen_width, screen_height)

    else:
        screen = pygame.display.set_mode(VIEWPORT)
        view_port = VIEWPORT
    
    asset_loader = AssetLoader(ASSETS_DIR, PICTURES_TO_LOAD, view_port)
    user = User(
                screen,
                (0, 1000), (-500, 500),
                (
                    asset_loader.pictures["grid"][1][0],
                    asset_loader.pictures["grid"][1][1],
                    asset_loader.pictures["grid"][0].get_width(),
                    asset_loader.pictures["grid"][0].get_height()
                ),
            )

    def cool_function(x):
        """
        x: in range [0, 1000]
        returns y in range roughly [-500, 500]
        """
        y = 250 * math.sin(x * 0.02)                               # Base wave
        y += 120 * math.sin(x * 0.1 + math.sin(x * 0.03))          # Nested sine wave
        y += 90 * math.cos(x * 0.005 + math.sin(x * 0.01))         # Curvy wiggles
        y += 70 * math.tan(math.sin(x * 0.004)) / 2                # Occasional spikes
        y += 80 * math.exp(-((x - 700)**2) / 5000)                 # Sharp bump
        return y
    
    # Create a graph object
    graphs = [
            Graph(
                screen,
                lambda x: 500 - x,
                (0, 1000), (-500, 500),
                (
                    asset_loader.pictures["grid"][1][0],
                    asset_loader.pictures["grid"][1][1],
                    asset_loader.pictures["grid"][0].get_width(),
                    asset_loader.pictures["grid"][0].get_height()
                ),
                title="Linear Function"
            ),
            Graph(
                screen,
                lambda x: -0.001 * (x)**2 + 500,
                (0, 1000), (-500, 500),
                (
                    asset_loader.pictures["grid"][1][0],
                    asset_loader.pictures["grid"][1][1],
                    asset_loader.pictures["grid"][0].get_width(),
                    asset_loader.pictures["grid"][0].get_height()
                ),
                title="Quadratic Function"
            ),
            Graph(
                screen,
                lambda x: 0.0036*(x-500)**2 - 400,
                (0, 1000), (-500, 500),
                (
                    asset_loader.pictures["grid"][1][0],
                    asset_loader.pictures["grid"][1][1],
                    asset_loader.pictures["grid"][0].get_width(),
                    asset_loader.pictures["grid"][0].get_height()
                ),
                title="Stop and Go"
            ),
            Graph(
                screen,
                lambda x: 300 * math.sin(0.02*x),
                (0, 1000), (-500, 500),
                (
                    asset_loader.pictures["grid"][1][0],
                    asset_loader.pictures["grid"][1][1],
                    asset_loader.pictures["grid"][0].get_width(),
                    asset_loader.pictures["grid"][0].get_height()
                ),
                title="Sine Function"
            ),
            Graph(
                screen,
                lambda x: 0,
                (0, 1000), (-500, 500),
                (
                    asset_loader.pictures["grid"][1][0],
                    asset_loader.pictures["grid"][1][1],
                    asset_loader.pictures["grid"][0].get_width(),
                    asset_loader.pictures["grid"][0].get_height()
                ),
                title="Zero Function"
            ),
            Graph(
                screen,
                cool_function,
                (0, 1000), (-500, 500),
                (
                    asset_loader.pictures["grid"][1][0],
                    asset_loader.pictures["grid"][1][1],
                    asset_loader.pictures["grid"][0].get_width(),
                    asset_loader.pictures["grid"][0].get_height()
                ),
                title="Complicated Function"
            )]
    graph_index = 0

    # Main loop
    running = True
    while running:
        for event in pygame.event.get():
            if event.type == QUIT or (event.type == KEYDOWN and event.key == K_ESCAPE):
                running = False

            if event.type == KEYDOWN:
                if event.key == K_UP:
                    user.move_y(True)
                
                elif event.key == K_DOWN:
                    user.move_y(False)
                
                elif event.key == K_LEFT:
                    graph_index = (graph_index - 1) % len(graphs)
                    user.reset()

                elif event.key == K_RIGHT:
                    graph_index = (graph_index + 1) % len(graphs)
                    user.reset()
            
            if event.type == pygame.MOUSEWHEEL:
                if event.y > 0:
                    user.move_y(True)
                else:
                    user.move_y(False)

            if event.type == pygame.JOYDEVICEREMOVED:
                logger.info("Joystick disconnected.")
                logger.info("Trying to reconnect...")
                joystick.joystick = None
                joystick.reconnect_waiting = True

            elif event.type == pygame.JOYDEVICEADDED:
                if joystick.reconnect_waiting:
                    joystick.joystick = joystick.try_connect()
                    if joystick.joystick:
                        joystick.reconnect_waiting = False

        if joystick.joystick:
            joystick.get_value()
            user.set_y(joystick.map_value(joystick.value, 500, -500))

        user.add_point()
        has_done_graph = user.move_x()

        if has_done_graph:
            graph_index = (graph_index + 1) % len(graphs)

        user.calc_score(graphs[graph_index])

        screen.fill(BLACK)
        asset_loader.render(screen)
        graphs[graph_index].draw()
        user.render_all()

        pygame.display.flip()
        clock.tick(1000)


if __name__ == "__main__":
    main()