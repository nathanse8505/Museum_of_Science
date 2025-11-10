"""
Filename: logs.py
Purpose: Logging functions for the Hydrogen Rocket UI
"""

import logging
import os
import re
from datetime import datetime
from consts import MAX_SIZE_PER_LOG_FILE, LOG_FOLDER, BACKUP_COUNT


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

    def _compiled_pattern(self):
        """
        Nouveau style uniquement :
          - log_YYYY-MM-DD.txt
          - log_YYYY-MM-DD(2).txt
          - log_YYYY-MM-DD_to_YYYY-MM-DD.txt
          - log_YYYY-MM-DD(2)_to_YYYY-MM-DD.txt  (index AVANT _to_)
        """
        return re.compile(
            r"^log_(\d{4}-\d{2}-\d{2})(?:\((\d+)\))?(?:_to_(\d{4}-\d{2}-\d{2}))?\.txt$"
        )

    def _select_startup_file(self):
        """Trouve le dernier fichier 'ouvert' existant (sans _to_), continue s’il n’est pas plein. Sinon crée un nouveau fichier."""
        pattern = self._compiled_pattern()
        files = []

        try:
            for fname in sorted(os.listdir(self.log_folder)):
                match = pattern.match(fname)
                if match:
                    start_date_str = match.group(1)   # date de début
                    idx_str = match.group(2)          # index éventuel (ex: "2")
                    end_date_str = match.group(3)     # date de fin (_to_), None si "ouvert"
                    idx = int(idx_str) if idx_str else 0
                    path = os.path.join(self.log_folder, fname)
                    size = os.path.getsize(path)
                    files.append((start_date_str, end_date_str, idx, path, size))
        except FileNotFoundError:
            pass

        # Fichiers "ouverts" (sans _to_)
        open_files = [t for t in files if t[1] is None]

        # ✅ Choisir le dernier ouvert via tri par (start_date, index)
        open_files.sort(key=lambda t: (t[0], t[2]))

        if open_files:
            last_start, _, last_idx, last_path, last_size = open_files[-1]
            if last_size < self.max_bytes:
                # Continuer le dernier fichier non plein
                self.base_date = last_start
                self.current_index = last_idx
                self.current_log_file = last_path
                self._create_log_file(self.current_log_file)
                return
            else:
                # Le dernier ouvert est plein → créer un nouveau
                today = datetime.now().strftime("%Y-%m-%d")
                if last_start == today:
                    self.base_date = today
                    self.current_index = last_idx + 1
                else:
                    self.base_date = today
                    self.current_index = 0
        else:
            # Aucun fichier ouvert → créer un nouveau pour aujourd'hui
            self.base_date = datetime.now().strftime("%Y-%m-%d")
            self.current_index = 0

        self.current_log_file = self._get_log_filename(self.base_date, self.current_index)
        self._create_log_file(self.current_log_file)

    def _create_log_file(self, filepath):
        if self.current_file_handler:
            self.current_file_handler.close()

        os.makedirs(self.log_folder, exist_ok=True)
        self.current_file_handler = logging.FileHandler(filepath, mode="a", encoding="utf-8")
        self.current_file_handler.setFormatter(logging.Formatter(
            "%(asctime)s - %(message)s", datefmt="%Y-%m-%d %H:%M:%S"
        ))

        self._cleanup_old_logs()

    ################### V2 #######################
    def _finalize_current_file(self, end_date_str: str):
        """
        Renomme le fichier courant si la date de fin est différente de la date de début.
        Formats :
          - même jour  : log_YYYY-MM-DD[(idx)].txt                         (pas de _to_)
          - jours diff : log_YYYY-MM-DD[(idx)]_to_YYYY-MM-DD.txt           (index AVANT _to_)
        """
        if not self.current_file_handler:
            return

        old_path = self.current_file_handler.baseFilename
        fname = os.path.basename(old_path)

        # Si déjà finalisé, ne rien faire
        if "_to_" in fname:
            return

        m = re.match(r"^log_(\d{4}-\d{2}-\d{2})(?:\((\d+)\))?\.txt$", fname)
        if not m:
            return  # format inattendu

        start_date_str = m.group(1)
        idx_str = m.group(2)
        idx_suffix = f"({idx_str})" if idx_str else ""

        # Même jour → on NE CHANGE PAS le nom (pas de _to_)
        if end_date_str == start_date_str:
            self.current_file_handler.close()
            self.current_file_handler = None
            self.current_log_file = old_path
            return

        # Jours différents → index AVANT _to_
        new_name = f"log_{start_date_str}{idx_suffix}_to_{end_date_str}.txt"
        new_path = os.path.join(self.log_folder, new_name)

        try:
            self.current_file_handler.close()
            os.replace(old_path, new_path)
            self.current_log_file = new_path
        finally:
            self.current_file_handler = None

    def emit(self, record):
        # ✅ Vérification robuste de la taille réelle (>=)
        if self.current_file_handler:
            try:
                current_size = os.path.getsize(self.current_file_handler.baseFilename)
            except FileNotFoundError:
                current_size = 0

            if current_size >= self.max_bytes:
                today = datetime.now().strftime("%Y-%m-%d")

                # Finaliser l'ancien fichier AVANT d'ouvrir le nouveau
                self._finalize_current_file(today)

                # Nouveau segment : même jour → index+1 ; autre jour → index=0
                if today != self.base_date:
                    self.base_date = today
                    self.current_index = 0
                else:
                    self.current_index += 1

                self.current_log_file = self._get_log_filename(self.base_date, self.current_index)
                self._create_log_file(self.current_log_file)

        # Créer un fichier si, pour une raison X, le handler a été finalisé
        if not self.current_file_handler:
            self.current_log_file = self._get_log_filename(self.base_date, self.current_index)
            self._create_log_file(self.current_log_file)

        # Écriture réelle
        self.current_file_handler.emit(record)

    def _cleanup_old_logs(self):
        """Supprime les plus vieux fichiers de log si on dépasse le BACKUP_COUNT."""
        pattern = self._compiled_pattern()
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

    # Éviter multiples handlers si déjà configuré
    if not logger.handlers:
        date_handler = DateBasedFileHandler(LOG_FOLDER, MAX_SIZE_PER_LOG_FILE)
        logger.addHandler(date_handler)

    return logger


