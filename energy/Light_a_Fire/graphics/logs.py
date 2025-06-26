"""
Filename: logs.py
Purpose: Logging functions for the Air Pressure UI
"""
import logging
import os
from consts import MAX_SIZE_PER_LOG_FILE, LOG_FOLDER,BACKUP_COUNT, TEMP_AFTER_PEAK, TEMP_NEW_USER
from datetime import datetime
import re


class DateBasedFileHandler(logging.Handler):
    def __init__(self, log_folder, max_bytes):
        super().__init__()
        self.log_folder = log_folder
        self.max_bytes = max_bytes
        self.base_date = datetime.now().strftime("%Y-%m-%d")
        self.current_index = 0
        self.current_log_file = None
        self.current_file_handler = None
        self.backup_count = BACKUP_COUNT

        self._select_startup_file()  # Nouveau comportement au démarrage

    def _get_log_filename(self, date_str, index=0):
        suffix = f"({index})" if index > 0 else ""
        filename = f"log_{date_str}{suffix}.txt"
        return os.path.join(self.log_folder, filename)

    def _select_startup_file(self):
        """Trouve le dernier fichier existant, continue s’il n’est pas plein. Sinon crée nouveau fichier."""
        pattern = re.compile(r"log_(\d{4}-\d{2}-\d{2})(?:\((\d+)\))?\.txt$")
        files = []

        try:
            for fname in sorted(os.listdir(self.log_folder)):
                match = pattern.match(fname)
                if match:
                    date_str = match.group(1)
                    idx = int(match.group(2)) if match.group(2) else 0
                    path = os.path.join(self.log_folder, fname)
                    size = os.path.getsize(path)
                    files.append((date_str, idx, path, size))
        except FileNotFoundError:
            pass

        if files:

            last_date, last_idx, last_path, last_size = files[-1]
            if last_size < self.max_bytes:
                # On continue le dernier fichier existant
                self.base_date = last_date
                self.current_index = last_idx
                self.current_log_file = last_path
                self._create_log_file(self.current_log_file)
                return
            else:
                # Le fichier est plein
                today = datetime.now().strftime("%Y-%m-%d")
                if last_date == today:
                    self.base_date = today
                    self.current_index = last_idx + 1
                else:
                    self.base_date = today
                    self.current_index = 0
        else:
            # Aucun fichier trouvé
            self.base_date = datetime.now().strftime("%Y-%m-%d")
            self.current_index = 0

        self.current_log_file = self._get_log_filename(self.base_date, self.current_index)
        self._create_log_file(self.current_log_file)

    def _create_log_file(self, filepath):
        if self.current_file_handler:
            self.current_file_handler.close()

        self.current_file_handler = logging.FileHandler(filepath, mode="a", encoding="utf-8")
        self.current_file_handler.setFormatter(logging.Formatter(
            "%(asctime)s - %(message)s", datefmt="%Y-%m-%d %H:%M:%S"
        ))

        self._cleanup_old_logs()  # ← AJOUT ICI

    def emit(self, record):
        # Si fichier courant est plein
        if self.current_file_handler.stream.tell() > self.max_bytes:
            today = datetime.now().strftime("%Y-%m-%d")

            if today != self.base_date:
                # Nouveau jour → recommence à 0 avec la date du jour
                self.base_date = today
                self.current_index = 0
            else:
                # Même jour → juste incrémenter l’index
                self.current_index += 1

            self.current_log_file = self._get_log_filename(self.base_date, self.current_index)
            self._create_log_file(self.current_log_file)

        self.current_file_handler.emit(record)

    def _cleanup_old_logs(self):
        """Supprime les plus vieux fichiers de log si on dépasse le BACKUP_COUNT."""
        pattern = re.compile(r"log_(\d{4}-\d{2}-\d{2})(?:\((\d+)\))?\.txt$")
        all_logs = []

        try:
            for fname in os.listdir(self.log_folder):
                match = pattern.match(fname)
                if match:
                    path = os.path.join(self.log_folder, fname)
                    mtime = os.path.getmtime(path)
                    all_logs.append((mtime, path))
        except FileNotFoundError:
            return

        all_logs.sort()  # plus anciens en premier

        while len(all_logs) > self.backup_count:
            _, oldest_path = all_logs.pop(0)
            try:
                os.remove(oldest_path)
            except Exception as e:
                print(f"Erreur lors de la suppression de {oldest_path}: {e}")

    def close(self):
        if self.current_file_handler:
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
            # print(f"Peak temperature reached: {last_peak_temperature:.2f}°C")
            state = "after_drop"

    elif state == "after_drop":
        if temperature < TEMP_NEW_USER:
            # print("enter after drop")
            # any rising trend means we can restart
            state = "idle"
            last_peak_temperature = None

    return state, last_peak_temperature


def log_temperature_new(logger, temperature, state, last_peak_temperature=None,
                        previous_window=None, current_window=None):
    if previous_window is None:
        previous_window = []
    if current_window is None:
        current_window = []

    if state == "idle":
        print("enter idle")
        last_peak_temperature = temperature
        state = "tracking_peak"
        previous_window.clear()
        current_window.clear()

    elif state == "tracking_peak":
        if temperature > last_peak_temperature:
            last_peak_temperature = temperature
        elif temperature <= last_peak_temperature - TEMP_AFTER_PEAK:
            logger.info(f"Peak temperature reached: {last_peak_temperature:.2f}°C")
            print(f"Peak temperature reached: {last_peak_temperature:.2f}°C")
            state = "after_drop"
            previous_window.clear()
            current_window = [temperature]

    elif state == "after_drop":
        # print("enter after drop")
        current_window.append(temperature)

        if len(current_window) == WINDOWS_SIZE:
            if len(previous_window) == WINDOWS_SIZE:
                print(f"previous_window:{previous_window}")
                print(f"current_window:{current_window}")
                rise_count = sum(1 for p, c in zip(previous_window, current_window) if c > p)
                print(f"rise_count = {rise_count}")

                if rise_count >= RISE_THRESHOLD:
                    state = "idle"
                    last_peak_temperature = None
                    previous_window.clear()
                    current_window.clear()
                else:
                    previous_window = current_window.copy()
                    current_window.clear()
            else:
                # La première fois : on initialise previous_window
                previous_window = current_window.copy()
                current_window.clear()

    return state, last_peak_temperature, previous_window, current_window
