/*
Wiring Instructions:

1. **OLED Display Wiring (I2C):**
   - OLED VCC -> Arduino 5V
   - OLED GND -> Arduino GND
   - OLED SCL -> Arduino A5
   - OLED SDA -> Arduino A4

2. **nRF24L01 Wiring:**
   - VCC -> 3.3V (NOT 5V) [Add 10uF capacitor across VCC-GND]
   - GND -> GND
   - CE  -> D9
   - CSN -> D10
   - SCK -> D13
   - MOSI -> D11
   - MISO -> D12

3. **Joystick 1 (Throttle/Yaw):**
   - VRx (Yaw)     -> A1
   - VRy (Throttle)-> A0
   - SW (Button)   -> D2

4. **Joystick 2 (Roll/Pitch):**
   - VRx (Roll)    -> A3
   - VRy (Pitch)   -> A2
   - SW (Button)   -> D3

5. **Switches:**
   - Switch1 -> D4
   - Switch2 -> D5
   - Switch3 -> D6
   - Switch4 -> D7
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ------------------ OLED Display Configuration ------------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ------------------ NRF24L01 Configuration ------------------
#define CE_PIN 9
#define CSN_PIN 10
RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

// ------------------ Joystick & Switch Pin Definitions ------------------
const int joy1X = A1;
const int joy1Y = A0;
const int joy1SW = 2;
const int joy2X = A3;
const int joy2Y = A2;
const int joy2SW = 3;
const int switch1 = 4;
const int switch2 = 5;
const int switch3 = 6;
const int switch4 = 7;

// ------------------ Throttle Logic Variables ------------------
int throttle = 0;
unsigned long previousThrottleUpdateTime = 0;
unsigned long throttleUpdateInterval = 50;

// ------------------ Send Timing ------------------
unsigned long previousSendTime = 0;
unsigned long sendInterval = 100;

// ------------------ Data Structure to Transmit ------------------
struct ControlData {
  int j1x;
  int j1y;
  bool j1Btn;
  int j2x;
  int j2y;
  bool j2Btn;
  bool sw1;
  bool sw2;
  bool sw3;
  bool sw4;
};

void setup() {
  Serial.begin(9600);

  pinMode(joy1SW, INPUT_PULLUP);
  pinMode(joy2SW, INPUT_PULLUP);
  pinMode(switch1, INPUT_PULLUP);
  pinMode(switch2, INPUT_PULLUP);
  pinMode(switch3, INPUT_PULLUP);
  pinMode(switch4, INPUT_PULLUP);

  // OLED Init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();
  delay(1000);

  // NRF24L01 Init
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.stopListening();
}

void loop() {
  unsigned long currentMillis = millis();

  // Throttle Logic
  if (currentMillis - previousThrottleUpdateTime >= throttleUpdateInterval) {
    previousThrottleUpdateTime = currentMillis;

    int throttleValue = analogRead(joy1Y);

    if (throttleValue > 550 && throttle < 1023) {
      throttle += 5;
      if (throttle > 1023) throttle = 1023;
    } else if (throttleValue < 500 && throttle > 0) {
      throttle -= 5;
      if (throttle < 0) throttle = 0;
    }
  }

  // Send Data to Receiver
  if (currentMillis - previousSendTime >= sendInterval) {
    previousSendTime = currentMillis;

    ControlData data;
    data.j1x = analogRead(joy1X);
    data.j1y = throttle;
    data.j1Btn = digitalRead(joy1SW) == LOW;
    data.j2x = analogRead(joy2X);
    data.j2y = analogRead(joy2Y);
    data.j2Btn = digitalRead(joy2SW) == LOW;
    data.sw1 = digitalRead(switch1) == LOW;
    data.sw2 = digitalRead(switch2) == LOW;
    data.sw3 = digitalRead(switch3) == LOW;
    data.sw4 = digitalRead(switch4) == LOW;

    bool success = radio.write(&data, sizeof(data));

    // Serial Debug
    Serial.print("Throttle: "); Serial.print(throttle);
    Serial.print(" | Yaw: "); Serial.print(data.j1x);
    Serial.print(" | Pitch: "); Serial.print(data.j2y);
    Serial.print(" | Roll: "); Serial.print(data.j2x);
    Serial.print(" | Buttons: ");
    Serial.print(data.sw1); Serial.print(" ");
    Serial.print(data.sw2); Serial.print(" ");
    Serial.print(data.sw3); Serial.print(" ");
    Serial.println(data.sw4);
    Serial.println(success ? "Data sent successfully" : "Data send failed!");

    // OLED Display
    display.clearDisplay();
    display.setCursor(0, 0);

    if (success) {
      display.print("Throttle: "); display.println(throttle);
      display.print("Yaw: "); display.println(data.j1x);
      display.print("Pitch: "); display.println(data.j2y);
      display.print("Roll: "); display.println(data.j2x);
      display.print("Btn1: "); display.print(data.j1Btn ? "P" : "-");
      display.print(" Btn2: "); display.println(data.j2Btn ? "P" : "-");
      display.print("SW: ");
      display.print(data.sw1); display.print(" ");
      display.print(data.sw2); display.print(" ");
      display.print(data.sw3); display.print(" ");
      display.println(data.sw4);
    } else {
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 10);
      display.println("Receiver Not Found");
      display.setCursor(0, 25);
      display.println("Waiting for connection");
      display.setCursor(0, 40);
      display.println("Retrying...");
    }

    display.display();
  }
}
