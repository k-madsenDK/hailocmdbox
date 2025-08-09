# HailoCMDBOX

A simple Qt application to control a pan-and-tilt camera using a Raspberry Pi 5, Hailo+ HAT, Raspberry Pi Pico, and Pimoroni Pan-Tilt HAT.

## Overview

This project allows you to automatically track objects detected by a Hailo AI chip. The software reads detection results from the Hailo device (via `madsen.py`) and, when a detection occurs, sends bounding box coordinates to a Raspberry Pi Pico over USB serial. The Pico then drives a Pimoroni Pan-Tilt HAT to keep the detected object centered in the camera's view.

By default, the program auto-tracks the detected "label" with the highest confidence score. The default label for tracking is "person".

![Screenshot](qtboxscreendump.png)

---

## Hardware Setup

- **Raspberry Pi 5** with Hailo+ HAT
- **Raspberry Pi Pico** (for servo control)
- **Pimoroni Pan-Tilt HAT** (attached to Pico, for pan/tilt servos)
- **USB Serial Connection** between Pi 5 and Pico

### Wiring

- Pan-Tilt HAT connects directly to the Pico.
- Pico connects to Pi 5 via USB for serial communication.

---

## Software Components

- **hailocmdbox**: Qt application for camera control and object tracking
- **madsen.py**: Reads object detection results from Hailo chain and outputs detection data
- **Pico firmware**: Receives coordinates over serial and drives servos

---

## Settings & Usage

### 1. Hailo Detection Output

- The Hailo AI pipeline outputs object detection results as text lines (label, confidence, bounding box coordinates).
- `madsen.py` parses this output and relays the highest-confidence detection (by default, "person") to the main Qt application.

### 2. Serial Communication

- The Qt app sends serial messages to the Pico with bounding box coordinates for the detected object.
- The Pico adjusts pan/tilt servos to center the object.

### 3. Default Behavior

- The Qt app tracks the object label with the highest confidence.
- Default label: `"person"`
- Custom labels can be specified in the app settings.

---

## Example Settings (from Hackster.io)

Here are typical serial settings and parameters (as used in the Hackster.io guide):

- **Serial Baudrate:** 115200
- **Serial port (on Pi):** `/dev/ttyACM0` (may differ, check with `ls /dev/tty*`)
- **Default label to track:** `"person"`
- **Servo pulse min/max:** 500 / 2500 (microseconds, for typical SG90 servos)

**Note:** If you use different servos or change the setup, adjust minimum and maximum pulse values in the Pico firmware.

---
## Getting Started

1. **Install Dependencies**
   - Install Qt5 and CMake (needed to build and run `hailocmdbox`).
   - Clone and set up the [hailo-rpi5-examples](https://github.com/hailo-ai/hailo-rpi5-examples) repository.  
     - Place `madsen.py` inside the `hailo-rpi5-examples/basic_pipelines` directory.
   - For the Pico firmware, make sure you have the required libraries installed.  
     - See the [Hackster project guide](https://www.hackster.io/kim-madsen/hailo-rpi5-and-pico-as-io-f80990) for details on library setup.

2. **Flash the Raspberry Pi Pico**
   - I have added hailocamtilpan.ino to the files it is my latest version.
   - Upload the provided firmware to your Raspberry Pi Pico using your preferred method (e.g., drag-and-drop UF2, Thonny, etc.).
   - Connect the Pico to your Raspberry Pi 5 via USB.

4. **Connect the Pan-Tilt HAT**
   - Attach the Pimoroni Pan-Tilt HAT to the Pico.
   - Connect the pan and tilt servos to the HAT according to the manufacturer's instructions.

5. **Run the Software**
   - put starthailo.sh in hailo-rpi5-examples
   - ./starthailo.sh
   - to stop the chain shutdown Hailo Detection App

6. **Configure Application Settings**
   - In the Qt application's settings, select the correct serial port for your Pico (e.g., `/dev/ttyACM0`).
   - Set the object label you want the system to track (default is `"person"`).
   - Adjust any other settings as needed for your environment.

7.   this is only working until relaese  april-2025 sry:-(
- madsen2.py will work with the new release but you have to change the directory path to the new one end some recources is not there any more. 
---

## Troubleshooting

- **Serial port not found:**  
  Check the Pico appears as a serial device (`/dev/ttyACM0` or similar).
- **Servos not moving:**  
  Re-check wiring, power supply, and firmware settings.
- **No detection:**  
  Ensure Hailo AI pipeline is running and outputting detections.

---

## References

- [Hackster Project: Hailo, RPi5 and Pico as IO](https://www.hackster.io/kim-madsen/hailo-rpi5-and-pico-as-io-f80990)
- [Pimoroni Pan-Tilt HAT Documentation](https://learn.pimoroni.com/tutorial/sandyj/getting-started-with-pantilt-hat)
- [Raspberry Pi Pico Documentation](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html)

---
