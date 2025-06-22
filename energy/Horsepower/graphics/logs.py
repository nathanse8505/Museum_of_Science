"""
Filename: logs.py
Purpose: Logging functions for the Horse Power UI
"""
import logging
from logging.handlers import RotatingFileHandler
import os
from consts import MAX_SIZE_PER_LOG_FILE, BACKUP_COUNT, LOG_FOLDER, dic_lang , MAX_HORSEPOWER


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


def log_horsepower(logger, horsepower, check_horse_power, language, previous_language):
    if check_horse_power and horsepower != 0 and horsepower < MAX_HORSEPOWER:
        # print(f"your horsepower is: {horsepower} in {deltatime} second")
        logger.info(f"your horsepower is: {horsepower}")
        check_horse_power = False
    elif check_horse_power and horsepower == MAX_HORSEPOWER:
        logger.info(f"reset Arduino")
        check_horse_power = False
    elif horsepower == 0:#reset measuremnt
        check_horse_power = True

    if language != previous_language:
        logger.info(f"your language is: {dic_lang.get(language)}")
        previous_language = language
    return check_horse_power, previous_language
