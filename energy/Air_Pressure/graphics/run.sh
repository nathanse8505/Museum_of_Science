#!/bin/bash

# Ensure the script is executable
chmod +x /home/mada/Air_pressure_py/run.sh

# Define the virtual environment directory
VENV_DIR="Air_Pressure_py_venv"

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

# Check if the virtual environment exists, if not, create it
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

# Check and install required packages
check_and_install pygame
check_and_install pyserial
check_and_install numpy  

# Run the main.py script using the virtual environment's Python interpreter
echo "Running main.py..."
$VENV_DIR/bin/python /home/mada/Air_Pressure_py/main.py

# Deactivate the virtual environment after running the script
deactivate
