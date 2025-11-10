#!/bin/bash

# === Chemins ===
PROJECT_DIR="$HOME/Desktop/light_a_fire/New"
MAIN_SCRIPT="$PROJECT_DIR/main.py"
AUTOSTART_FILE="$HOME/.config/lxsession/LXDE-pi/autostart"
AUTOSTART_CMD="@bash -c 'sleep 20 && $PROJECT_DIR/run.sh &'"

echo "📁 Projet : $PROJECT_DIR"

# === Vérifie et installe les modules Python (via apt) ===
install_if_missing() {
    MODULE_NAME=$1
    APT_PACKAGE=$2

    if python3 -c "import $MODULE_NAME" &> /dev/null; then
        echo "✅ $MODULE_NAME est déjà installé."
    else
        echo "📦 Installation de $APT_PACKAGE pour $MODULE_NAME..."
        sudo apt install -y "$APT_PACKAGE"
    fi
}

install_if_missing pygame python3-pygame
install_if_missing serial python3-serial
install_if_missing numpy python3-numpy
install_if_missing cv2 python3-opencv

# === Exécution du script principal ===
if [ -f "$MAIN_SCRIPT" ]; then
    echo "🚀 Lancement de $MAIN_SCRIPT..."
    python3 "$MAIN_SCRIPT"
else
    echo "❌ Le fichier $MAIN_SCRIPT est introuvable."
fi

# === Ajout au démarrage automatique si absent ===
#echo "⚙️ Vérification de l'autostart..."
#mkdir -p "$(dirname "$AUTOSTART_FILE")"

#if grep -Fxq "$AUTOSTART_CMD" "$AUTOSTART_FILE"; then
#    echo "✅ Ligne déjà présente dans autostart."
#else
#    echo "$AUTOSTART_CMD" >> "$AUTOSTART_FILE"
#    echo "✅ Ajouté à autostart : $AUTOSTART_CMD"
#fi

echo "🎉 Script terminé avec succès."
