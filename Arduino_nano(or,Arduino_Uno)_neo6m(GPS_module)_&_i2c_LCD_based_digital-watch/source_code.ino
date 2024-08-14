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
