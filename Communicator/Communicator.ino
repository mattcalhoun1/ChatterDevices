#include "Globals.h" // load first, to make sure settings propegate into chatter library
#include <Arduino.h>
#include <stdint.h>
#include "ControlMode.h"
#include "CommunicatorControlMode.h"
#include "HeadlessControlMode.h"
#include "GuiControlMode.h"
//#include "TestControlMode.h"

CommunicatorControlMode* controlMode;

void setup() {
  long start = millis();
  Serial.begin(9600);
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
  //pinMode(CLUSTER_ADMIN_PIN, INPUT_PULLUP);
  //pinMode(DEVICE_TYPE_PIN_MINI, INPUT_PULLUP);
  //pinMode(FACTORY_RESET_PIN, INPUT_PULLUP);

  #ifdef ROTARY_ENABLED
    pinMode(BUTTON_A_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_A_PIN), buttonAPressed, LOW);
  #endif

  //Wire.setClock(400000L); // enc chip wants 400khz
  Wire.begin();

  SPI.begin();

  controlMode = new GuiControlMode(selectDeviceType(), isAdmin());
  //controlMode = new TestControlMode(selectDeviceType(), isAdmin());
  
  if(!controlMode->init()) {
    logConsole("Error initializing!");
  }

  // register interrupt routine
  #ifdef ROTARY_ENABLED
    attachInterrupt(digitalPinToInterrupt(PIN_ROTARY_IN1), handleRotary, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ROTARY_IN2), handleRotary, CHANGE);
  #endif

}

DeviceType selectDeviceType () {
  /*if (!digitalRead(DEVICE_TYPE_PIN_MINI)) {
    logConsole("Device Type: Communicator Mini");
    return DeviceTypeCommunicatorMini;
  }
  */
  logConsole("Device Type: Communicator");
  return DeviceTypeCommunicator;
}

bool isAdmin () {
  /*if (!digitalRead(CLUSTER_ADMIN_PIN)) {
    logConsole("This is an admin device");
    return true;
  }

  logConsole("This is a non-admin device");*/
  return false;
}

void buttonAPressed () {
  // dial decides whether remote or live image requested
  if (controlMode->isInteractive()) {
    ((GuiControlMode*)controlMode)->buttonAPressed();
  }
}

void handleRotary () {
  // dial decides whether remote or live image requested
  if (controlMode->isInteractive()) {
    ((GuiControlMode*)controlMode)->handleRotary();
  }
}

void loop() {
  controlMode->loop();
}


void logConsole (String msg) {
  Serial.println(msg);
}