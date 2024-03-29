#include "HeadsUpControlMode.h"

bool HeadsUpControlMode::init () {
    encoder = new ThermalEncoder(THERMAL_HEIGHT, THERMAL_WIDTH, false);

    // if display hasn't already been set
    if (display == nullptr) {
        if (deviceType == DeviceTypeCommunicatorMini) {
            display = new Display_7789(encoder);
        }
        else {
            fullyInteractive = true;
            display = new Display_TFT(encoder);
            // attach keyboard to display
            Keyboard* keyboard = new Keyboard((TouchEnabledDisplay*)display);
            keyboard->init();
            ((FullyInteractiveDisplay*)display)->setDefaultKeyboard(keyboard);
        }
    }

    // make sure parent is initialized
    return CommunicatorControlMode::init();
}

void HeadsUpControlMode::updateChatDashboard () {
  //int screenWidth = deviceType == DeviceTypeCommunicatorMini ? 170 : 240;
  //int dashboardY = 280;
  // paint white bar across bottom
  //display->clearArea(0, dashboardY , screenWidth, 16, Gray);
  display->clearDashboard();

  for (int channelNum = 0; channelNum < chatter->getNumChannels(); channelNum++) {
    if (chatter->getChatStatus(channelNum) != ChatNoDevice) {
        dashboardChannels[channelNum] = chatter->getChannel(channelNum)->getName();
        dashboardColors[channelNum] = getDisplayColorForStatus(chatter->getChatStatus(channelNum));
    }
    else {
        dashboardChannels[channelNum] = channelUnknown;
        dashboardColors[channelNum] = getDisplayColorForStatus(chatter->getChatStatus(channelNum));
    }
  }

  display->showDashboardItems(dashboardChannels, dashboardColors, chatter->getNumChannels());
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
            return Green;
        case ChatConnecting:
            return Yellow;
        case ChatDisconnected:
            return Black;
        case ChatFailed:
            return Red;
        case ChatNoDevice:
            return White;
        case ChatReceived:
            return Blue;
        case ChatReceiving:
            return Magenta;
        case ChatSending:
            return Cyan;
        case ChatSent:
            return Green;
        default:
            return Black;
    }
}


void HeadsUpControlMode::showStatus(const char* status) {
    display->showStatus(status, Blue);
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