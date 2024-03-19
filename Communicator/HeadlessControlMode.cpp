#include "HeadlessControlMode.h"

bool HeadlessControlMode::init () {
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

/*
void HeadlessControlMode::updateChatStatusBuffer(ChatStatus chatStatus) {
    switch (chatStatus) {
        case ChatDisconnected:
            memcpy(chatStatusBuffer, "Disconnected", 12);
            chatStatusBuffer[12] = '\0';
            break;
        case ChatConnecting:
            memcpy(chatStatusBuffer, "Connecting", 10);
            chatStatusBuffer[10] = '\0';
            break;
        case ChatConnected:
            memcpy(chatStatusBuffer, "Connected", 9);
            chatStatusBuffer[9] = '\0';
            break;
        case ChatReceiving:
            memcpy(chatStatusBuffer, "Receiving", 9);
            chatStatusBuffer[9] = '\0';
            break;
        case ChatReceived:
            memcpy(chatStatusBuffer, "Received", 8);
            chatStatusBuffer[8] = '\0';
            break;
        case ChatSending:
            memcpy(chatStatusBuffer, "Sending", 7);
            chatStatusBuffer[7] = '\0';
            break;
        case ChatSent:
            memcpy(chatStatusBuffer, "Sent", 4);
            chatStatusBuffer[4] = '\0';
            break;
        case ChatFailed:
            memcpy(chatStatusBuffer, "Failed", 6);
            chatStatusBuffer[6] = '\0';
            break;
        case ChatNoDevice:
            memcpy(chatStatusBuffer, "No Device", 9);
            chatStatusBuffer[9] = '\0';
            break;
        default:
            memcpy(chatStatusBuffer, "Unknown", 7);
            chatStatusBuffer[7] = '\0';
    }
}
*/

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
