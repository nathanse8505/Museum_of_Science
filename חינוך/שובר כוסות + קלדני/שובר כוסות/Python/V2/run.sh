#!/bin/bash
# -------------------------------------------------------
# Sound Demonstration Auto-Start Script (Loop + Quit)
# Original project: Windows XP / Python 2.7
# Modernized for Linux Mint / Python 3.10
# Author: Dima Polonsky
# -------------------------------------------------------
# -------------------------------------------------------
# Ensure correct audio output (headphones/internal) and mic
pactl set-default-sink alsa_output.pci-0000_00_1f.3-platform-skl_hda_dsp_generic.HiFi__hw_sofhdadsp__sink || true
pactl set-default-source alsa_input.pci-0000_00_1f.3.analog-stereo || true

APP_DIR="/home/mada/Documents/break_glass/code_python/V2"
PYTHON_BIN="/usr/bin/python3"          # or ~/.venvs/sounddemo/bin/python
LOG_FILE="$APP_DIR/sounddemo.log"
SLEEP_SECONDS=5                        # Delay before restart if crash occurs
QUIT_FILE="$APP_DIR/quit.flag"         # File created when "q" is pressed

echo "============================================" >> "$LOG_FILE"
echo "Sound Demonstration Autostart (XP → Linux)" >> "$LOG_FILE"
echo "Started on: $(date)" >> "$LOG_FILE"
echo "============================================" >> "$LOG_FILE"

cd "$APP_DIR" || {
    echo "❌ Failed to change directory to $APP_DIR" | tee -a "$LOG_FILE"
    exit 1
}

# Optional: activate virtual environment
# source ~/.venvs/sounddemo/bin/activate

# --- MAIN LOOP ---
while true; do
    if [ -f "$QUIT_FILE" ]; then
        echo "[$(date)] Quit flag detected, exiting loop." >> "$LOG_FILE"
        rm -f "$QUIT_FILE"
        break
    fi

    echo "[$(date)] Launching init.py ..." | tee -a "$LOG_FILE"
    $PYTHON_BIN init.py >> "$LOG_FILE" 2>&1
    EXIT_CODE=$?

    # Check if user pressed "q"
    if [ -f "$QUIT_FILE" ]; then
        echo "[$(date)] User pressed Q — exiting program." >> "$LOG_FILE"
        rm -f "$QUIT_FILE"
        break
    fi

    echo "[$(date)] Program exited (code $EXIT_CODE), restarting in $SLEEP_SECONDS sec..." >> "$LOG_FILE"
    sleep $SLEEP_SECONDS
done

echo "[$(date)] run.sh terminated cleanly." >> "$LOG_FILE"
exit 0

