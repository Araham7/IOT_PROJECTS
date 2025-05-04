# ESP32 Audio Transmitter with INMP441

## Author: Araham Abeddin

This project configures an ESP32 as an audio transmitter that captures 16-bit, 16kHz audio from an I2S microphone (e.g., INMP441) and streams it over a WebSocket connection to a server. The ESP32 connects to a WiFi network, establishes a WebSocket connection, and uses I2S to read audio data, which is then amplified and sent to the server.

## <a href="./IMAGES/" title="View the list of required components">Required Components</a>

* ESP32 (e.g., ESP32-WROOM-32)
* INMP441 I2S microphone module (or similar I2S microphone)
* Power Supply (3.3V for INMP441, provided by ESP32)
* Jumper Wires
* Breadboard (optional)

>See the Required Components for a detailed list.

# Code

The following Arduino sketch configures the ESP32 to capture audio via I2S and stream it over WebSocket.

```cpp
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

// Transmitter Code
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <driver/i2s.h>

#define I2S_WS 15    // Word Select
#define I2S_SD 13    // Serial Data
#define I2S_SCK 2    // Serial Clock
#define I2S_PORT I2S_NUM_0
#define I2S_SAMPLE_RATE   (16000)
#define I2S_SAMPLE_BITS   (16)
#define I2S_READ_LEN      (512)
#define I2S_CHANNEL_NUM   (2)
#define GAIN_FACTOR       (4.0) // Adjust this (1.0 = no gain, 4.0 = 4x amplification)

WebSocketsClient webSocket;
const char* ssid = "Laptop";
const char* password = "8918172922";
const char* ws_host = "192.168.239.231"; // WebSocket server IP
const uint16_t ws_port = 8080;
const char* ws_path = "/";

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.println(WiFi.localIP());

  // Initialize WebSocket
  webSocket.begin(ws_host, ws_port, ws_path);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);

  // Initialize I2S
  i2sInit();

  // Create a task to handle I2S ADC
  xTaskCreate(i2s_adc, "i2s_adc", 4096, NULL, 1, NULL);
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
    case WStype_ERROR:
      Serial.println("WebSocket Error");
      break;
    default:
      break;
  }
}

void i2sInit() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = I2S_SAMPLE_RATE,
    .bits_per_sample = i2s_bits_per_sample_t(I2S_SAMPLE_BITS),
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = true
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);
}

void i2s_adc(void *arg) {
  int i2s_read_len = I2S_READ_LEN;
  size_t bytes_read;

  uint8_t* i2s_read_buff = (uint8_t*) calloc(i2s_read_len, sizeof(uint8_t));
  int16_t* sample_buff = (int16_t*) i2s_read_buff; // Treat buffer as 16-bit samples

  Serial.println(" *** Streaming Start *** ");

  while (true) {
    i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);

    if (bytes_read > 0) {
      // Apply gain to each 16-bit sample
      for (size_t i = 0; i < bytes_read / 2; i++) {
        int32_t sample = (int32_t)(sample_buff[i] * GAIN_FACTOR);
        // Clip to prevent overflow
        if (sample > 32767) sample = 32767;
        if (sample < -32768) sample = -32768;
        sample_buff[i] = (int16_t)sample;
      }

      if (webSocket.isConnected()) {
        webSocket.sendBIN(i2s_read_buff, bytes_read);
        Serial.printf("Read %d bytes from I2S, amplified, sent %d bytes over WebSocket\n", bytes_read, bytes_read);
      } else {
        Serial.println("WebSocket not connected, skipping send");
      }
    } else {
      Serial.println("No data read from I2S");
    }
    vTaskDelay(1); // Prevent task starvation
  }

  free(i2s_read_buff);
  vTaskDelete(NULL);
}
```

## Wiring

The ESP32 is connected to an INMP441 I2S microphone to capture audio. Below is the wiring diagram:
```
      ESP32                INMP441
┌────────────┐          ┌────────────┐
│     GPIO 13├──────────┤ SD         │
│     GPIO 2 ├──────────┤ SCK        │
│     GPIO 15├──────────┤ WS         │
│        GND ├──────────┤ L/R        │
│        GND ├──────────┤ GND        │
│        3.3V├──────────┤ VDD        │
└────────────┘          └────────────┘
```

## Wiring Notes

### `I2S Pins`:
* GPIO 13 → SD (Serial Data)
* GPIO 2 → SCK (Serial Clock)
* GPIO 15 → WS (Word Select)


## Power:
* Connect VDD to 3.3V (INMP441 operates at 1.8–3.3V).
* Ensure a common GND between ESP32 and INMP441.


## INMP441 Configuration:
* The INMP441 has an L/R pin to select left or right channel. For mono audio, connect L/R to GND (left channel) or leave floating (check datasheet).
* Ensure the microphone is positioned to capture sound effectively.


## Gain:
* The code applies a software gain of 4x (GAIN_FACTOR = 4.0). Adjust if audio is too loud or quiet (e.g., try 2.0 or 8.0).



## Setup Instructions

 Connect Hardware: Wire the ESP32 and INMP441 as shown in the diagram.

* > # Install Libraries:
  > Install `WiFi`, `WebSocketsClient`, and `driver/i2s` libraries in the Arduino IDE.
  > Ensure the `ESP32 board package is installed`.


## Configure Code:
Update ssid and password to match your WiFi network.
Set ws_host to the IP address of your WebSocket server.


* `Upload Code:` Upload the sketch to the ESP32 using Arduino IDE or PlatformIO.

## Test:
* Open the Serial Monitor (115200 baud) to verify WiFi and WebSocket connections.
* Speak into the microphone; the audio should be streamed to the WebSocket server and receivable by a paired receiver (e.g., the ESP32 audio receiver project).

## Troubleshooting

### No Audio Stream:

* Check wiring and ensure all connections are secure.
Verify the WebSocket server is running and accessible.
Confirm the INMP441 is functional and correctly configured (e.g., L/R pin setting).


### Low or Distorted Audio:

* Adjust GAIN_FACTOR in the code.
* Ensure a stable 3.3V power supply.
* Check microphone placement and environment noise.


### WebSocket Errors:
* Check the server IP (ws_host) and port (ws_port).
* Ensure the ESP32 is connected to the WiFi network.



> Notes :---
>* The code uses a 16kHz sample rate and 16-bit audio, suitable for the INMP441.
>* The INMP441 is a mono microphone, but the code is configured for stereo (I2S_CHANNEL_FMT_RIGHT_LEFT). The unused channel will be silent.
>* For battery-powered setups, use a stable 3.3V source for the ESP32 and INMP441.
>* This transmitter is designed to work with a corresponding receiver (e.g., an ESP32 with MAX98357A) to complete the audio streaming system.

