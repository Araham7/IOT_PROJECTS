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
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <LiquidCrystal_I2C.h>

#define rxPin 4
#define txPin 3

TinyGPSPlus gps;
LiquidCrystal_I2C lcd(0x3F, 16, 2);

SoftwareSerial neogps(rxPin,txPin);

const int timezone_hours = 5; // Isme humlogon ne 5 likha ha kyunki hamara IST(Time zone) , UTC ke comparision me 5 hour aage hai .
const int timezone_minutes = 30; // Isme humlogon ne 30 likha hai kunki hamara IST(Time zone) , UTC ke comparision me 30 min aage hai .
//-------------------------- Program variables ---------------------------

int day, month, year;
String hours, minutes, Date;
int second;


void setup() {
   // initialize the LCD
   Serial.begin(9600);
   neogps.begin(9600);
   lcd.init();

  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.clear();
}

void loop() {
  Read_GPS();
 
//  lcd.setCursor(0,0);
//  lcd.print("Time = " + String(hours)+":"+String(minutes));
//  lcd.setCursor(0,1);
//  lcd.print("Date = " );

}

void Read_GPS(){
  //------------------------------------------------------------------
  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (neogps.available())
    {
      if (gps.encode(neogps.read()))
      {
        newData = true;
        break;
      }
    }
  }
  //------------------------------------------------------------------
  //If newData is true
  if(newData == true){
    newData = false;
    Get_GPS();
  }
  else {
    //no data
  }
}

void Get_GPS(){
  if (gps.time.isValid()){
    int hour = gps.time.hour();
    int minute = gps.time.minute();
    second = gps.time.second();

    abc(hour, minute);
   
    hours   = String(hour);
    minutes = String(minute);
    hours = (hours.length() == 1) ? "0"+hours : hours;
    minutes = (minutes.length() == 1) ? "0"+minutes : minutes;
  }
}

void abc(int &hour, int &minute){
 
//  ----------------------------------------------------------------
//  This will print time in UTC formet :---
//    Serial.println("UTC =  "+String(hour)+":"+String(minute)+":"+String(second));
//------------------------------------------------------------------

    minute = minute + timezone_minutes;
    if(minute >= 60){
      minute = minute - 60;
      hour = hour + 1;
    }
    if(minute < 0){
      minute = minute + 60;
      hour = hour - 1;
    }
    hour = hour + timezone_hours;
    if(hour >= 24){
      hour = hour - 24;
    }
    else if(hour < 0){
      hour = hour + 24;
    }
   
  lcd.setCursor(0,0);
  Date = String(gps.date.day()) + "/" + String(gps.date.month()) + "/" + String(gps.date.year());
  lcd.print("Date = " + Date );
//--------------------------------------------------------------------
  lcd.setCursor(0,1);
    int h;
    if(hour > 12 ){
      h = hour - 12 ;
    }

     String AmPm;
     if(hour > 12){
      AmPm = "pm";
     }else{
      AmPm = "am";
     }
  lcd.print("Time = " + String(h)+":"+String(minutes)+" "+AmPm);
   Serial.println("Date = " + Date );
   Serial.println("Time = " + String(h)+":"+String(minutes)+" "+ AmPm );
   Serial.println("");
}

# How to Use

1. **Connect the Components**: Wire the GPS module and LCD display to the Arduino Nano according to the circuit diagram.
2. **Upload the Code**: Upload the provided Arduino code to your Arduino Nano using the Arduino IDE.
3. **Power Up**: Power the Arduino Nano. The LCD will start displaying the current time and date obtained from the GPS module.

# Troubleshooting

- **No Display**: Check the wiring of the I2C LCD. Make sure the connections are correct and the I2C address in the code matches your LCD.
- **No GPS Signal**: Ensure the GPS module has a clear view of the sky for satellite signals.

