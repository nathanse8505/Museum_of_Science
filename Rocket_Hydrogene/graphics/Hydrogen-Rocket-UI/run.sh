#!/bin/bash

# Define the virtual environment directory
VENV_DIR="hydrogen_rocket_venv"

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
    echo "Virtual environment not found. Creating a new one..."
    python3 -m venv $VENV_DIR
    echo "Virtual environment created."
else
    echo "Virtual environment found."
fi

# Activate the virtual environment
source $VENV_DIR/bin/activate

# Upgrade pip in the virtual environment
echo "Upgrading pip..."
$VENV_DIR/bin/pip install --upgrade pip

# Check and install pygame if necessary
check_and_install pygame

# Check and install pyserial if necessary
check_and_install pyserial

# Run the main.py script using the virtual environment's Python interpreter
echo "Running main.py..."
$VENV_DIR/bin/python /home/mada/Hydrogen_Rocket_Python-main/main.py

# Deactivate the virtual environment after running the script
deactivate

