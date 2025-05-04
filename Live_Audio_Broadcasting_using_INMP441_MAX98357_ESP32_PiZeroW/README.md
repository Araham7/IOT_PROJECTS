# Live Audio Broadcasting System using ESP32, INMP441, MAX98357A, and Node.js

## **Author**: Araham Abeddin

This project implements a `live audio broadcasting system` using ESP32 `microcontrollers`, an `INMP441 I2S microphone`, a `MAX98357A I2S amplifier`, and a `Node.js WebSocket server`. The system captures audio from a microphone, streams it over a local network via WebSocket, and plays it through a speaker in real-time. The project is designed for applications like wireless intercoms, live audio monitoring, or DIY audio streaming.

##  Project Overview
The system consists of three main components:

1. **Audio Transmitter:**
  * An ESP32 captures audio using an INMP441 I2S microphone.
  * The audio is streamed as 16-bit, 16kHz data to a WebSocket server.
  * Located in the `Audio_Transmitter` directory.

2. **Audio Receiver:**

  * An ESP32 receives audio data from the WebSocket server.
  * The audio is played through a MAX98357A I2S amplifier connected to an 8Ω speaker.
Located in the `Audio_Receiver` directory.

3. **Node.js WebSocket Server:**

  * A Node.js server running on a local machine (e.g., Raspberry Pi Zero W or a PC) facilitates communication.
  * It accepts audio from the transmitter and forwards it to one or more receivers.
  * Located in the `NodeJS_Server` directory.

## Directory Structure

```
Live_Audio_Broadcasting_using_INMP441_MAX98357_ESP32_PiZeroW/
├── All_IMAGES/                    # Images of components and setup
│   ├── 8Ohm_Speaker_05.jpeg
│   ├── all_comonents_01.jpeg
│   ├── all_components_02.jpeg
│   ├── ESP32_04.jpeg
│   └── max98357_I2S_Amplifire_03.jpeg
├── Audio_Receiver/                # ESP32 receiver code and documentation
│   ├── IMAGES/
│   ├── README.md
│   └── Receiver.ino
├── Audio_Transmitter/             # ESP32 transmitter code and documentation
│   ├── IMAGES/
│   ├── README.md
│   └── Transmitter.ino
├── NodeJS_Server/                 # Node.js WebSocket server
│   ├── package.json
│   ├── README.md
│   └── server.js
└── README.md                      # This file
```

## Components

See the Required Components for a visual overview of the hardware used.

* **ESP32** (x2, e.g., ESP32-WROOM-32): One for the transmitter, one for the receiver.
* **INMP441 I2S Microphone:** Captures audio for the transmitter.
* **MAX98357A I2S Amplifier:** Drives the speaker for the receiver.
* **8Ω Speaker** (3W, 4–8Ω compatible): Outputs audio on the receiver.
* **Power Supply** (3.3V/5V): Powers the ESP32, INMP441, and MAX98357A.
* **Jumper Wires and Breadboard:** For prototyping connections.

## Setup Instructions

1. **Prepare the Environment:**

  * Ensure a local WiFi network is available (default: SSID "your_ssid", password "your_password").
  * Note the IP address of the machine running the Node.js server (default: `192.168.239.231`).

2. **Set Up the Node.js Server:**

  * Navigate to the `NodeJS_Server` directory.
  * Follow the instructions in `NodeJS_Server/README.md` to install Node.js, the `ws` package, and run `server.js`.

3. **Configure the Audio Transmitter:**

  * Navigate to the `Audio_Transmitter` directory.
  * Follow the instructions in `Audio_Transmitter/README.md` to wire the INMP441 to the ESP32, install Arduino libraries, and upload `Transmitter.ino`.

4. **Configure the Audio Receiver:**

  * Navigate to the `Audio_Receiver` directory.
Follow the instructions in `Audio_Receiver/README.md` to wire the MAX98357A and speaker to the ESP32, install Arduino libraries, and upload `Receiver.ino`.

5. **Run the System:**

* Start the Node.js server (`node server.js` in `NodeJS_Server`).
* Power on the transmitter ESP32 to capture and stream audio.
* Power on the receiver ESP32 to play the streamed audio.
* Monitor the Serial Monitors (115200 baud) of the ESP32s and the server console for debugging.

## Usage

* **Streaming Audio:**

  * Speak into the INMP441 microphone connected to the transmitter.
  * The audio is sent to the Node.js server and forwarded to all connected receivers.
  * The receiver plays the audio through the 8Ω speaker via the MAX98357A.

## Debugging:

  * Check the server logs for connection and data transfer status.
  * Use the ESP32 Serial Monitors to verify WiFi and WebSocket connections.
  * Adjust gain settings (`GAIN_FACTOR`) in the transmitter and receiver sketches if audio is too quiet or distorted.

## Troubleshooting

* **No Audio:**
  * Verify all devices are on the same WiFi network.
  * Ensure the server IP and port (8080) are correct in the ESP32 sketches.
  * Check wiring for the INMP441, MAX98357A, and speaker.

## Connection Issues:
Confirm the Node.js server is running and port 8080 is not blocked.
Restart the server and ESP32s if connections fail.

## Distorted Audio:
Lower the GAIN_FACTOR in the transmitter (`Transmitter.ino`) or receiver (`Receiver.ino`).
Ensure a stable power supply (5V preferred for the receiver).


## Notes

* The system is configured for 16-bit, 16kHz audio, suitable for voice streaming.
* The Node.js server supports one transmitter and multiple receivers.
* For battery-powered setups, use stable 3.3V/5V sources for the ESP32s and peripherals.
The All_IMAGES directory contains reference images of the components and setup.

> **License:**
> The contents of this project and accompanying instructions are the intellectual property of Araham Abeddin. They are licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International Public License.
> This license allows you to remix, adapt, and build upon the material for non-commercial purposes, provided that appropriate credit is given to Araham Abeddin. You must not suggest that Araham Abeddin endorses you or your use of the work. Any derivative works must be distributed under the same license.
> This project is provided "as is" without any warranties of any kind. Neither Araham Abeddin nor the author assumes any liability for any loss or damage caused, or claimed to be caused, directly or indirectly by the instructions, software, or hardware described herein.
> As Araham Abeddin has no control over the use, setup, assembly, modification, or misuse of the hardware, software, or information provided in this manual, no responsibility shall be accepted for any resulting injury or damage. By copying, using, setting up, or assembling this material, the user accepts full liability for any consequences.
**Version 1.0** – Released on 4 May 2025


For issues or contributions, please contact <a href="mailto:arahamabeddin7@gmail.com" title="Contact me">Araham Abeddin</a>
