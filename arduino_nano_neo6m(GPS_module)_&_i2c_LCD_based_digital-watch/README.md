# Arduino Nano Neo-6M GPS Module & I2C LCD Based Digital Watch

This project demonstrates how to create a digital watch using an Arduino Nano, a Neo-6M GPS module, and an I2C LCD display. The watch will display the current time and date obtained from the GPS module on the LCD screen.

## Components Used

- **Arduino Nano:** The microcontroller board used to control the project.
- **Neo-6M GPS Module:** Provides real-time location data including time and date.
- **I2C LCD Display:** An LCD screen with an I2C interface for displaying information.
- **Jumper Wires:** For connecting the components.

## Circuit Diagram

Here’s how to wire the components:

- **Neo-6M GPS Module:**
  - VCC -> 5V (Arduino)
  - GND -> GND (Arduino)
  - TX -> D2 (Arduino)
  - RX -> D3 (Arduino)

- **I2C LCD Display:**
  - VCC -> 5V (Arduino)
  - GND -> GND (Arduino)
  - SDA -> A4 (Arduino)
  - SCL -> A5 (Arduino)

## Code

Here’s the Arduino code to get started:

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Initialize the LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust the address and dimensions as necessary

// Create software serial instance for GPS module
SoftwareSerial gpsSerial(3, 2); // RX, TX

// Create TinyGPS++ instance
TinyGPSPlus gps;

void setup() {
  // Start serial communication
  Serial.begin(9600);
  gpsSerial.begin(9600);

  // Initialize the LCD
  lcd.begin();
  lcd.backlight();
}

void loop() {
  // Feed the GPS data to TinyGPS++
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  // Check if a valid GPS fix is available
  if (gps.location.isUpdated()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    
    // Display time
    lcd.print("Time: ");
    lcd.print(gps.time.hour());
    lcd.print(':');
    lcd.print(gps.time.minute());
    lcd.print(':');
    lcd.print(gps.time.second());

    // Display date
    lcd.setCursor(0, 1);
    lcd.print("Date: ");
    lcd.print(gps.date.day());
    lcd.print('/');
    lcd.print(gps.date.month());
    lcd.print('/');
    lcd.print(gps.date.year());
  }

  delay(1000); // Update every second
}
