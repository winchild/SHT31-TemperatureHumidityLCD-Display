/*************************************************** 
  This is an example for the SHT31-D Humidity & Temp Sensor

  Designed specifically to work with the SHT31-D sensor from Adafruit
  ----> https://www.adafruit.com/products/2857

  These sensors use I2C to communicate, 2 pins are required to  
  interface
 ****************************************************/
 
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Adafruit_SHT31.h"

Adafruit_SHT31 sht31 = Adafruit_SHT31();
LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display


void setup() {
  Serial.begin(9600);

    lcd.init();                      // initialize the lcd 
  
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("SHT Temperate & ");
  lcd.setCursor(0,1);
  lcd.print("Humidity Control");

  while (!Serial)
    delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  // Serial.println("SHT31 test");
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    //Serial.println("Couldn't find SHT31");
    lcd.setCursor(0,0);
    lcd.print ("Couldn't find SHT31");
    while (1) delay(1);
  }
}


void loop() {
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();

  if (! isnan(t)) {  // check if 'is not a number'
    lcd.setCursor(0,0);
    lcd.print("TEMP: ");
    lcd.print (t);
    lcd.print (" C   ");
  } else { 
    lcd.println("Failed to TEMP.");
  }
  
  if (! isnan(h)) {  // check if 'is not a number'
    lcd.setCursor(0,1);
    lcd.print("HUMI: ");
    lcd.print (h);
    lcd.print (" %   ");
  } else { 
    lcd.println("Failed to HUMDT.");
  }
  delay(1000);
}
