# 🎙️ ESP32 + INMP441 Audio Streaming Project

This project captures real-time audio using the **INMP441 I2S microphone** connected to an **ESP32**, transmits the audio data over **UDP**, and saves it to a **WAV file** using a **Python server**.

---

## 🧰 Hardware Used

- ESP32 Dev Board (38-pin)
- INMP441 Digital I2S Microphone
- Wi-Fi Network

---

## 🔌 Wiring: INMP441 to ESP32

| INMP441 Pin | ESP32 Pin |
|-------------|------------|
| VCC         | 3.3V       |
| GND         | GND        |
| SD (Data)   | GPIO 13    |
| SCK (BCLK)  | GPIO 2     |
| WS (LRCLK)  | GPIO 15    |
| L/R         | GND        |

---

## 📤 ESP32: Audio Transmitter Code

```cpp
#include <WiFi.h>
#include <WiFiUdp.h>
#include <driver/i2s.h>

#define I2S_WS 15
#define I2S_SD 13
#define I2S_SCK 2
#define I2S_PORT I2S_NUM_0
#define I2S_SAMPLE_RATE   (16000)
#define I2S_SAMPLE_BITS   (16)
#define I2S_READ_LEN      (1024)
#define I2S_CHANNEL_NUM   (1)
#define UDP_PORT          12345

WiFiUDP udp;
const char* ssid = "Laptop";
const char* password = "8918172922";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  i2sInit();
  udp.begin(UDP_PORT);
  xTaskCreate(i2s_adc, "i2s_adc", 1024 * 2, NULL, 1, NULL);
}

void loop() {}

void i2sInit() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = I2S_SAMPLE_RATE,
    .bits_per_sample = i2s_bits_per_sample_t(I2S_SAMPLE_BITS),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0,
    .dma_buf_count = 64,
    .dma_buf_len = 1024,
    .use_apll = 1
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);
}

void i2s_adc_data_scale(uint8_t * d_buff, uint8_t* s_buff, uint32_t len) {
  uint32_t j = 0;
  uint32_t dac_value = 0;
  for (int i = 0; i < len; i += 2) {
    dac_value = ((((uint16_t)(s_buff[i + 1] & 0xf) << 8) | ((s_buff[i + 0]))));
    d_buff[j++] = 0;
    d_buff[j++] = dac_value * 256 / 2048;
  }
}

void i2s_adc(void *arg) {
  int i2s_read_len = I2S_READ_LEN;
  size_t bytes_read;
  char* i2s_read_buff = (char*) calloc(i2s_read_len, sizeof(char));
  uint8_t* udp_send_buff = (uint8_t*) calloc(i2s_read_len, sizeof(char));

  Serial.println(" *** Streaming Start *** ");

  while (true) {
    i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
    i2s_adc_data_scale(udp_send_buff, (uint8_t*) i2s_read_buff, i2s_read_len);
    udp.beginPacket("192.168.218.231", UDP_PORT);  // Set this to your server IP
    udp.write(udp_send_buff, i2s_read_len);
    udp.endPacket();
    Serial.println("Packet sent");
  }

  free(i2s_read_buff);
  free(udp_send_buff);
  vTaskDelete(NULL);
}
```

# 🖥️ Python Server: Convert PCM to WAV

```python
import socket
import wave

UDP_IP = "0.0.0.0"
UDP_PORT = 12345

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print(f"Listening on {UDP_IP}:{UDP_PORT} for incoming data...")

SAMPLE_RATE = 16000
SAMPLE_WIDTH = 2
NUM_CHANNELS = 1
BUFFER_SIZE = 1024

with wave.open("received_audio.wav", "wb") as wave_file:
    wave_file.setnchannels(NUM_CHANNELS)
    wave_file.setsampwidth(SAMPLE_WIDTH)
    wave_file.setframerate(SAMPLE_RATE)

    while True:
        data, addr = sock.recvfrom(BUFFER_SIZE)
        if data:
            print(f"Received {len(data)} bytes from {addr}")
            wave_file.writeframes(data)
```

# 🎧 Output

- The ESP32 continuously sends audio data over Wi-Fi via UDP.

- The Python script receives the data and writes it to received_audio.wav.

- You can play the .wav file with any audio software (e.g., VLC, Audacity).

# 🧠 Notes

- The IP address in the ESP32 code should match the computer running the Python server.
- Make sure the PC and ESP32 are connected to the same Wi-Fi network.
- Stop the Python script manually to finalize and close the WAV file properly.

# 👤 Author
**Name:** `Araham Abeddin`

**Date:** April 20, 2025

**Project:** Real-time I2S Audio Streaming with ESP32 & INMP441

