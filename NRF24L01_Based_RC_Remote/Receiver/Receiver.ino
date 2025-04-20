//Wiring: (I'm using here Arduino Nano Microcontroller.)
//NRF24L01:  
//   VCC → 3.3V  
//   GND → GND  
//   CE  → D9  
//   CSN → D10  
//   SCK → D13  
//   MOSI→ D11  
//   MISO→ D12  
//
//Serial Output:  
//   USB → Computer (For JSON data)  


#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ArduinoJson.h>

// NRF24L01 pin configuration
#define CE_PIN 9
#define CSN_PIN 10

RF24 radio(CE_PIN, CSN_PIN);

// Same address as transmitter
const byte address[6] = "00001";

// Structure to match transmitted data
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
  while (!Serial); // Wait for serial port to connect (for native USB)

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    ControlData data;
    radio.read(&data, sizeof(data));

    // Create JSON document with increased capacity for pretty formatting
    StaticJsonDocument<300> doc;
    
    // Create nested objects
    JsonObject joystick1 = doc.createNestedObject("joystick1");
    joystick1["x1"] = data.j1x;
    joystick1["y1"] = data.j1y;
    joystick1["button01"] = data.j1Btn;
    
    JsonObject joystick2 = doc.createNestedObject("joystick2");
    joystick2["x2"] = data.j2x;
    joystick2["y2"] = data.j2y;
    joystick2["button02"] = data.j2Btn;
    
    JsonObject switches = doc.createNestedObject("switches");
    switches["sw1"] = data.sw1;
    switches["sw2"] = data.sw2;
    switches["sw3"] = data.sw3;
    switches["sw4"] = data.sw4;

    // Serialize pretty JSON to Serial with indentation
//    serializeJsonPretty(doc, Serial); // Uncomment this for getting pretter JSONformeted received data.
    serializeJson(doc, Serial);
    Serial.println("\n"); // Extra newlines for better readability
  }
}
