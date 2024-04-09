#include "ControlMode.h"

bool ControlMode::init() {

#if defined(ARDUINO_UNOR4_WIFI)
  rtc = new R4RtClock();
#elif defined(ARDUINO_SAM_DUE)
  rtc = new DueRtClock();
#elif defined(ARDUINO_SAMD_MKRZERO) || defined (ARDUINO_SAMD_NANO_33_IOT) || defined (ARDUINO_SAMD_MKRWAN1310) || defined (ARDUINO_SAMD_ZERO) || defined(ARDUINO_FEATHER_M4)
  logConsole("ZeroRtClock selected");
  rtc = new ZeroRtClock();
#endif

  if(rtc->isFunctioning()) {
    logConsole("RTC is OK");

    #if defined(STORAGE_FRAM_SPI)
      chatter = new Chatter(ChatterDeviceCommunicator, BasicMode, rtc, StorageFramSPI, this);
    #elif defined(STORAGE_FRAM_I2C)
      chatter = new Chatter(ChatterDeviceCommunicator, BasicMode, rtc, StorageFramI2C, this);
    #endif

    bool chatterReady = chatter->init();
    if (factoryResetCheck()) {
      return false;
    }

    if (chatterReady) {
      showStatus("Init LoRa...");
      chatter->addLoRaChannel(LORA_RFM9X_CS, LORA_RFM9X_INT, LORA_RFM9X_RST, LORA_CHANNEL_LOG_ENABLED);
      return true;
    }
    else {
      logConsole("Realtime clock not functioning!");
    }
  }

  // if we get down here, something didn't initialize properly
  logConsole("Error: init did not complete");
  showStatus("ERROR!");
  return false;
}

void ControlMode::updateChatDashboard () {
  for (int channelNum = 0; channelNum < chatter->getNumChannels(); channelNum++) {
    if (chatter->getChatStatus(channelNum) != ChatNoDevice) {
      logConsole(String(chatter->getChannel(channelNum)->getName()) + ": " + String(getStatusName(chatter->getChatStatus(channelNum))));
    }
    else {
      logConsole("Unknown: No Device");
    }
  }
}

  const char* ControlMode::getStatusName (ChatStatus chatStatus) {
    //logConsole("finding status for: " + String(chatStatus));
    switch (chatStatus) {
      case ChatDisconnected:
        return "Disconnected";
      case ChatConnecting:
        return "Connecting";
      case ChatConnected:
        return "Connected";
      case ChatReceiving:
        return "Receiving";
      case ChatReceived:
        return "Received";
      case ChatSending:
        return "Sending";
      case ChatSent:
        return "Sent";
      case ChatFailed:
        return "Failed";
      case ChatNoDevice:
        return "No Device";
    }
    return "Unknown";
  }

// chat status callback
void ControlMode::updateChatStatus (uint8_t channelNum, ChatStatus newStatus) {
  updateChatDashboard();
}

void ControlMode::updateChatStatus (const char* statusMessage) {
  showStatus(statusMessage);
}

void ControlMode::updateChatProgress (float progress) {
  logConsole("Progress...");
}

void ControlMode::resetChatProgress () {
  logConsole("Reset progress");
}

void ControlMode::showStatus (const char* status) {
  logConsole(status);
}

void ControlMode::loop () {
  logConsole("in loop...");
  delay(2000);
}


void ControlMode::showTime () {
  //rtc->populateTimeDateString(timeDate, true);
  logConsole(rtc->getViewableTime());
}

/*****************
 * Miscellaneous *
 *****************/
void ControlMode::sleepOrBackground(unsigned long sleepTime) {
  delay(sleepTime);
}

void ControlMode::logConsole(String msg) {
  if (LOG_ENABLED)
  {
    Serial.println("Log:" + msg);
  }
}

bool ControlMode::initializeNewDevice () {
  // by default, we just generate random stuff
  // if interactive, a subclass will handle this, and prompt for info
  ClusterAdmin* admin = new ClusterAdmin(chatter);
  char tempAlias[CHATTER_ALIAS_NAME_SIZE];
  memset(tempAlias, 0, CHATTER_ALIAS_NAME_SIZE);
  sprintf(tempAlias, "%s_%d", DEFAULT_DEVICE_ALIAS, millis()/100);
  return admin->genesisRandom(tempAlias);
}

bool ControlMode::factoryResetCheck (bool forceReset) {
  
  if (forceReset || !chatter->isDeviceInitialized()) {
    logConsole("Factory resetting...");
    chatter->factoryReset();
    if (initializeNewDevice()) {
      // probably need to add cleanup code to admin obj?
      justOnboarded = true;
      logConsole("Factory reset complete");

      // wait a few seconds, then reset the device
      delay(3000);
      restartDevice();

      //chatter->logDebugInfo();
      return true;
    }
    else {
      logConsole("New device init failed.");
    }
  }

  return false;
}

void ControlMode::restartDevice() {
  logConsole("Restarting");
  NVIC_SystemReset();
}

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int ControlMode::freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}