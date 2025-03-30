import pygame
from tkinter import Tk, filedialog


def select_file():
    """Ouvre une boîte de dialogue pour sélectionner un fichier MP3 ou MP4."""
    root = Tk()
    root.withdraw()  # Masquer la fenêtre principale de Tkinter
    file_path = filedialog.askopenfilename(filetypes=[("Audio Files", "*.mp3;*.mp4")])
    return file_path


def play_audio(file_path):
    """Lit un fichier audio avec pygame."""
    pygame.mixer.init()
    pygame.mixer.music.load(file_path)
    pygame.mixer.music.play()

def stop_audio():
    #print("Lecture en cours... Appuyez sur Entrée pour arrêter.")
    #input()
    pygame.mixer.music.stop()


if __name__ == "__main__":
    file_path = select_file()
    if file_path:
        play_audio(file_path)
    else:
        print("Aucun fichier sélectionné.")
