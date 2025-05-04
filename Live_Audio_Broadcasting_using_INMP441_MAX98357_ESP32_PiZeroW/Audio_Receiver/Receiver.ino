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


// Receiver Code :---
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <driver/i2s.h>

const char* ssid = "Laptop";
const char* password = "8918172922";
const char* ws_host = "192.168.239.231"; // WebSocket server IP
const uint16_t ws_port = 8080;
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