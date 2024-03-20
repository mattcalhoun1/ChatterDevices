#include "ControlMode.h"

bool ControlMode::init() {

#if defined(ARDUINO_UNOR4_WIFI)
  rtc = new R4RtClock();
#elif defined(ARDUINO_SAM_DUE)
  rtc = new DueRtClock();
#elif defined(ARDUINO_SAMD_MKRZERO) || defined (ARDUINO_SAMD_NANO_33_IOT)
  rtc = new ZeroRtClock();
#endif

  if(rtc->isFunctioning()) {
    logConsole("RTC is OK");

    ChatterDeviceType deviceType = ChatterDeviceBridgeLora;
    if (!digitalRead(BRIDGE_ID_PIN_WIFI)) {
      deviceType = ChatterDeviceBridgeWifi;
      logConsole("This is a Wifi Bridge");
    }

    #if defined(STORAGE_FRAM_SPI)
      chatter = new Chatter(deviceType, BridgeMode, rtc, StorageFramSPI, this);
    #elif defined(STORAGE_FRAM_I2C)
      chatter = new Chatter(deviceType, BridgeMode, rtc, StorageFramI2C, this);
    #endif
    bool chatterReady = chatter->init();
    if (factoryResetCheck(deviceType)) {
      return false;
    }
    else if (joinClusterCheck()) {
      return false;
    }

    if (chatterReady) {
      if (deviceType == ChatterDeviceBridgeLora) {
        showStatus("Init LoRa...");
        chatter->addLoRaChannel(LORA_RFM9X_CS, LORA_RFM9X_INT, LORA_RFM9X_RST, LORA_CHANNEL_LOG_ENABLED);

        showStatus("Init UART...");
        chatter->addUartChannel(UART_CHANNEL_LOG_ENABLED);
      }
      else if (deviceType == ChatterDeviceBridgeWifi) {
        showStatus("Init WiFi...");
        chatter->addAirliftUdpChannel(SPIWIFI_SS, SPIWIFI_ACK, ESP32_RESETN, ESP32_GPIO0, UDP_CHANNEL_LOG_ENABLED);
        //chatter->addOnboardUdpChannel(UDP_CHANNEL_LOG_ENABLED);

        showStatus("Init UART...");
        chatter->addUartChannel(UART_CHANNEL_LOG_ENABLED);
      }
      else {
        logConsole("ERROR: Unknown bridge device!");
        return false;
      }

      return true;
    }
  }
  else {
    logConsole("Realtime clock not functioning!");
  }

  // if we get down here, something didn't initialize properly
  showStatus("ERROR!");
  while(true) {
    delay(500);
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
    logConsole("finding status for: " + String(chatStatus));
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

bool ControlMode::factoryResetCheck (ChatterDeviceType deviceType) {
  if (!digitalRead(FACTORY_RESET_PIN)) {
    // pin needs to be held down for 3 seconds
    int resetMillis = 3000;
    bool held = true;
    for (uint8_t timerCount = 0; held && timerCount < resetMillis / 250; timerCount++) {
      logConsole("Button held, factory reset countdown");
      held = !digitalRead(FACTORY_RESET_PIN);
      delay(250);
    }
    if (held) {
      showBusy();
      logConsole("Factory resetting...");
      chatter->factoryReset();
      ClusterAdmin* admin = new ClusterAdmin(chatter);
      char tempAlias[CHATTER_ALIAS_NAME_SIZE];
      memset(tempAlias, 0, CHATTER_ALIAS_NAME_SIZE);
      if(deviceType == ChatterDeviceBridgeLora) {
        sprintf(tempAlias, "%s", BRIDGE_LORA_ALIAS);
      }
      else if (deviceType == ChatterDeviceBridgeWifi) {
        sprintf(tempAlias, "%s", BRIDGE_WIFI_ALIAS);
      }
      else if (deviceType == ChatterDeviceBridgeCloud) {
        sprintf(tempAlias, "%s", BRIDGE_CLOUD_ALIAS);
      }
      admin->genesisRandom(tempAlias);

      // probably need to add cleanup code to admin obj?
      justOnboarded = true;
      logConsole("Factory reset complete");

      chatter->logDebugInfo();
      return true;
    }
  }

  return false;
}

bool ControlMode::joinClusterCheck () {
  if (!digitalRead(JOIN_CLUSTER_PIN)) {
    // pin needs to be held down for 3 seconds
    int resetMillis = 3000;
    bool held = true;
    for (uint8_t timerCount = 0; held && timerCount < resetMillis / 250; timerCount++) {
      logConsole("Button held, join cluster countdown");
      held = !digitalRead(JOIN_CLUSTER_PIN);
      delay(250);
    }
    if (held) {
      showClusterOnboard();
      logConsole("Joining cluster...");
      BleClusterAssistant assistant = BleClusterAssistant(chatter);
      assistant.init();
      if (assistant.attemptOnboard()) {
        justOnboarded = true;
        return true;
      }
      else {
        return false;
      }
    }
  }

  return false;
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