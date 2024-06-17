#include "ControlMode.h"

StartupState ControlMode::init() {

logConsole("Initializing clock");
#if defined(ARDUINO_UNOR4_WIFI)
  logConsole("Uno R4 clock selected");
  rtc = new R4RtClock();
#elif defined(ARDUINO_SAM_DUE)
  logConsole("Due clock selected");
  rtc = new DueRtClock();
#elif defined (ADAFRUIT_FEATHER_M4_EXPRESS)
  logConsole("Samd51RtClock selected");
  rtc = new Samd51RtClock();
#elif defined(ARDUINO_SAMD_MKRZERO) || defined (ARDUINO_SAMD_NANO_33_IOT) || defined (ARDUINO_SAMD_MKRWAN1310) || defined (ARDUINO_SAMD_ZERO) || defined(ARDUINO_FEATHER_M4)
  logConsole("ZeroRtClock selected");
  rtc = new ZeroRtClock();
#endif

  if(rtc->isFunctioning()) {
    logConsole("RTC Time: ", rtc->getViewableTime());

    #if defined(STORAGE_FRAM_SPI)
      chatter = new Chatter(ChatterDeviceCommunicator, BasicMode, rtc, StorageFramSPI, this, this, this);
    #elif defined(STORAGE_FRAM_I2C)
      chatter = new Chatter(ChatterDeviceCommunicator, BasicMode, rtc, StorageFramI2C, this, this, this);
    #endif

    bool chatterReady = chatter->init();
    if (!chatter->isDeviceLicensed ()) {
      return StartupUnlicensed;
    }
    else if (!chatter->isDeviceInitialized()) {
      return StartupInitializeDevice;
    }
    else if (factoryResetButtonHeld()) {
      return StartupInitializeDevice;
    }

    if (chatterReady) {
      #ifdef CHATTER_LORA_ENABLED
        if (chatter->getDeviceStore()->getLoraEnabled()) {
          showStatus("Init LoRa...");
          chatter->addLoRaChannel(LORA_RFM9X_CS, LORA_RFM9X_INT, LORA_RFM9X_RST, LORA_CHANNEL_LOG_ENABLED);
        }
        else {
          logConsole("Device LoRa disabled by user pref");
        }

      #endif

      #ifdef CHATTER_WIFI_ENABLED
        if (chatter->getDeviceStore()->getWifiEnabled()) {
          #if defined(CHATTER_ONBOARD_WIFI)
            chatter->addOnboardUdpChannel (UDP_CHANNEL_LOG_ENABLED);
          # else
            chatter->addAirliftUdpChannel (SPIWIFI_SS, SPIWIFI_ACK, ESP32_RESETN, ESP32_GPIO0, UDP_CHANNEL_LOG_ENABLED);
          #endif
        }
        else {
          logConsole("Device WiFi disabled by user pref");
        }
      #endif

      #ifdef CHATTER_UART_ENABLED
        if (chatter->getDeviceStore()->getUartEnabled()) {
          showStatus("Init UART...");
          chatter->addUartChannel(UART_CHANNEL_LOG_ENABLED);
        }
        else {
          logConsole("Device LoRa disabled by user pref");
        }

      #endif


      return StartupComplete;
    }
    else {
      logConsole("Realtime clock not functioning!");
    }
  }

  // if we get down here, something didn't initialize properly
  logConsole("Error: init did not complete");
  showStatus("ERROR!");
  return StartupError;
}

void ControlMode::handleStartupError() {
  logConsole("Startup error, no handler code!");
}

void ControlMode::handleUnlicensedDevice () {
  logConsole("No license!");
  while(true) {
    delay(50);
  }
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

void ControlMode::hideChatProgress () {
  logConsole("hide chat progress");  
}

void ControlMode::showStatus (const char* status) {
  logConsole(status);
}

void ControlMode::pingReceived (uint8_t deviceAddress) {
  logConsole("Ping received");  
}

void ControlMode::ackReceived(const char* sender, const char* messageId) {
  logConsole("ack received");
}

bool ControlMode::userInterrupted () {
  return false;
}


void ControlMode::resetBackupProgress() {
  logConsole("reset backup progress");
}

void ControlMode::updateBackupProgress (float pct) {
  logConsole("progress pct: " + String(pct));
}

uint8_t ControlMode::promptBackupPassword (char* buffer) {
  logConsole("prompt backup pw");
  return 0;
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

void ControlMode::logConsole(const char* msg) {
  if (LOG_ENABLED)
  {
    Serial.println(msg);
  }
}

void ControlMode::logConsole(const char* msg, const char* msg2) {
  if (LOG_ENABLED)
  {
    Serial.print(msg);
    Serial.println(msg2);
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

bool ControlMode::factoryResetCheck (bool forceReset, bool writeZeros) {
  
  if (forceReset || !chatter->isDeviceInitialized()) {
    logConsole("Factory resetting...");
    chatter->factoryReset(writeZeros);
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
    return false;
  }

  return false;
}

bool ControlMode::factoryResetButtonHeld() {
  #ifdef FACTORY_RESET_PIN
  // check if factory rest button being held
  if (!digitalRead(FACTORY_RESET_PIN)) {
    // pin needs to be held down for 3 seconds
    int resetMillis = 30000;
    bool held = true;
    for (uint8_t timerCount = 0; held && timerCount < resetMillis / 250; timerCount++) {
      logConsole("Button held, factory reset countdown");
      held = !digitalRead(FACTORY_RESET_PIN);
      delay(250);
    }
    if (held) {
      logConsole("Factory reset by button upon startup");
      //return true;
    }
  }
  #endif

  return false;

}

void ControlMode::restartDevice() {
  logConsole("Restarting");
  NVIC_SystemReset();
}

uint8_t ControlMode::promptLicenseKey (char* buffer) {
  logConsole("prompt for license key...");
  return 16;
}

void ControlMode::licenseValidation (bool isValid) {
  if (isValid) {
    logConsole("License IS valid");
  }
  else {
    logConsole("License is NOT valid");
  }
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