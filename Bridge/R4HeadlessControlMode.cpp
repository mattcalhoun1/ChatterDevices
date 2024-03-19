#if defined(ARDUINO_UNOR4_WIFI)

#include "R4HeadlessControlMode.h"

bool R4HeadlessControlMode::init () {
    logConsole("R4 Headless Control Mode Initializing");

    ledMatrix.begin();
    //matrixFrame.renderBitmap(matrixFrame, 8, 12);

    // make sure parent is initialized
    return HeadlessControlMode::init();
}

void R4HeadlessControlMode::showBusy() {
    ledMatrix.loadFrame(ConnectivityMatrixAnimation[StatusBusy]);
}

uint8_t R4HeadlessControlMode::getFrameFor (ChatStatus loraStatus, ChatStatus wifiStatus) {

    /* --- Status frames ----
        WifiDownLoraDown = 0,
        WifiUpLoraUp = 1,
        WifiDownLoraUp = 2,
        WifiUpLoraDown = 3,
        Unknown1 = 4,
        Unknown2 = 5,
        WifiUploading = 6,
        WifiDownloading = 7,
        WifiSuccess = 8,
        WifiFail = 9,
        LoraUploading = 10,
        LoraDownloading = 11,
        LoraSuccess = 12,
        LoraFail = 13
    
    */


    /* -- different chat statuses --
        ChatDisconnected = 0,
        ChatConnecting = 1,
        ChatConnected = 2,
        ChatReceiving = 3,
        ChatReceived = 4,
        ChatSending = 5,
        ChatSent = 6,
        ChatFailed = 7,
        ChatNoDevice = 8        
    */

    // lora activity
    if (loraStatus == ChatReceiving || loraStatus == ChatReceived) {
        return LoraDownloading;
    }
    else if (loraStatus == ChatSending) {
        return LoraUploading;
    }
    else if (loraStatus == ChatSent) {
        return LoraSuccess;
    }
    else if (loraStatus == ChatFailed) {
        return LoraFail;
    }

    // wifi activity
    if (wifiStatus == ChatReceiving || wifiStatus == ChatReceived) {
        return WifiDownloading;
    }
    else if (wifiStatus == ChatSending) {
        return WifiUploading;
    }
    else if (wifiStatus == ChatSent) {
        return WifiSuccess;
    }
    else if (wifiStatus == ChatFailed) {
        return WifiFail;
    }

    // general connectivity
    if (loraStatus == ChatConnected) {
        if (wifiStatus == ChatConnected) {
            return WifiUpLoraUp;
        }
        else if (wifiStatus == ChatDisconnected || wifiStatus == ChatConnecting || wifiStatus == ChatNoDevice) {
            return WifiDownLoraUp;
        }
    }
    else if (wifiStatus == ChatConnected) {
        if (loraStatus == ChatDisconnected || loraStatus == ChatConnecting || loraStatus == ChatNoDevice) {
            return WifiUpLoraDown;
        }
    }

    return Unknown2;

}

void R4HeadlessControlMode::updateChatDashboard () {
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

    // uart status is mapped to whichever other (lora or wifi) is NOT onboard
    if (loraStatus == ChatNoDevice) {
        ledMatrix.loadFrame(ConnectivityMatrixAnimation[getFrameFor(uartStatus, wifiStatus)]);
    }
    else if (wifiStatus == ChatNoDevice) {
        ledMatrix.loadFrame(ConnectivityMatrixAnimation[getFrameFor(loraStatus, uartStatus)]);
    }
    else {
        ledMatrix.loadFrame(ConnectivityMatrixAnimation[getFrameFor(loraStatus, wifiStatus)]);
    }
}

void R4HeadlessControlMode::showClusterOk () {
    ledMatrix.begin();
    ledMatrix.loadFrame(ConnectivityMatrixAnimation[ClusterStatusOk]);    
}

void R4HeadlessControlMode::showClusterError () {
    ledMatrix.begin();
    ledMatrix.loadFrame(ConnectivityMatrixAnimation[ClusterStatusError]);    
}

void R4HeadlessControlMode::showClusterOnboard () {
    ledMatrix.begin();
    ledMatrix.loadFrame(ConnectivityMatrixAnimation[ClusterOnboard]);    
}

#endif