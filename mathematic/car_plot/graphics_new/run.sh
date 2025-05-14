#!/bin/bash

# === S'assure que le script est exécutable ===
chmod +x "$0"

# === Chemins ===
PROJECT_DIR="$HOME/Desktop/graphics_new"
MAIN_SCRIPT="$PROJECT_DIR/main_serial.py"

echo "📁 Projet : $PROJECT_DIR"

# === Vérifie et installe les modules Python (via apt) ===
install_if_missing() {
    MODULE_NAME=$1
    APT_PACKAGE=$2

    if python3 -c "import $MODULE_NAME" &> /dev/null; then
        echo "✅ $MODULE_NAME has been installed."
    else
        echo "📦 Installation de $APT_PACKAGE pour $MODULE_NAME..."
        sudo apt install -y "$APT_PACKAGE"
    fi
}

install_if_missing pygame python3-pygame
install_if_missing serial python3-serial
install_if_missing numpy python3-numpy

# === Exécution du script principal ===
if [ -f "$MAIN_SCRIPT" ]; then
    echo "🚀 Lancement de $MAIN_SCRIPT..."
    python3 "$MAIN_SCRIPT"
else
    echo "❌ Le fichier $MAIN_SCRIPT est introuvable."
fi

# === Ajout au démarrage automatique via fichier .desktop ===
DESKTOP_ENTRY_DIR="$HOME/.config/autostart"
DESKTOP_ENTRY_FILE="$DESKTOP_ENTRY_DIR/run_car_plot.desktop"

echo "🖥️ Vérification du fichier .desktop de démarrage..."

mkdir -p "$DESKTOP_ENTRY_DIR"

if [ -f "$DESKTOP_ENTRY_FILE" ]; then
    echo "✅ Fichier .desktop déjà présent : $DESKTOP_ENTRY_FILE"
else
    echo "🛠️ Création du fichier .desktop..."
    cat > "$DESKTOP_ENTRY_FILE" <<EOL
[Desktop Entry]
Type=Application
Name=Run Car Plot Script
Exec=lxterminal -e "$PROJECT_DIR/run.sh"
X-GNOME-Autostart-enabled=true
EOL
    echo "✅ Fichier .desktop créé : $DESKTOP_ENTRY_FILE"
fi

echo "🎉 Script terminé avec succès."
