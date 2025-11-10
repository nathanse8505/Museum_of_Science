#!/bin/bash

# Define paths
PROJECT_DIR="/home/mada/Hydrogen_Rocket_py"
VENV_DIR="$PROJECT_DIR/Hydrogen_Rocket_venv"
MAIN_SCRIPT="$PROJECT_DIR/main.py"

# Function to check if a Python package is installed in the virtual environment
check_and_install() {
    PACKAGE=$1
    if $VENV_DIR/bin/python -c "import $PACKAGE" &> /dev/null; then
        echo "$PACKAGE is already installed."
    else
        echo "$PACKAGE is not installed. Installing..."
        $VENV_DIR/bin/pip install $PACKAGE
    fi
}

# Check if virtual environment exists, if not, create it
if [ ! -d "$VENV_DIR" ]; then
    echo "Virtual environment not found in $PROJECT_DIR. Creating a new one..."
    python3 -m venv "$VENV_DIR"
    echo "Virtual environment created at $VENV_DIR."
else
    echo "Virtual environment found in $PROJECT_DIR."
fi

# Activate the virtual environment
source "$VENV_DIR/bin/activate"

# Upgrade pip in the virtual environment
echo "Upgrading pip..."
pip install --upgrade pip

# Check and install required packages
check_and_install pygame
check_and_install pyserial

# Run the main script
echo "Running $MAIN_SCRIPT..."
python "$MAIN_SCRIPT"

# Deactivate the virtual environment after running the script
deactivate

