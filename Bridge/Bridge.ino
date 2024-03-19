#include <Arduino.h>
#include <stdint.h>
#include "ControlMode.h"
#include "BridgeControlMode.h"
#include "LedHeadlessControlMode.h"
//#include "HeadlessControlMode.h"

BridgeControlMode* controlMode;

void setup() {
  long start = millis();
  Serial.begin(115200);
  // if log enabled, wait up to 5 sec for serial to become available
  if (LOG_ENABLED) {
    while (!Serial) {
      if (start + 5000 < millis()) {
        break;
      }
      delay(10);
    } 
  }
  delay(100);

  // the bridge type test pins
  pinMode(JOIN_CLUSTER_PIN, INPUT_PULLUP);
  pinMode(BRIDGE_ID_PIN_WIFI, INPUT_PULLUP);
  pinMode(FACTORY_RESET_PIN, INPUT_PULLUP);

  //Wire.setClock(400000L); // enc chip wants 400khz
  Wire.begin();

  SPI.begin();

  controlMode = new LedHeadlessControlMode();

  if(!controlMode->init()) {
    logConsole("Error initializing!");
  }
}

void loop() {
  controlMode->loop();
}


void logConsole (String msg) {
  Serial.println(msg);
}