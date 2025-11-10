"""
Filename: consts.py
Purpose: Constants for the Hydrogen Rocket UI
"""
import os
import pygame

FPS = 100

# arduino
BAUDRATE = 115200
SERIAL_ERROR = -1
RECONNECT_INTERVAL = 1  # seconds

PARSE_VALID = 0
PARSE_ERROR = 1

# logging values
LOG_FOLDER = os.path.join(os.path.dirname(__file__), "logs")  # get the path of the logs folder
MAX_SIZE_PER_LOG_FILE = 1 * 1024 * 1024  # 1MB
BACKUP_COUNT = 10  # max number of log files, if all 10 are full, the first one will be deleted, rotating the rest
