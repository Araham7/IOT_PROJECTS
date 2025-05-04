# ESP32 Audio Receiver with MAX98357A
## Author: Araham Abeddin

This project sets up an ESP32 as an audio receiver that streams 16-bit, 16kHz audio over a WebSocket connection and outputs it to a speaker via a MAX98357A I2S amplifier. The ESP32 connects to a WiFi network, establishes a WebSocket connection to a server, and uses I2S to send amplified audio to the MAX98357A, which drives an 8Ω speaker.

## <a href="./IMAGES/" title="View the list of required components">Required Components</a>

* ESP32 (e.g., ESP32-WROOM-32)
* MAX98357A I2S amplifier module
* 8Ω Speaker (3W, 4–8Ω compatible)
* Power Supply (3.3V or 5V for MAX98357A, 5V recommended)
* Jumper Wires
* Breadboard (optional)

## Receiver Code

The following Arduino sketch configures the `ESP32` to `receive audio data via WebSocket` and `output it through the MAX98357A amplifier`.

``` cpp
/*
The contents of this code and accompanying instructions are the intellectual property of Araham Abeddin. 
They are licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International Public License. 

This license allows you to remix, adapt, and build upon the material for non-commercial purposes, 
provided that appropriate credit is given to Araham Abeddin. You must not suggest that Araham Abeddin endorses you or your use of the work. 
Any derivative works must be distributed under the same license.

This code and documentation are provided "as is" without any warranties of any kind. 
Neither Araham Abeddin nor the author assumes any liability for any loss or damage caused, 
or claimed to be caused, directly or indirectly by the instructions, software, or hardware described herein.

As Araham Abeddin has no control over the use, setup, assembly, modification, or misuse of the hardware, 
software, or information provided in this manual, no responsibility shall be accepted for any resulting injury or damage. 
By copying, using, setting up, or assembling this material, the user accepts full liability for any consequences.

Version 1.0 – Released on 4 May 2025
*/

// Receiver Code
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <driver/i2s.h>

const char* ssid = "Laptop"; // Your_hotspot_ssid
const char* password = "891*****22"; // Your_hotspot_password
const char* ws_host = "192.168.239.231"; // WebSocket_server_IP
const uint16_t ws_port = 8080; // Port_at_which_Your_server_is_running
const char* ws_path = "/";

#define I2S_WS 26    // LRCK
#define I2S_DOUT 27  // DOUT
#define I2S_SCK 25   // BCLK
#define I2S_PORT I2S_NUM_0
#define SAMPLE_RATE 16000
#define BUFFER_SIZE 512
#define GAIN_FACTOR 8.0 // Adjust this (1.0 = no gain, 4.0 = 4x amplification)

WebSocketsClient webSocket;

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

  // Initialize WebSocket
  webSocket.begin(ws_host, ws_port, ws_path);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);

  // I2S configuration
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 16, // Increased for smoother output
    .dma_buf_len = 64,
    .use_apll = true
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_DOUT,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
}

void loop() {
  webSocket.loop();
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket Disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket Connected");
      break;
    case WStype_BIN:
      if (length > 0) {
        // Treat payload as 16-bit samples
        int16_t* sample_buff = (int16_t*) payload;
        size_t sample_count = length / 2;

        // Apply gain to each sample
        for (size_t i = 0; i < sample_count; i++) {
          int32_t sample = (int32_t)(sample_buff[i] * GAIN_FACTOR);
          // Clip to prevent overflow
          if (sample > 32767) sample = 32767;
          if (sample < -32768) sample = -32768;
          sample_buff[i] = (int16_t)sample;
        }

        // Write amplified data to I2S
        size_t bytesWritten;
        i2s_write(I2S_PORT, payload, length, &bytesWritten, portMAX_DELAY);
        Serial.printf("Received %d bytes via WebSocket, amplified, wrote %d bytes to I2S\n", length, bytesWritten);
        if (bytesWritten != length) {
          Serial.println("Warning: Not all bytes written to I2S!");
        }
      } else {
        Serial.println("Received empty binary message");
      }
      break;
    case WStype_ERROR:
      Serial.println("WebSocket Error");
      break;
    default:
      break;
  }
}
```
## Wiring
The ESP32 is connected to the MAX98357A I2S amplifier, which drives an 8Ω speaker. The MAX98357A combines stereo audio into mono output for a single speaker. Below is the wiring diagram:
```
      ESP32                MAX98357A                8Ω-Speaker
┌────────────┐          ┌────────────┐             ┌───────────┐
│     GPIO 27├──────────┤ DIN        │             │           │
│     GPIO 25├──────────┤ BCLK       │             │           │---
│     GPIO 26├──────────┤ LRC      + ├─────────────┤ +         │--- Sound Out
│         GND├──────────┤ GND      - ├─────────────┤ -         │---
│     5V/3.3V├──────────┤ VIN        │             │           │
└────────────┘          └────────────┘             └───────────┘
```

## Wiring Notes

### I2S Pins:
* GPIO 27 → DIN (Data In)
* GPIO 25 → BCLK (Bit Clock)
* GPIO 26 → LRC (Left/Right Clock, aka WS)


## Power:
* Connect VIN to 5V for louder output or 3.3V if 5V is unavailable.
* Ensure a common GND between ESP32, MAX98357A, and speaker.


## Speaker:
* Use an 8Ω speaker (4–8Ω, 3W recommended).
* Connect to the + and - terminals of the MAX98357A.


## Gain:
* The code applies a software gain of 8x (GAIN_FACTOR = 8.0). Adjust if audio is distorted (e.g., try 4.0 or 2.0).
* Some MAX98357A modules have a hardware gain pin; check the datasheet.



## Setup Instructions

`Connect Hardware`: Wire the ESP32, MAX98357A, and speaker as shown in the diagram.

## Install Libraries:
* Install `WiFi`, `WebSocketsClient`, and` driver/i2s` libraries in the Arduino IDE.
* Ensure the `ESP32 board package` is installed.


## Configure Code:

* Update `ssid` and `password` to match your WiFi network.
* Set ws_host to the IP address of your WebSocket server.


> `Upload Code`: Upload the sketch to the ESP32 using `Arduino IDE` or `PlatformIO`.

>## `Test`:
> Open the Serial Monitor (115200 baud) to verify WiFi and WebSocket connections.
Stream audio from the WebSocket server; the speaker should output sound.



## Troubleshooting

### No Sound:
* Check wiring and ensure all connections are secure.
Verify the WebSocket server is running and accessible.
Confirm the speaker is functional and properly connected.


### Distorted Audio:
* Reduce GAIN_FACTOR in the code.
* Ensure a stable power supply (5V preferred).


### WebSocket Errors:
* Check the server IP (ws_host) and port (ws_port).
* Ensure the ESP32 is connected to the WiFi network.



> ## Notes:---
> * The code uses a `16kHz sample rate` and `16-bit audio`, suitable for the `MAX98357A`.
> * The `MAX98357A` internally combines stereo to mono, so a single speaker is sufficient.
> * For `battery-powered setups`, use a `stable 5V source` to power both the `ESP32` and `MAX98357A`.
