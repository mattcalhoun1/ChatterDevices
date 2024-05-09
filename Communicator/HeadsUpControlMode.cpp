#include "HeadsUpControlMode.h"

StartupState HeadsUpControlMode::init () {
    encoder = new ThermalEncoder(THERMAL_HEIGHT, THERMAL_WIDTH, false);

    // if display hasn't already been set
    if (display == nullptr) {
      // this is where a different display could be initialized if desired

      fullyInteractive = true;
      display = new Display_TFT(encoder);
      logConsole("Display created");



      // attach keyboard to display
      Keyboard* keyboard = new Keyboard((TouchEnabledDisplay*)display);
      logConsole("keyboard created");
      keyboard->init();
      logConsole("keyboard initialized");
      ((FullyInteractiveDisplay*)display)->setDefaultKeyboard(keyboard);
    }

    // make sure parent is initialized
    return CommunicatorControlMode::init();
}

void HeadsUpControlMode::updateChatDashboard () {
  updateChatDashboard(false);
}


void HeadsUpControlMode::updateChatDashboard (bool forceRepaint) {
  //int screenWidth = deviceType == DeviceTypeCommunicatorMini ? 170 : 240;
  //int dashboardY = 280;
  // paint white bar across bottom
  //display->clearArea(0, dashboardY , screenWidth, 16, Gray);

  bool statusChanged = false;
  for (int channelNum = 0; channelNum < chatter->getNumChannels(); channelNum++) {
    ChatStatus thisStatus = chatter->getChatStatus(channelNum);
    if (thisStatus != lastChannelStatus[channelNum] || forceRepaint) {
      lastChannelStatus[channelNum] = thisStatus;
      statusChanged = true;
      if (thisStatus != ChatNoDevice) {
        sprintf(dashboardChannels[channelNum], "%s@%s", chatter->getChannel(channelNum)->getName(), chatter->getChannel(channelNum)->getConfigName());
          //dashboardChannels[channelNum] = chatter->getChannel(channelNum)->getName();
          dashboardColors[channelNum] = getDisplayColorForStatus(thisStatus);
      }
      else {
          sprintf(dashboardChannels[channelNum], "%s", channelUnknown);
          dashboardColors[channelNum] = getDisplayColorForStatus(thisStatus);
      }
    }
  }

  if (statusChanged) {
    display->clearDashboard();
    display->showDashboardItems((const char*)dashboardChannels, dashboardColors, chatter->getNumChannels());
  }
}

int HeadsUpControlMode::getSymboForDevice(const char* deviceName) {
    if (strcmp(deviceName, "LoRa") == 0) {
        return 0xF3;
    }
    else if (strcmp(deviceName, "UDP") == 0) {
        return 0x7F;
    }
    return 0x3F;
}

DisplayColor HeadsUpControlMode::getDisplayColorForStatus (ChatStatus chatStatus) {
    switch (chatStatus) {
        case ChatConnected:
            return LightGreen;
        case ChatConnecting:
            return LightGray;
        case ChatDisconnected:
            return Black;
        case ChatFailed:
            return Red;
        case ChatNoDevice:
            return White;
        case ChatReceived:
            return LightBlue;
        case ChatReceiving:
            return Magenta;
        case ChatSending:
            return Cyan;
        case ChatSent:
            return LightGreen;
        default:
            return Black;
    }
}


void HeadsUpControlMode::showStatus(const char* status) {
  if (strcmp(status, lastStatus) != 0) {
    strncpy(lastStatus, status, 23); // up to 23
    lastStatus[23] = 0;// make sure last char is always a term
    display->showStatus(lastStatus, BrightGreen);
  }
}

void HeadsUpControlMode::showStatus(String status) {
  showStatus(status.c_str());
}

void HeadsUpControlMode::showTitle(const char* title) {
    display->showTitle(title);
}

void HeadsUpControlMode::showTime() {
    // only update time every second at most
    if (millis() - lastTimeUpdate > 1000) {
        const char* latestTime = rtc->getViewableTime();
        display->showSubtitle(latestTime);
        lastTimeUpdate = millis();
    }
}

void HeadsUpControlMode::showBusy () {
    showStatus("Busy");
}

