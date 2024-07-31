#include "HeadlessControlMode.h"

StartupState HeadlessControlMode::init () {
    logConsole("Headless Control Mode Initializing");

    //ledMatrix.begin();
    //matrixFrame.renderBitmap(matrixFrame, 8, 12);

    // make sure parent is initialized
    return CommunicatorControlMode::init();
}

void HeadlessControlMode::showBusy() {
    //ledMatrix.loadFrame(ConnectivityMatrixAnimation[StatusBusy]);
}

void HeadlessControlMode::showReady() {
    //ledMatrix.loadFrame(ConnectivityMatrixAnimation[StatusBusy]);
}

void HeadlessControlMode::updateChatDashboard () {
    for (int channelNum = 0; channelNum < chatter->getNumChannels(); channelNum++) {
        ChatStatus status = chatter->getChatStatus(channelNum);
        const char* channelName = chatter->getChannel(channelNum)->getName();
        //logConsole("Cannel: " + String(chatter->getChannel(channelNum)->getName()) + " has status " + String(status));

        // if this channel is uploading or downloading, it automatically wipes the status
        if (strcmp(channelName, "WiFi") == 0) {
            // this is wifi
            wifiStatus = status;
        }
        else if (strcmp(channelName, "LoRa") == 0) {
            // this is lora
            loraStatus = status;
        }
        else if (strcmp(channelName, "UART") == 0) {
            uartStatus = status;
        }
    }
}

void HeadlessControlMode::showStatus(const char* status) {
    String statusWithId = String(chatter->getDeviceId()) + ": " + String(status);
    logConsole(statusWithId);
}

void HeadlessControlMode::showStatus(String status) {
  showStatus(status.c_str());
}

void HeadlessControlMode::showTitle(const char* title) {
    logConsole("**************************");
    logConsole(title);
    logConsole("**************************");
}

void HeadlessControlMode::showTime() {
    // only update time every second at most
    if (millis() - lastTimeUpdate > 1000) {
        const char* latestTime = rtc->getViewableTime();
        logConsole(latestTime);
        lastTimeUpdate = millis();
    }
}

void HeadlessControlMode::showClusterOk () {

}

void HeadlessControlMode::showClusterError () {

}

void HeadlessControlMode::showClusterOnboard () {

}
