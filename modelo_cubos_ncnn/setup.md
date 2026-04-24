# Setup Guide for YOLO NCNN on Raspberry Pi (Ubuntu)

This guide explains how to set up the environment to run the `test_ncnn.py` script on a Raspberry Pi using Ubuntu.

## 1. Install System Dependencies
Before creating the virtual environment, ensure you have the necessary system packages for Python development and OpenCV:

```bash
sudo apt update
sudo apt install -y python3-venv python3-pip libgl1-mesa-glx libglib2.0-0
```

## 2. Create a Virtual Environment
Ubuntu on Raspberry Pi often restricts global `pip` installs (PEP 668). We use a virtual environment to manage dependencies safely:

```bash
# Navigate to the project directory
cd ~/Downloads/modelo_cubos_ncnn

# Create the virtual environment
python3 -m venv .venv

# Activate the virtual environment
source .venv/bin/activate
```

## 3. Install Python Packages
Once the environment is active, install the required libraries:

```bash
pip install --upgrade pip
pip install ultralytics ncnn opencv-python
```

## 4. Running the Script
Always ensure the virtual environment is active before running the script:

```bash
# Activate (if not already active)
source .venv/bin/activate

# Run the test script
python3 test_ncnn.py
```

## Troubleshooting
- **Camera Access:** If the webcam doesn't open, ensure your user has permission to access video devices: `sudo usermod -aG video $USER` (re-login required).
- **Performance:** NCNN is optimized for ARM CPUs like the one in the Raspberry Pi, but performance will vary depending on the model size and Pi version.
