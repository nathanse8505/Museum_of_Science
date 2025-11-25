# Installation and Usage Guide (Switch Camera + run.sh)

This README explains all the steps required to make the full system work:
- Python + PyAudio  
- The `SwitchCamera.desktop` launcher  
- File permissions (`chmod`)  
- The unified `run.sh` with automatic camera supervision  
- Behavior when quitting (X)  

---

## 1. Install PyAudio

```bash
sudo apt update
sudo apt install python3-pyaudio
```

If errors appear:

```bash
sudo apt install portaudio19-dev
pip install pyaudio
```

---

## 2. Create the `SwitchCamera.desktop` launcher

```bash
nano ~/Desktop/SwitchCamera.desktop
```

Paste:

```ini
[Desktop Entry]
Type=Application
Name=Switch Camera
Exec=/home/mada/Documents/break_glass/code_python/V2/switch_cam.sh
Terminal=false
Icon=video-camera
Comment=Switch between CAM1 and CAM2 instantly
Categories=Utility;
```

Save with **Ctrl+O**, **Enter**, **Ctrl+X**.

---

## 3. Make scripts executable

```bash
chmod +x ~/Desktop/SwitchCamera.desktop
chmod +x /home/mada/Documents/break_glass/code_python/V2/run.sh
chmod +x /home/mada/Documents/break_glass/code_python/V2/switch_cam.sh
```

---

## 4. Allow launching from Desktop

Right-click → **Allow Launching**

---

## 5. How the system works

### Python (`init.py`)
- Supervised by `run.sh`  
- Restarts automatically on close/crash  

### Cameras
- Only one active at a time  
- Closing window (X) → **camera restarts automatically**  
- Switching via `SwitchCamera.desktop` updates `current_cam.txt` and reopens the other camera  

### Quit
- Pressing `q` (quit.flag) stops **everything** cleanly  
- Next start resumes normally  

---

## 6. Summary

1. Install PyAudio  
2. Create Desktop launcher  
3. Apply chmod  
4. Launch run.sh  
5. Use Switch Camera to toggle  
6. Closing camera reopens it  
7. Closing Python restarts it  
8. Pressing q stops all  
