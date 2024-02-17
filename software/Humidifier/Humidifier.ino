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
#include <arduino-timer.h>
#include <SendOnlySoftwareSerial.h>
#include <Wire.h>
#include "SHTSensor.h"

#define HUMREF 65 // Reference value for the humidity
#define HUMHYST 3 // Tolerance for the humidity, +- HUMHYST %RH
#define FAN PIN_B3
#define IND PIN_B4

auto timer = timer_create_default();
SendOnlySoftwareSerial mySerial(PIN_B1);  // Tx pin
//SHTSensor sht(SHTSensor::SHT3X);
SHTSensor sht;

void checkHumidity(void) {
  if (sht.readSample()) {
//     Send to serial connection for debug
    mySerial.print("RH: ");
    mySerial.print(sht.getHumidity(), 2);
    mySerial.print("  T: ");
    mySerial.println(sht.getTemperature(), 2);
    
    // Decide whether the fan should be switched on to increase humidity
    if(sht.getHumidity() < (HUMREF - HUMHYST)) {
      digitalWrite(FAN, HIGH);
    } else if (sht.getHumidity() > (HUMREF + HUMHYST)) {
      digitalWrite(FAN, LOW);
    }
  }
}

void disableIndicator(void) {
  digitalWrite(IND, LOW);
}

void enableIndicator(unsigned long time) {
  digitalWrite(IND, HIGH);
  timer.in(50, disableIndicator);
}


void setup() {
  // Set the fan and indicator pins
  pinMode(FAN, OUTPUT);
  digitalWrite(FAN, LOW);
  pinMode(IND, OUTPUT);
  
  // Start the serial connection
  mySerial.begin(115200);
  
  // Start the I2C/TWI interface
  Wire.begin();
  delay(100);
  
  // Try to init the humidity sensor
  if (sht.init()) {
      mySerial.println("init(): success");
//    sht.setAccuracy(SHTSensor::SHT_ACCURACY_HIGH); // only supported by SHT3x
    timer.every(1000, checkHumidity);
    // Signal succesful start
    enableIndicator(500);
  } else {
      mySerial.println("init(): failed");
    enableIndicator(200);
  }
}

void loop() {
  timer.tick();
}
