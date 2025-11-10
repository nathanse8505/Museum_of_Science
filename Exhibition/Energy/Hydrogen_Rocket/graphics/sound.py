import pygame
from tkinter import Tk, filedialog
import os
from gtts import gTTS

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

def text_to_speech():
    # Correct language codes and phrases
    phrase_translations = {
        "en": "Look up at the bottle: 3, 2, 1, launch!",
        "ar": "انظروا إلى الأعلى نحو الزجاجة، ثلاثة، اثنان، واحد... انطلاق!"
    }

    # Output file paths
    file_paths = {}
    for lang_code, text in phrase_translations.items():
        tts = gTTS(text=text, lang=lang_code)
        path = os.path.join(os.path.dirname(__file__), "music")
        file_path = os.path.join(path, f"{lang_code}_launch.mp3")
        tts.save(file_path)
        file_paths[lang_code] = file_path


if __name__ == "__main__":
    pass
    # file_path = select_file()
    # if file_path:
    #     play_audio(file_path)
    # else:
    #     print("Aucun fichier sélectionné.")
