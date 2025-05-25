"""
Filename: logs.py
Purpose: Logging functions for the Air Pressure UI
"""
import logging
from logging.handlers import RotatingFileHandler
import os
from consts import MAX_SIZE_PER_LOG_FILE, BACKUP_COUNT, LOG_FOLDER, TEMP_AFTER_PEAK,TEMP_NEW_USER


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


def log_temperature(logger, temperature, state, last_peak_temperature=None):
    """
    Detect and log a true peak temperature after a 2°C drop, and wait for any upward trend to start tracking again.

    States:
    - "idle": waiting to start tracking a peak
    - "tracking_peak": monitoring for a maximum
    - "after_drop": logged a peak, waiting for temperature to rise again (any rise)
    """
    if state == "idle":
        last_peak_temperature = temperature
        state = "tracking_peak"

    elif state == "tracking_peak":
        if temperature > last_peak_temperature:
            last_peak_temperature = temperature  # peak still rising
        elif temperature <= last_peak_temperature - TEMP_AFTER_PEAK:
            logger.info(f"Peak temperature reached: {last_peak_temperature:.2f}°C")
            #print(f"Peak temperature reached: {last_peak_temperature:.2f}°C")
            state = "after_drop"

    elif state == "after_drop":
        if temperature < TEMP_NEW_USER:
            #print("enter after drop")
            # any rising trend means we can restart
            state = "idle"
            last_peak_temperature = None

    return state, last_peak_temperature

