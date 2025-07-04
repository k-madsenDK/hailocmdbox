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

1. **Install dependencies**  
   - Qt (for hailocmdbox)
   - Python 3 (for madsen.py)
   - Required libraries for Pico firmware (see [Hackster project](https://www.hackster.io/kim-madsen/hailo-rpi5-and-pico-as-io-f80990) for details)

2. **Flash the Pico**  
   - Upload the provided firmware to the Pico.
   - Connect Pico to Pi 5 via USB.

3. **Connect Pan-Tilt HAT**  
   - Attach Pan-Tilt HAT to the Pico and connect servos.

4. **Run madsen.py and hailocmdbox**  
   - Start `madsen.py` to parse Hailo AI output.
   - Launch `hailocmdbox` Qt application.

5. **Configure settings**  
   - Set the serial port and label-to-track in the Qt app's settings if required.

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
