#!/bin/bash
# -------------------------------------------------------
# Sound Demonstration Auto-Start Script
# Originally developed for Windows XP (Python 2.7),
# modernized for Linux Mint / Python 3.10.
# -------------------------------------------------------

# --- SETTINGS ---
APP_DIR="/home/mada/Desktop/שוברי כוסות/soundDemonstration-master/soundDemo/soundDemo/sound"
PYTHON_BIN="/usr/bin/python3"        # or ~/.venvs/sounddemo/bin/python
LOG_FILE="$APP_DIR/sounddemo.log"

# --- STARTUP ---
echo "----------------------------------------" >> "$LOG_FILE"
echo "Starting Sound Demonstration: $(date)" >> "$LOG_FILE"

cd "$APP_DIR" || {
    echo "❌ Failed to change directory to $APP_DIR" | tee -a "$LOG_FILE"
    exit 1
}

# optional: activate virtual environment
# source ~/.venvs/sounddemo/bin/activate

# Run the main program
$PYTHON_BIN init.py >> "$LOG_FILE" 2>&1

EXIT_CODE=$?
echo "Program exited with code $EXIT_CODE at $(date)" >> "$LOG_FILE"
echo "----------------------------------------" >> "$LOG_FILE"

exit $EXIT_CODE

