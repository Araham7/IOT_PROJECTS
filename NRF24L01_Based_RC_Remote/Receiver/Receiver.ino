/*
 * Wiring Configuration: (I'm using here Arduino Nano Microcontroller.)
 * 
 * NRF24L01 Module:
 *   CE  -> Arduino pin 9
 *   CSN -> Arduino pin 10
 *   SCK -> Arduino pin 13
 *   MOSI-> Arduino pin 11
 *   MISO-> Arduino pin 12
 *   IRQ -> Not connected
 *   VCC -> 3.3V (Important! Don't use 5V)
 *   GND -> GND
 * 
 * OLED Display (I2C 128x32):
 *   SDA -> Arduino A4 (or SDA pin)
 *   SCL -> Arduino A5 (or SCL pin)
 *   VCC -> 3.3V or 5V (check display specs)
 *   GND -> GND
 *   
 * Serial Output:     
 * USB → Computer
 * 
 */


#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ==================== Hardware Configuration ====================
// NRF24L01 Wiring:
#define CE_PIN   9
#define CSN_PIN 10

// OLED 128x32 I2C Configuration:
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1  // Reset pin not used
#define OLED_ADDRESS 0x3C

// ==================== Constants ====================
const byte RADIO_ADDRESS[6] = "00001";
const unsigned long SIGNAL_TIMEOUT_MS = 1500;  // 1.5 seconds

// ==================== Global Objects ====================
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RF24 radio(CE_PIN, CSN_PIN);

// ==================== Data Structures ====================
struct ControlData {
  // Joystick 1 (Left)
  int j1x;      // Yaw
  int j1y;      // Throttle
  bool j1Btn;   // Button
  
  // Joystick 2 (Right)
  int j2x;      // Roll
  int j2y;      // Pitch
  bool j2Btn;   // Button
  
  // Toggle switches
  bool sw1;
  bool sw2;
  bool sw3;
  bool sw4;
};

// ==================== Global Variables ====================
unsigned long lastReceivedTime = 0;

// ==================== Function Prototypes ====================
void initializeRadio();
void initializeDisplay();
void displayControlData(const ControlData &data);
void displayNoSignalMessage();

// ==================== Main Functions ====================
void setup() {
  Serial.begin(9600);
  
  initializeRadio();
  initializeDisplay();
  
  Serial.println("Receiver Ready. Waiting for data...");
}

void loop() {
  ControlData data;
  bool dataReceived = false;

  if (radio.available()) {
    radio.read(&data, sizeof(data));
    lastReceivedTime = millis();
    dataReceived = true;
  }

  if (dataReceived) {
    printDebugData(data);
    displayControlData(data);
  }

  // Check for signal timeout
  if (millis() - lastReceivedTime > SIGNAL_TIMEOUT_MS) {
    displayNoSignalMessage();
  }
}

// ==================== Initialization Functions ====================
void initializeRadio() {
  if (!radio.begin()) {
    Serial.println("Radio initialization failed!");
    while (true); // Halt if radio fails
  }
  
  radio.openReadingPipe(0, RADIO_ADDRESS);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();
}

void initializeDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("OLED initialization failed!");
    while (true); // Halt if display fails
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();
  delay(500); // Brief pause to show message
}

// ==================== Display Functions ====================
void displayControlData(const ControlData &data) {
  display.clearDisplay();
  
  // Row 1: Throttle and Roll
  display.setCursor(0, 0);
  display.print("Thr:"); display.print(data.j1y);
  
  display.setCursor(64, 0);
  display.print("Rol:"); display.print(data.j2x);

  // Row 2: Pitch and Yaw
  display.setCursor(0, 10);
  display.print("Pit:"); display.print(data.j2y);
  
  display.setCursor(64, 10);
  display.print("Yaw:"); display.print(data.j1x);

  // Row 3: Buttons and Switches
  display.setCursor(0, 20);
  display.print("B1:"); display.print(data.j1Btn ? "P" : "-");
  
  display.setCursor(32, 20);
  display.print("B2:"); display.print(data.j2Btn ? "P" : "-");
  
  display.setCursor(64, 20);
  display.print("SW:");
  display.print(data.sw1 ? "1" : "0");
  display.print(data.sw2 ? "1" : "0");
  display.print(data.sw3 ? "1" : "0");
  display.print(data.sw4 ? "1" : "0");

  display.display();
}

void displayNoSignalMessage() {
  display.clearDisplay();
  display.setCursor(0, 5);
  display.setTextSize(1);
  display.println("  NO SIGNAL!");
  display.setCursor(0, 20);
  display.println("On Standby..."); // Waiting for transmitter
  display.display();
}

// ==================== Debug Functions ====================
void printDebugData(const ControlData &data) {
  Serial.print("Throttle:"); Serial.print(data.j1y);
  Serial.print(" Yaw:"); Serial.print(data.j1x);
  Serial.print(" Pitch:"); Serial.print(data.j2y);
  Serial.print(" Roll:"); Serial.print(data.j2x);
  
  Serial.print(" | B1:"); Serial.print(data.j1Btn ? "P" : "-");
  Serial.print(" B2:"); Serial.print(data.j2Btn ? "P" : "-");
  
  Serial.print(" | SW:");
  Serial.print(data.sw1 ? "1" : "0");
  Serial.print(data.sw2 ? "1" : "0");
  Serial.print(data.sw3 ? "1" : "0");
  Serial.println(data.sw4 ? "1" : "0");
}
