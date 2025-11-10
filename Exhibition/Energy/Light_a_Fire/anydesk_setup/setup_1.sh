#!/bin/bash

echo "=== 🖥️ Informations système ==="
lsb_release -a
uname -mrs
cat /etc/debian_version
getconf LONG_BIT

echo "=== 🔍 Vérification du service AnyDesk ==="
sudo systemctl status anydesk

echo "=== 📦 Installation des bibliothèques requises ==="
sudo apt install -y libgles-dev libegl-dev

echo "=== 📂 Vérification des bibliothèques OpenGL ==="
ls /usr/lib/arm-linux-gnueabihf/libGLE*
ls /usr/lib/arm-linux-gnueabihf/libEGL*

echo "=== 🔗 Création des liens symboliques ==="
sudo ln -sf /usr/lib/arm-linux-gnueabihf/libGLESv2.so /usr/lib/libbrcmGLESv2.so
sudo ln -sf /usr/lib/arm-linux-gnueabihf/libEGL.so /usr/lib/libbrcmEGL.so

echo "♻️ Le système va redémarrer dans 5 secondes..."
sleep 5
sudo reboot
