//Wiring :(I'm using here Arduino Nano Microcontroller)
//NRF24L01:  
//   VCC → 3.3V  
//   GND → GND  
//   CE  → D9  
//   CSN → D10  
//   SCK → D13  
//   MOSI→ D11  
//   MISO→ D12  
//
//Joystick 1:  
//   VRx → A1  
//   VRy → A0  
//   SW  → D2  
//
//Joystick 2:  
//   VRx → A3  
//   VRy → A2  
//   SW  → D3  
//
//Switches:  
//   SW1 → D4  
//   SW2 → D5  
//   SW3 → D6  
//   SW4 → D7  
//   (All switches' other side → GND)  




#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// NRF24L01 pin configuration
#define CE_PIN 9
#define CSN_PIN 10

RF24 radio(CE_PIN, CSN_PIN);

// NRF24L01 communication address
const byte address[6] = "00001";

// Joystick 1 Pins
const int joy1X = A1;
const int joy1Y = A0;
const int joy1SW = 2;

// Joystick 2 Pins
const int joy2X = A3;
const int joy2Y = A2;
const int joy2SW = 3;

// Switch Pins (connected to ground when pressed)
const int switch1 = 4;
const int switch2 = 5;
const int switch3 = 6;
const int switch4 = 7;

// Structure to hold joystick and switch data
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
  Serial.begin(9600);  // For debugging

  // Joystick button pins as input with pullup
  pinMode(joy1SW, INPUT_PULLUP);
  pinMode(joy2SW, INPUT_PULLUP);
  
  // Switch pins as input with pullup
  pinMode(switch1, INPUT_PULLUP);
  pinMode(switch2, INPUT_PULLUP);
  pinMode(switch3, INPUT_PULLUP);
  pinMode(switch4, INPUT_PULLUP);

  // Initialize NRF24L01
  radio.begin();
  radio.openWritingPipe(address);      // Set transmitter address
  radio.setPALevel(RF24_PA_LOW);       // Power level (LOW, MEDIUM, HIGH, MAX)
  radio.setDataRate(RF24_250KBPS);     // Lower data rate = better range
  radio.stopListening();               // Set as transmitter
}

void loop() {
  // Read joystick values
  ControlData data;
  data.j1x = analogRead(joy1X);
  data.j1y = analogRead(joy1Y);
  data.j1Btn = digitalRead(joy1SW) == LOW;

  data.j2x = analogRead(joy2X);
  data.j2y = analogRead(joy2Y);
  data.j2Btn = digitalRead(joy2SW) == LOW;

  // Read switch values (LOW when pressed)
  data.sw1 = digitalRead(switch1) == LOW;
  data.sw2 = digitalRead(switch2) == LOW;
  data.sw3 = digitalRead(switch3) == LOW;
  data.sw4 = digitalRead(switch4) == LOW;

  // Send data over NRF24L01
  bool success = radio.write(&data, sizeof(data));

 
  // Debug output
  Serial.print("Sending: ");
  Serial.print("J1("); Serial.print(data.j1x); Serial.print(", "); Serial.print(data.j1y); Serial.print(", "); Serial.print(data.j1Btn); Serial.print(") | ");
  Serial.print("J2("); Serial.print(data.j2x); Serial.print(", "); Serial.print(data.j2y); Serial.print(", "); Serial.print(data.j2Btn); Serial.print(") | ");
  Serial.print("SW("); Serial.print(data.sw1); Serial.print(", "); Serial.print(data.sw2); Serial.print(", "); Serial.print(data.sw3); Serial.print(", "); Serial.print(data.sw4); Serial.print(")");
  Serial.print(" => ");
  Serial.println(success ? "Success" : "Failed");

  delay(300);  // Small delay to avoid flooding
}
