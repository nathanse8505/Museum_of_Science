"""
Filename: logs.py
Purpose: Logging functions for the Hydrogen Rocket UI
"""
import logging
import os
from consts import MAX_SIZE_PER_LOG_FILE, BACKUP_COUNT, LOG_FOLDER
import re
from datetime import datetime


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
