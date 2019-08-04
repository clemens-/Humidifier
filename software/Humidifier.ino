/*
 * Copyright 2019 Clemens Clausen
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *  
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Uses the send-only serial library by Nick Gammon and the 
// arduino-sht library by Sensirion
// https://github.com/nickgammon/SendOnlySoftwareSerial
// https://github.com/Sensirion/arduino-sht/blob/master/SHTSensor.h
#include <SendOnlySoftwareSerial.h>
#include <Wire.h>
#include "SHTSensor.h"

#define HUMREF 65 // Reference value for the humidity
#define HUMHYST 3 // Tolerance for the humidity, +- HUMHYST %RH
#define MAXALARMCOUNTS 3600
#define FAN PIN_B3
#define IND PIN_B4

SendOnlySoftwareSerial mySerial(PIN_B1);  // Tx pin

// SHTSensor sht;
// To use a specific sensor instead of probing the bus use this command:
SHTSensor sht(SHTSensor::SHT3X);

uint16_t alarm_counter = 0;

void setup() {
  // Set the fan and indicator pins
  pinMode(FAN, OUTPUT);
  digitalWrite(FAN, LOW);
  pinMode(IND, OUTPUT);
  digitalWrite(IND, HIGH);
  delay(50);
  digitalWrite(IND, LOW);
  // Start the serial connection
  mySerial.begin(115200);
  delay(100);
  // Start the I2C/TWI interface
  Wire.begin();
  // Try to init the humidity sensor
  if (sht.init()) {
      mySerial.println("init(): success");
  } else {
      mySerial.println("init(): failed");
  }
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_HIGH); // only supported by SHT3x
}

void loop() {  
  // Get the current humidity and temperature values
  if (sht.readSample()) {
    // Send to serial connection for debug
    mySerial.print("RH: ");
    mySerial.print(sht.getHumidity(), 2);
    mySerial.print("  T: ");
    mySerial.println(sht.getTemperature(), 2);
    
    // Decide whether the fan should be switched on to increase humidity
    if(sht.getHumidity() < (HUMREF - HUMHYST)) {
      digitalWrite(FAN, HIGH);
      alarm_counter++;
      mySerial.print("Alarm: ");
      mySerial.println(alarm_counter);
    } else if (sht.getHumidity() > (HUMREF + HUMHYST)) {
      digitalWrite(FAN, LOW);
      alarm_counter = 0;
//      WDT_Initialization_as_Wakeup_Source();
    }
    
    // If the alarm counter increases, do periodic pauses to check the actual humidity
    if( (alarm_counter % (MAXALARMCOUNTS / 8)) == 0) {
      digitalWrite(FAN, LOW);
      delay(30000);
    }

    
    // Check if the humidity cannot increase and sound the alarm once
    if(alarm_counter >= MAXALARMCOUNTS) {
      alarm_counter = 0;
      digitalWrite(IND, HIGH);
      delay(500);
      digitalWrite(IND, LOW);
      delay(500);
      digitalWrite(IND, HIGH);
      delay(500);
      digitalWrite(IND, LOW);
      delay(500);
    }
  } else {
    // If the sensor cannot be read, sound a different alarm
    mySerial.println("Error in readSample()");
    digitalWrite(IND, HIGH);
    delay(200);
    digitalWrite(IND, LOW);
    delay(200);
    digitalWrite(IND, HIGH);
    delay(200);
    digitalWrite(IND, LOW);
    delay(200);
    digitalWrite(IND, HIGH);
    delay(200);
    digitalWrite(IND, LOW);
  }
  // Wait 1 seconds before the next measurement
  delay(1000);
}
