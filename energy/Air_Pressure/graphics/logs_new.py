import logging
import os
from datetime import datetime
from consts import MAX_SIZE_PER_LOG_FILE, LOG_FOLDER
import re


class DateBasedFileHandler(logging.Handler):
    def __init__(self, log_folder, max_bytes):
        super().__init__()
        self.log_folder = log_folder
        self.max_bytes = max_bytes
        self.base_date = datetime.now().strftime("%Y-%m-%d")
        self.current_index = self._get_last_index()  # ← nouveau
        self.current_log_file = None
        self.current_file_handler = None
        self._create_new_log_file()

    def _get_log_filename(self, index=None):
        index = self.current_index if index is None else index
        suffix = f"({index})" if index > 0 else ""
        filename = f"log_{self.base_date}{suffix}.txt"
        return os.path.join(self.log_folder, filename)

    def _get_last_index(self):
        """Trouve le plus grand index existant pour la date d'aujourd'hui (avec suffixes entre parenthèses)"""
        pattern = re.compile(rf"log_{self.base_date}(?:\((\d+)\))?\.txt$")
        max_index = 0

        try:
            for filename in os.listdir(self.log_folder):
                match = pattern.match(filename)
                if match:
                    idx = int(match.group(1)) if match.group(1) else 0
                    max_index = max(max_index, idx)
        except FileNotFoundError:
            pass  # le dossier n'existe pas encore

        return max_index

    def _create_new_log_file(self):
        self.current_log_file = self._get_log_filename()

        if self.current_file_handler:
            self.current_file_handler.close()

        self.current_file_handler = logging.FileHandler(self.current_log_file, mode="a", encoding="utf-8")
        self.current_file_handler.setFormatter(logging.Formatter(
            "%(asctime)s - %(message)s", datefmt="%Y-%m-%d %H:%M:%S"
        ))

    def emit(self, record):
        today = datetime.now().strftime("%Y-%m-%d")

        if today != self.base_date:
            # Nouveau jour : on redémarre à zéro
            self.base_date = today
            self.current_index = 0
            self._create_new_log_file()

        if self.current_file_handler.stream.tell() > self.max_bytes:
            self.current_index += 1
            self._create_new_log_file()

        self.current_file_handler.emit(record)

    def close(self):
        if hasattr(self, "current_file_handler") and self.current_file_handler:
            self.current_file_handler.close()
        super().close()


def get_logger():
    os.makedirs(LOG_FOLDER, exist_ok=True)

    logger = logging.getLogger("date_logger")
    logger.setLevel(logging.INFO)

    # Avoid adding multiple handlers if already configured
    if not logger.handlers:
        date_handler = DateBasedFileHandler(LOG_FOLDER, MAX_SIZE_PER_LOG_FILE)
        logger.addHandler(date_handler)

    return logger



def log_air_pressure(logger, pressure_value, check_pressure_value, language, previous_language):
    if check_pressure_value and pressure_value >= SWITCH_TO_MEASURE_SCREEN_PRESSURE_THRESHOLD:
        # print(f"your horsepower is: {horsepower} in {deltatime} second")
        logger.info(f"The Bottle flew!")
        check_pressure_value = False
    elif pressure_value < SWITCH_TO_MEASURE_SCREEN_PRESSURE_THRESHOLD:
        check_pressure_value = True

    if language != previous_language:
        logger.info(f"your language is: {dic_lang.get(language)}")
        previous_language = language
    return check_pressure_value, previous_language


