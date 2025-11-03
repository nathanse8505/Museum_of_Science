import logging
import os
from consts import MAX_SIZE_PER_LOG_FILE, LOG_FOLDER, BACKUP_COUNT
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

    # UPDATED: accepte aussi les fichiers finalisés avec _to_
    def _get_log_filename(self, date_str, index=0):
        suffix = f"({index})" if index > 0 else ""
        filename = f"log_{date_str}{suffix}.txt"
        return os.path.join(self.log_folder, filename)

    # NEW: motif qui comprend aussi la forme finalisée *_to_*
    def _compiled_pattern(self):
        # log_YYYY-MM-DD[_to_YYYY-MM-DD][(index)].txt
        return re.compile(
            r"log_(\d{4}-\d{2}-\d{2})(?:_to_(\d{4}-\d{2}-\d{2}))?(?:\((\d+)\))?\.txt$"
        )

    def _select_startup_file(self):
        """Trouve le dernier fichier 'ouvert' existant (sans _to_), continue s’il n’est pas plein. Sinon crée un nouveau fichier."""
        pattern = self._compiled_pattern()
        files = []

        try:
            for fname in sorted(os.listdir(self.log_folder)):
                match = pattern.match(fname)
                if match:
                    start_date_str = match.group(1)
                    end_date_str = match.group(2)  # présent si déjà finalisé
                    idx = int(match.group(3)) if match.group(3) else 0
                    path = os.path.join(self.log_folder, fname)
                    size = os.path.getsize(path)
                    files.append((start_date_str, end_date_str, idx, path, size))
        except FileNotFoundError:
            pass

        # Filtrer les fichiers "ouverts" (sans _to_)
        open_files = [t for t in files if t[1] is None]

        if open_files:
            last_start, _, last_idx, last_path, last_size = open_files[-1]
            if last_size < self.max_bytes:
                # Continuer le dernier fichier encore ouvert et non plein
                self.base_date = last_start
                self.current_index = last_idx
                self.current_log_file = last_path
                self._create_log_file(self.current_log_file)
                return
            else:
                # Le fichier ouvert le plus récent est plein → on repart sur un nouveau
                today = datetime.now().strftime("%Y-%m-%d")
                if last_start == today:
                    self.base_date = today
                    self.current_index = last_idx + 1
                else:
                    self.base_date = today
                    self.current_index = 0
        else:
            # Aucun fichier "ouvert" → créer un nouveau pour aujourd'hui
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

        self._cleanup_old_logs()  # ← AJOUT ICI

    def _finalize_current_file(self, end_date_str: str):
        """
        Renomme le fichier courant seulement si la date de fin est différente de la date de début.
        Formats:
          - même jour  : log_YYYY-MM-DD[(idx)].txt  (pas de _to_)
          - jours diff : log_YYYY-MM-DD_to_YYYY-MM-DD[(idx)].txt
        """
        if not self.current_file_handler:
            return

        old_path = self.current_file_handler.baseFilename
        fname = os.path.basename(old_path)

        # Si déjà finalisé, ne rien faire
        if "_to_" in fname:
            return

        # Extraire start_date et index éventuel: log_YYYY-MM-DD[(idx)].txt
        import re
        m = re.match(r"^log_(\d{4}-\d{2}-\d{2})(?:\((\d+)\))?\.txt$", fname)
        if not m:
            return  # format inattendu

        start_date_str = m.group(1)
        idx_str = m.group(2)
        idx_suffix = f"({idx_str})" if idx_str else ""

        # ⚠️ Si même jour → on NE CHANGE PAS le nom (aucun _to_)
        if end_date_str == start_date_str:
            # On ferme le handler et on le laisse avec le même nom
            self.current_file_handler.close()
            self.current_file_handler = None
            self.current_log_file = old_path
            return

        # Jours différents → on ajoute _to_
        new_name = f"log_{start_date_str}_to_{end_date_str}{idx_suffix}.txt"
        new_path = os.path.join(self.log_folder, new_name)

        try:
            self.current_file_handler.close()
            os.replace(old_path, new_path)
            self.current_log_file = new_path
        finally:
            self.current_file_handler = None

    # # NEW: finalise (renomme) le fichier courant quand il devient plein
    # def _finalize_current_file(self, end_date_str: str):
    #     """
    #     Renomme le fichier courant:
    #     log_YYYY-MM-DD[(idx)].txt -> log_YYYY-MM-DD_to_YYYY-MM-DD[(idx)].txt
    #     (Ne rien faire s'il est déjà finalisé.)
    #     """
    #     if not self.current_file_handler:
    #         return
    #     old_path = self.current_file_handler.baseFilename
    #     fname = os.path.basename(old_path)
    #
    #     # Si déjà finalisé, on ne touche pas
    #     if "_to_" in fname:
    #         return
    #
    #     # Extraire start_date et index éventuel
    #     pattern = re.compile(r"^log_(\d{4}-\d{2}-\d{2})(?:\((\d+)\))?\.txt$")
    #     m = pattern.match(fname)
    #     if not m:
    #         return  # format inattendu, on ne casse rien
    #
    #     start_date_str = m.group(1)
    #     idx_str = m.group(2)
    #     idx_suffix = f"({idx_str})" if idx_str else ""
    #
    #     new_name = f"log_{start_date_str}_to_{end_date_str}{idx_suffix}.txt"
    #     new_path = os.path.join(self.log_folder, new_name)
    #
    #     try:
    #         # Fermer le handler avant de renommer sur Windows
    #         self.current_file_handler.close()
    #         os.replace(old_path, new_path)
    #         # Mettre à jour le chemin courant (il ne sera plus utilisé pour écrire)
    #         self.current_log_file = new_path
    #     finally:
    #         # on ne ré-ouvre pas ce fichier finalisé pour l’écriture
    #         self.current_file_handler = None

    def emit(self, record):
        # Si fichier courant est plein → finaliser (renommer) et ouvrir un nouveau
        if self.current_file_handler and self.current_file_handler.stream.tell() > self.max_bytes:
            today = datetime.now().strftime("%Y-%m-%d")

            # Finaliser l'ancien fichier AVANT de choisir le nouveau
            self._finalize_current_file(today)

            # Choisir base_date / index pour le nouveau fichier
            if today != self.base_date:
                # Nouveau jour → recommence à 0 avec la date du jour
                self.base_date = today
                self.current_index = 0
            else:
                # Même jour → incrémenter l’index
                self.current_index += 1

            self.current_log_file = self._get_log_filename(self.base_date, self.current_index)
            self._create_log_file(self.current_log_file)

        # Créer un fichier si, pour une raison X, le handler a été finalisé
        if not self.current_file_handler:
            self.current_log_file = self._get_log_filename(self.base_date, self.current_index)
            self._create_log_file(self.current_log_file)

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

    # Avoid adding multiple handlers if already configured
    if not logger.handlers:
        date_handler = DateBasedFileHandler(LOG_FOLDER, MAX_SIZE_PER_LOG_FILE)
        logger.addHandler(date_handler)

    return logger


logger = get_logger()
for i in range(30000):
    logger.info("oooo")