#!/bin/bash
# -------------------------------------------------------
# Unified Supervisor: Python + Camera Loop (switchable)
# -------------------------------------------------------

# Audio par défaut
pactl set-default-sink alsa_output.pci-0000_00_1f.3-platform-skl_hda_dsp_generic.HiFi__hw_sofhdadsp__sink || true
pactl set-default-source alsa_input.pci-0000_00_1f.3.analog-stereo || true

APP_DIR="/home/mada/Documents/break_glass/code_python/V2"
PYTHON_BIN="/usr/bin/python3"
LOG_FILE="$APP_DIR/sounddemo.log"
SLEEP_SECONDS=5
QUIT_FILE="$APP_DIR/quit.flag"

STATE_FILE="$APP_DIR/current_cam.txt"
CAM1="/dev/video0"
CAM2="/dev/video2"

echo "============================================" >> "$LOG_FILE"
echo "RUN.SH STARTED ON: $(date)" >> "$LOG_FILE"
echo "============================================" >> "$LOG_FILE"

cd "$APP_DIR" || exit 1


# -------------------------------------------------------
# CAMERA SUPERVISOR (intégré ici !)
# -------------------------------------------------------
camera_loop() {
    while true; do
        
        # si Python éteint tout
        [ -f "$QUIT_FILE" ] && exit 0

        # caméra par défaut si fichier inexistant
        if [ ! -f "$STATE_FILE" ]; then
            echo "CAM1" > "$STATE_FILE"
        fi

        CURRENT=$(cat "$STATE_FILE")

        if [ "$CURRENT" = "CAM2" ]; then
            DEV="$CAM2"
        else
            DEV="$CAM1"
        fi

        # config fluidité
        v4l2-ctl --device="$DEV" \
            --set-fmt-video=width=640,height=480,pixelformat=MJPG \
            --set-parm=30 2>/dev/null || true

        echo "[$(date)] Starting camera $DEV" >> "$LOG_FILE"

        # lancer caméra
        guvcview --device="$DEV" --gui=none

        # si la fenêtre se ferme, on relance
        sleep 1
    done
}

# Lancer la caméra en arrière-plan
camera_loop &
CAM_LOOP_PID=$!


# -------------------------------------------------------
# PYTHON SUPERVISOR (comme avant)
# -------------------------------------------------------
while true; do
    if [ -f "$QUIT_FILE" ]; then
        echo "[$(date)] Quit flag detected. Stopping everything." >> "$LOG_FILE"
        rm -f "$QUIT_FILE"
        break
    fi

    echo "[$(date)] Launching init.py ..." >> "$LOG_FILE"
    $PYTHON_BIN init.py >> "$LOG_FILE" 2>&1
    EXIT_CODE=$?

    if [ -f "$QUIT_FILE" ]; then
        echo "[$(date)] User pressed Q — exiting." >> "$LOG_FILE"
        rm -f "$QUIT_FILE"
        break
    fi

    echo "[$(date)] init.py exited ($EXIT_CODE). Restarting in $SLEEP_SECONDS sec" >> "$LOG_FILE"
    sleep $SLEEP_SECONDS
done


# -------------------------------------------------------
# SHUTDOWN
# -------------------------------------------------------
kill "$CAM_LOOP_PID" 2>/dev/null || true
killall guvcview 2>/dev/null || true

echo "[$(date)] run.sh terminated cleanly." >> "$LOG_FILE"
exit 0
