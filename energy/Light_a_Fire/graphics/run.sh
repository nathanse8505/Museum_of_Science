#!/bin/bash

# Mise à jour du système
echo "🔄 Mise à jour du système..."
sudo apt update && sudo apt upgrade -y

# Installation des paquets via APT
echo "📦 Installation des bibliothèques via APT..."
sudo apt install -y python3-pip python3-opencv python3-serial python3-numpy

# Installation ou mise à jour de pygame via pip (plus fiable)
echo "🎮 Installation de pygame via pip..."
pip3 install --upgrade pip
pip3 install --upgrade pygame

# Vérification des installations
echo "🔍 Vérification des installations..."
python3 -c "import pygame" || echo "❌ Erreur: pygame non installé"
python3 -c "import cv2" || echo "❌ Erreur: opencv (cv2) non installé"
python3 -c "import serial" || echo "❌ Erreur: pyserial non installé"
python3 -c "import numpy" || echo "❌ Erreur: numpy non installé"

# Configuration du script au démarrage
AUTOSTART_FILE="$HOME/.config/lxsession/LXDE-pi/autostart"
SCRIPT_PATH="$HOME/Desktop/light_a_fire/main.py"
AUTOSTART_CMD="@bash -c 'sleep 20 && /usr/bin/python3 $SCRIPT_PATH'"

# Création du dossier si besoin
mkdir -p "$(dirname "$AUTOSTART_FILE")"

# Ajout du script si pas déjà présent
if ! grep -Fxq "$AUTOSTART_CMD" "$AUTOSTART_FILE"; then
    echo "⚙️ Ajout de l'exécution automatique de main.py au démarrage..."
    echo "$AUTOSTART_CMD" >> "$AUTOSTART_FILE"
else
    echo "✅ Le script est déjà configuré pour démarrer automatiquement."
fi

echo "🎉 Configuration terminée avec succès."
