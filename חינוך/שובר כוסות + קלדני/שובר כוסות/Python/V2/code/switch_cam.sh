#!/bin/bash
APP_DIR="/home/mada/Documents/break_glass/code_python/V2"
STATE_FILE="$APP_DIR/current_cam.txt"

CURRENT=$(cat "$STATE_FILE" 2>/dev/null || echo "CAM1")

if [ "$CURRENT" = "CAM1" ]; then
    echo "CAM2" > "$STATE_FILE"
else
    echo "CAM1" > "$STATE_FILE"
fi

# ferme guvcview pour que run.sh relance la bonne caméra
killall guvcview 2>/dev/null || true
