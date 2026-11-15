# OpenCVExperiments

These are experiments that I wanted to do in C++, after prototyping them in Python. I wanted to see how much faster the C++ versions would be.

# Hardware
Raspberry Pi 4 model B, with a Raspberry Pi Camera Module 2.

Debian version  13 (trixie)

# Dependencies
sudo apt install librpicam-app-dev

sudo apt install -y build-essential cmake git libcamera-dev libopencv-dev

Build and install LCCV: https://github.com/kbarni/LCCV.git


# Testing
Detect cameras: rpicam-hello --list-cameras

Simple test: rpicam-hello -t 0

Take a sample still image: rpicam-still -o test.jpg

