#include "HeadsUpControlMode.h"

StartupState HeadsUpControlMode::init () {
    encoder = new ThermalEncoder(THERMAL_HEIGHT, THERMAL_WIDTH, false);

    // if display hasn't already been set
    if (display == nullptr) {
      // this is where a different display could be initialized if desired

      fullyInteractive = true;
      display = new Display_TFT(encoder);

      // attach keyboard to display
      Keyboard* keyboard = new Keyboard((TouchEnabledDisplay*)display);
      keyboard->init();
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

  if (statusChanged && !screenLocked) {
    display->clearDashboard();
    display->showDashboardItems((const char*)dashboardChannels, dashboardColors, chatter->getNumChannels());
  }
}

void HeadsUpControlMode::showStartupScreen (float progress) {
  display->showStartupScreen(progress);
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
  if (strcmp(status, lastStatus) != 0 && !screenLocked) {
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
    if (millis() - lastTimeUpdate > 1000 && !screenLocked) {
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

