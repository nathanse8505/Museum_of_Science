"""
Filename: asset_loader.py
Purpose: Asset loading functions for the car plotter exhibit
"""

import os
import pygame

class AssetLoader:
    """
    This class is responsible for loading assets for the exhibit.
    """

    def __init__(self, folder_path, pictures_dict, viewport_size=(800, 600)):
        """
        Initialize the AssetLoader with a folder path and default size.
        :param folder_path: Path to the folder containing the assets.
        :param pictures_dict: Dictionary of picture names, their sizes and positions to load on screen.
        :param viewport_size: Size of the viewport (width, height).
        """
        self.folder_path = folder_path
        self.pictures_dict = pictures_dict
        self.viewport_size = viewport_size
        self.pictures = {}

        if not os.path.exists(folder_path):
            raise FileNotFoundError(f"Folder {folder_path} does not exist.")
        if not os.path.isdir(folder_path):
            raise NotADirectoryError(f"{folder_path} is not a directory.")
        if not os.access(folder_path, os.R_OK):
            raise PermissionError(f"Folder {folder_path} is not readable.")
        
        self.pictures = self.load_pictures()


    def load_pictures(self):
        """
        Load all pictures from the folder that are in pictures_dict and resize them according to the specified sizes and positions.
        :return: A dictionary with values of [loaded pictures, positions] with their names as keys.
        """

        pictures = {}

        for filename, size_pos in self.pictures_dict.items():
            
            if filename.endswith('.png') or filename.endswith('.jpg') or filename.endswith('.jpeg') or filename.endswith('.gif'):
                image_path = os.path.join(self.folder_path, filename)

                print(f"Loading image: {image_path}")
                size, pos = size_pos
                new_size, new_pos = [0, 0], [0, 0]

                # calculate the size
                for k in [0, 1]:
                    if size[k] == "full":
                        new_size[k] = self.viewport_size[k]
                        continue
                    elif isinstance(size[k], str) and size[k].endswith("%"):
                        percent = float(size[k][:-1]) / 100
                        new_size[k] = int(self.viewport_size[k] * percent)
                        continue
                    elif isinstance(size[k], int):
                        new_size[k] = size[k]
                        continue
                    else:
                        raise ValueError(f"Invalid size {size[k]} for image {filename}")
                    
                # calculate the position
                for k in [0, 1]:
                    if pos[k] == "full":
                        new_pos[k] = 0
                        continue
                    if pos[k] == "center":
                        new_pos[k] = int((self.viewport_size[k] - new_size[k]) / 2)
                        continue
                    if pos[k] == "left":
                        new_pos[k] = 0
                        continue
                    if pos[k] == "right":
                        new_pos[k] = self.viewport_size[k] - new_size[k]
                        continue
                    if pos[k] == "top":
                        new_pos[k] = 0
                        continue
                    if pos[k] == "bottom":
                        new_pos[k] = self.viewport_size[k] - new_size[k]
                        continue
                    elif isinstance(pos[k], str) and pos[k].endswith("%"):
                        percent = float(pos[k][:-1]) / 100
                        new_pos[k] = int(self.viewport_size[k] * percent)
                        continue
                    elif isinstance(pos[k], int):
                        new_pos[k] = pos[k]
                        continue
                    else:
                        raise ValueError(f"Invalid position {pos[k]} for image {filename}")
                
                pictures[filename.split('.')[0]] = [pygame.transform.scale(
                    pygame.image.load(image_path).convert_alpha(), new_size
                ), new_pos]
            else:
                print(f"Skipping non-image file: {filename}")
        
        return pictures


    def render(self, screen):
        """
        Render all loaded pictures on the screen at their specified positions.
        :param screen: The screen to render the pictures on.
        """
        for _, (image, pos) in self.pictures.items():
            screen.blit(image, pos)


def convert_to_pixels(value, viewport):
        """
        Convert a value to pixels based on the viewport size.
        :param value: The value to convert (can be a percentage or an absolute value).
        :param viewport: The size of the viewport (width, height).
        :return: The value in pixels.
        """
        if value == "full":
            return viewport
        elif value == "center":
            return viewport // 2
        if isinstance(value, str) and value.endswith("%"):
            percent = float(value[:-1]) / 100
            return int(viewport * percent)
        elif isinstance(value, int):
            return value
        else:
            raise ValueError(f"Invalid value {value} for conversion to pixels.")