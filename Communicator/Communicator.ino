#include "src/globals/Globals.h" // load first, to make sure settings propegate into chatter library
#include <Arduino.h>
#include <stdint.h>
#include "src/control/ControlMode.h"
#include "src/control/CommunicatorControlMode.h"
#include "src/control/HeadlessControlMode.h"
#include "src/control/GuiControlMode.h"
#include "src/control/TestControlMode.h"

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

  if (BACKPACK_ENABLED) {
    logConsole("connecting to backpack");
    start = millis();
    Serial1.begin(9600);
    // if log enabled, wait up to 5 sec for backpack to become available
    while (!Serial1) {
      if (start + 5000 < millis()) {
        break;
      }
      delay(10);
    } 
    if (Serial1) {
      logConsole("Connected to backpack!");
      Serial1.print("HELO!");
      Serial1.print("\n");
    }
    else {
      logConsole("No backpack connected");
    }
    delay(100);
  }



  // the bridge type test pins
  //pinMode(CLUSTER_ADMIN_PIN, INPUT_PULLUP);
  //pinMode(DEVICE_TYPE_PIN_MINI, INPUT_PULLUP);
  
  #ifdef FACTORY_RESET_PIN
  pinMode(FACTORY_RESET_PIN, INPUT_PULLUP);
  #endif

  pinMode(DISPLAY_TFT_BACKLIGHT, OUTPUT);

  #ifdef BUTTON_ACTION_PIN
    attachInterrupt(digitalPinToInterrupt(BUTTON_ACTION_PIN), buttonInterrupt, LOW);
  #endif


  #ifdef ROTARY_ENABLED
    pinMode(BUTTON_A_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_A_PIN), buttonInterrupt, LOW);
  #endif

  //Wire.setClock(400000L); // enc chip wants 400khz
  Wire.begin();

  SPI.begin();

  //controlMode = new GuiControlMode(selectDeviceType());
  //#if defined (ARDUINO_FEATHER_M4) || defined(ADAFRUIT_FEATHER_M4_EXPRESS)
  //controlMode = new TestControlMode(selectDeviceType());
  //#else
  controlMode = new GuiControlMode(selectDeviceType());
  //#endif

  StartupState startupState = controlMode->init();

  // register interrupt routine
  #ifdef ROTARY_ENABLED
    attachInterrupt(digitalPinToInterrupt(PIN_ROTARY_IN1), handleRotary, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ROTARY_IN2), handleRotary, CHANGE);
  #endif

  #if defined(TOUCH_CONTROL_RAK) || defined(TOUCH_CONTROL_ADAFRUIT_28) || defined(TOUCH_CONTROL_ADAFRUIT_35)
    pinMode(PIN_TOUCH_INT, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_TOUCH_INT), handleTouch, FALLING);
  #endif

  controlMode->beginInteractiveIfPossible();

  if (startupState == StartupInitializeDevice) {
    logConsole("Startup returned initialize new device");
    controlMode->initializeNewDevice();
  }
  else if (startupState == StartupError) {
    logConsole("Startup returned error");
    controlMode->handleStartupError();
  }
  else if (startupState == StartupUnlicensed) {
    logConsole("Startup returned unlicensed");
    controlMode->handleUnlicensedDevice();
  }
  if(startupState != StartupComplete) {
    logConsole("Error initializing!");
  }
}

DeviceType selectDeviceType () {
  logConsole("Device Type: Communicator");
  return DeviceTypeCommunicator;
}

void buttonInterrupt () {
  // dial decides whether remote or live image requested
  if (controlMode->isInteractive()) {
    ((GuiControlMode*)controlMode)->buttonInterrupt();
  }
}

void handleRotary () {
  // dial decides whether remote or live image requested
  if (controlMode->isInteractive()) {
    ((GuiControlMode*)controlMode)->handleRotaryInterrupt();
  }
}

void handleTouch () {
  if (controlMode->isInteractive()) {
    ((GuiControlMode*)controlMode)->touchInterrupt();
  }
}

void loop() {
  if (BACKPACK_ENABLED) {
    // check if info from backpack
    if (Serial1.available()) {
      while (Serial1.available()) {
        Serial.print((char)Serial1.read());
      }
      //Serial1.clear();
      Serial.print('\n');
    }
    Serial1.print("!"); // just keep alive
  }
  controlMode->loop();
}


void logConsole (String msg) {
  Serial.println(msg);
}