void HeadsUpControlMode::showReady () {
    showStatus("Ready");
}

void HeadsUpControlMode::showClusterOk () {
    showStatus("Success");
}

void HeadsUpControlMode::showClusterError () {
    showStatus("Error");
}

void HeadsUpControlMode::showClusterOnboard () {
    showStatus("Onboard");
}

void HeadsUpControlMode::loop () {
    CommunicatorControlMode::loop();
}

bool HeadsUpControlMode::isPreferenceEnabled (CommunicatorPreference pref) {
  switch (pref) {
    case PreferenceMessageHistory:
      return chatter->getDeviceStore()->getMessageHistoryEnabled();
    case PreferenceKeyboardLandscape:
      return chatter->getDeviceStore()->getKeyboardOrientedLandscape();
    case PreferenceWifiEnabled:
      return chatter->getDeviceStore()->getWifiEnabled();
    case PreferenceMeshEnabled:
      return chatter->getDeviceStore()->getMeshEnabled();
    case PreferenceWiredEnabled:
      return chatter->getDeviceStore()->getUartEnabled();
    case PreferenceLoraEnabled:
      return chatter->getDeviceStore()->getLoraEnabled();
  }

  logConsole("Unknown preference read attempt");
  return false;
}

void HeadsUpControlMode::enablePreference (CommunicatorPreference pref) {
  switch (pref) {
    case PreferenceMessageHistory:
      chatter->getDeviceStore()->setMessageHistoryEnabled(true);
      logConsole("Message history enabled");

      // reset device
      restartDevice();

      break;
    case PreferenceKeyboardLandscape:
      chatter->getDeviceStore()->setKeyboardOrientedLandscape(true);
      if (display->isTouchEnabled()) {
        // set keyboard orientation
        ((TouchEnabledDisplay*)display)->setKeyboardOrientation(Landscape);
      }

      logConsole("Keyboard landscape enabled");
      break;
    case PreferenceWifiEnabled:
      chatter->getDeviceStore()->setWifiEnabled(true);
      logConsole("Wifi enabled");

      // reset device
      restartDevice();

      break;
    case PreferenceMeshEnabled:
      chatter->getDeviceStore()->setMeshEnabled(true);
      logConsole("Mesh enabled");

      // reset device
      restartDevice();

      break;
    case PreferenceWiredEnabled:
      chatter->getDeviceStore()->setUartEnabled(true);
      logConsole("Wired enabled");
      // reset device
      restartDevice();
      break;
    case PreferenceLoraEnabled:
      chatter->getDeviceStore()->setLoraEnabled(true);
      logConsole("Lora enabled");
      // reset device
      restartDevice();
      break;
    
    default:
      logConsole("Unknown preference enable attempt");
  }
}

void HeadsUpControlMode::disablePreference (CommunicatorPreference pref) {
  switch (pref) {
    case PreferenceMessageHistory:
      chatter->getDeviceStore()->setMessageHistoryEnabled(false);
      logConsole("Message history disabled");

      // reset device
      restartDevice();
      break;
    case PreferenceKeyboardLandscape:
      chatter->getDeviceStore()->setKeyboardOrientedLandscape(false);
      if (display->isTouchEnabled()) {
        // set keyboard orientation
        ((TouchEnabledDisplay*)display)->setKeyboardOrientation(Portrait);
      }

      logConsole("Keyboard landscape disabled");
      break;
    case PreferenceWifiEnabled:
      chatter->getDeviceStore()->setWifiEnabled(false);
      logConsole("Wifi disabled");

      // reset device
      restartDevice();

      break;
    case PreferenceMeshEnabled:
      chatter->getDeviceStore()->setMeshEnabled(false);
      logConsole("Mesh disabled");

      // reset device
      restartDevice();

      break;
    case PreferenceLoraEnabled:
      chatter->getDeviceStore()->setLoraEnabled(false);
      logConsole("Lora disabled");

      // reset device
      restartDevice();

      break;
    case PreferenceWiredEnabled:
      chatter->getDeviceStore()->setUartEnabled(false);
      logConsole("Wired disabled");

      // reset device
      restartDevice();

      break;
    default:
      logConsole("Unknown preference disable attempt");
  }
}
