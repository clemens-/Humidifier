#include <Wire.h>
#include "SHTSensor.h"
#include <SendOnlySoftwareSerial.h>

#define HUMREF 65 // Reference value for the humidity
#define HUMHYST 3 // Tolerance for the humidity, +- HUMHYST %RH
#define FAN PIN_B3
#define IND PIN_B4
#define INTERVAL_SENSOR 1000
#define INTERVAL_OK 50
#define INTERVAL_NOK 300
#define MAX_ATTEMPTS 120
#define DELAY_CONTROL 300000

SendOnlySoftwareSerial mySerial(PIN_B1);  // Tx pin

//SHTSensor sht;
// To use a specific sensor instead of probing the bus use this command:
SHTSensor sht(SHTSensor::SHT3X);

uint8_t counterAttempts = 0;
unsigned long startTimeSensor = 0, startTimeBeep = 0, intervalTimeBeep = 0, startTimeDelayControl = 0;
bool isBeep = false, isDelayControl = false;

void checkControlAttempts(void) {
  mySerial.print("#: ");
  mySerial.print(counterAttempts);
  mySerial.print("\r\n");
  if (counterAttempts >= MAX_ATTEMPTS) {
    mySerial.print("Max attempts reached");
    mySerial.print("\r\n");
    enableIndicator(INTERVAL_NOK);
    isDelayControl = true;
    startTimeDelayControl = millis();
    digitalWrite(FAN, LOW);
    counterAttempts = 0;
  }
}

void doControl(float humidity) {
  if (humidity < (HUMREF - HUMHYST)) {
    digitalWrite(FAN, HIGH);
    counterAttempts++;
  } else if (humidity > (HUMREF + HUMHYST)) {
    digitalWrite(FAN, LOW);
    counterAttempts = 0;
  }
}

float getHumidity(void) {
  float humidity = 0.0F;
  float temperature = 0.0F;
  if (sht.readSample()) {
    humidity = sht.getHumidity();
    temperature = sht.getTemperature();
    mySerial.print("SHT: ");
    mySerial.print("  RH: ");
    mySerial.print(humidity, 2);
    mySerial.print(",  T:  ");
    mySerial.print(temperature, 2);
    mySerial.print("\r\n");
  } else {
    mySerial.print("Error in readSample()\r");
  }
  return humidity;
}

void disableIndicator(void) {
  digitalWrite(IND, LOW);
}

void enableIndicator(unsigned long time) {
  isBeep = true;
  startTimeBeep = millis();
  intervalTimeBeep = time;
  digitalWrite(IND, HIGH);
}

void setup() {
  // put your setup code here, to run once:
  // Set the fan and indicator pins
  pinMode(FAN, OUTPUT);
  digitalWrite(FAN, LOW);
  pinMode(IND, OUTPUT);

  Wire.begin();

  mySerial.begin(9600);
  delay(1000); // let serial console settle

  if (sht.init()) {
    mySerial.print("init(): success\n");
    enableIndicator(INTERVAL_OK);
  } else {
    mySerial.print("init(): failed\n");
    enableIndicator(INTERVAL_NOK);
  }
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x

}

void loop() {
  // put your main code here, to run repeatedly:
  if (isDelayControl == false) {
    if (millis() - startTimeSensor >= INTERVAL_SENSOR) {
      startTimeSensor = millis();
      float humidity = getHumidity();
      doControl(humidity);
      checkControlAttempts();
    }
  }

  if (isBeep) {
    if (millis() - startTimeBeep >= intervalTimeBeep) {
      isBeep = false;
      disableIndicator();
    }
  }

  if (isDelayControl) {
    if (millis() - startTimeDelayControl >= DELAY_CONTROL) {
      isDelayControl = false;
    }
  }
}
