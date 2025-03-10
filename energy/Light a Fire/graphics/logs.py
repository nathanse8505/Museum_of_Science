"""
Filename: logs.py
Purpose: Logging functions for the Air Pressure UI
"""
import logging
from logging.handlers import RotatingFileHandler
import os
from consts import MAX_SIZE_PER_LOG_FILE, BACKUP_COUNT, LOG_FOLDER, SWITCH_TO_MEASURE_SCREEN_TEMPERATURE_THRESHOLD


def get_logger():
    """
    Setup logging into a file called log.txt in the folder /logs with the format: [TIME] - [MESSAGE].
    If it exceeds 1MB (MAX_SIZE_PER_LOG_FILE), create a new file with a number suffix (before the .txt) and continue logging to it (e.g., log1.txt, log2.txt, etc.)
    """
    os.makedirs(LOG_FOLDER, exist_ok=True)  # create the logs folder if it doesn't exist
    log_file = os.path.join(LOG_FOLDER, "log.txt")

    # Configure the logging
    logging.basicConfig(
        level=logging.INFO,
        format="%(asctime)s - %(message)s",  # Format: [TIME] - [MESSAGE]
        datefmt="%Y-%m-%d %H:%M:%S",
        handlers=[
            RotatingFileHandler(
                log_file, mode="a", maxBytes=MAX_SIZE_PER_LOG_FILE, backupCount=BACKUP_COUNT
            )
        ],
    )

    logger = logging.getLogger()  # get the logger
    return logger


def log_temperature(logger, temperature, check_temperature_value):
    if check_temperature_value and temperature >= SWITCH_TO_MEASURE_SCREEN_TEMPERATURE_THRESHOLD:
        logger.info(f"The Bottle flew!")
        check_temperature_value = False
    elif temperature < SWITCH_TO_MEASURE_SCREEN_TEMPERATURE_THRESHOLD:
        check_temperature_value = True

    return check_temperature_value
