#include "src/globals/Globals.h" // load first, to make sure settings propegate into chatter library
#include <Arduino.h>
#include <stdint.h>
#include "src/control/ControlMode.h"
#include "src/control/CommunicatorControlMode.h"
#include "src/control/HeadlessControlMode.h"
#include "src/control/GuiControlMode.h"
#include "src/control/TestControlMode.h"
#include "ChatterLogging.h"
#include <AlmostRandom.h>

CommunicatorControlMode* controlMode;

void setup() {
  long start = millis();
  Serial.begin(9600);

  AlmostRandom ar;
  randomSeed(ar.getRandomInt());

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
  setupLogging();

  if (BACKPACK_ENABLED) {
    Logger::info("connecting to backpack", LogAppControl);
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
      Logger::info("Connected to backpack!", LogAppControl);
      Serial1.print("HELO!");
      Serial1.print("\n");
    }
    else {
      Logger::info("No backpack connected", LogAppControl);
    }
    delay(100);
  }
  
  // set up the various pin configs
  #ifdef FACTORY_RESET_PIN
  pinMode(FACTORY_RESET_PIN, INPUT_PULLUP);
  #endif

  pinMode(DISPLAY_TFT_BACKLIGHT, OUTPUT);

  //#ifdef BUTTON_ACTION_PIN
  //  attachInterrupt(digitalPinToInterrupt(BUTTON_ACTION_PIN), buttonInterrupt, LOW);
  //#endif

  #ifdef ROTARY_ENABLED
    pinMode(BUTTON_A_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_A_PIN), buttonInterrupt, LOW);
  #endif

  Wire.begin();
  SPI.begin();

  // setup the control mode
  if (CONTROL_MODE_GUI == true) {
    controlMode = new GuiControlMode(selectDeviceType());
  }
  else {
    controlMode = new TestControlMode(selectDeviceType());
  }

  // attempt to startup
  StartupState startupState = controlMode->init();

  // rotary and touch interrupts (if configured)
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
    Logger::info("Startup returned initialize new device", LogAppControl);
    controlMode->initializeNewDevice();
  }
  else if (startupState == StartupError) {
    Logger::error("Startup returned error", LogAppControl);
    controlMode->handleStartupError();
  }
  else if (startupState == StartupUnlicensed) {
    Logger::warn("Startup returned unlicensed", LogAppControl);
    controlMode->handleUnlicensedDevice();
  }
  if(startupState != StartupComplete) {
    Logger::error("Error initializing!", LogAppControl);
  }
}

DeviceType selectDeviceType () {
  Logger::info("Device Type: Communicator", LogAppControl);
  return DeviceTypeCommunicator;
}

void buttonInterrupt () {
  // pass button press through to control mode
  if (controlMode->isInteractive()) {
    ((GuiControlMode*)controlMode)->buttonInterrupt();
  }
}

void handleRotary () {
  // pass rotary movement through to control mode
  if (controlMode->isInteractive()) {
    ((GuiControlMode*)controlMode)->handleRotaryInterrupt();
  }
}

void handleTouch () {
  // pass touch notification through to control mode
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

  // control mode handles rest of the loop
  controlMode->loop();
}

void setupLogging () {
  Logger::init(new SerialLogger());

  Logger::setLogLevel (LogChatter, LogLevelDebug);
  Logger::setLogLevel (LogStorage, LogLevelInfo);
  Logger::setLogLevel (LogEncryption, LogLevelInfo);
  Logger::setLogLevel (LogComLora, LogLevelDebug);
  Logger::setLogLevel (LogComWiFi, LogLevelInfo);
  Logger::setLogLevel (LogComWired, LogLevelInfo);
  Logger::setLogLevel (LogMesh, LogLevelInfo);
  Logger::setLogLevel (LogMeshStrategy, LogLevelInfo);
  Logger::setLogLevel (LogLicensing, LogLevelInfo);
  Logger::setLogLevel (LogUi, LogLevelInfo);
  Logger::setLogLevel (LogUiEvents, LogLevelInfo);
  Logger::setLogLevel (LogAppControl, LogLevelInfo);
  Logger::setLogLevel (LogMeshGraph, LogLevelInfo);
  Logger::setLogLevel (LogLocation, LogLevelInfo);
  Logger::setLogLevel (LogRtc, LogLevelInfo);
  Logger::setLogLevel (LogOnboard, LogLevelInfo);
  Logger::setLogLevel (LogInit, LogLevelInfo);
  Logger::setLogLevel (LogBackup, LogLevelInfo); 
}