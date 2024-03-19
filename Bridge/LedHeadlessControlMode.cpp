#include "LedHeadlessControlMode.h"

bool LedHeadlessControlMode::init() {
    if (matrix.begin()) {
        matrix.setRotation(0);
        return HeadlessControlMode::init();;
    }

    logConsole("Matrix not started");
    return false;
}

void LedHeadlessControlMode::showBusy() {
    matrix.clear();
    matrix.drawBitmap(4, 1, MatrixValues[MATRIX_BUSY], 8, 6, 128);
}

void LedHeadlessControlMode::showLedStatus (ChatStatus loraStatus, ChatStatus wifiStatus) {

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
        showIndividualChannelStatus('L', MATRIX_DOWN);
        //return LoraDownloading;
        return;
    }
    else if (loraStatus == ChatSending) {
        showIndividualChannelStatus('L', MATRIX_UP);
        //return LoraUploading;
        return;
    }
    else if (loraStatus == ChatSent) {
        showIndividualChannelStatus('L', MATRIX_CHECK);
        //return LoraSuccess;
        return;
    }
    else if (loraStatus == ChatFailed) {
        showIndividualChannelStatus('L', MATRIX_X);
        //return LoraFail;
        return;
    }

    // wifi activity
    if (wifiStatus == ChatReceiving || wifiStatus == ChatReceived) {
        //return WifiDownloading;
        showIndividualChannelStatus('W', MATRIX_DOWN);
        return;
    }
    else if (wifiStatus == ChatSending) {
        //return WifiUploading;
        showIndividualChannelStatus('W', MATRIX_UP);
        return;
    }
    else if (wifiStatus == ChatSent) {
        showIndividualChannelStatus('W', MATRIX_CHECK);
        //return WifiSuccess;
        return;
    }
    else if (wifiStatus == ChatFailed) {
        showIndividualChannelStatus('W', MATRIX_X);
        //return WifiFail;
        return;
    }

    // general connectivity
    if (loraStatus == ChatConnected) {
        if (wifiStatus == ChatConnected) {
            showBothChannelStatus('L', MATRIX_CONNECTED, 'W', MATRIX_CONNECTED);
            //return WifiUpLoraUp;
            return;
        }
        else if (wifiStatus == ChatDisconnected || wifiStatus == ChatConnecting || wifiStatus == ChatNoDevice) {
            showBothChannelStatus('L', MATRIX_CONNECTED, 'W', MATRIX_DISCONNECTED);
            //return WifiDownLoraUp;
            return;
        }
    }
    else if (wifiStatus == ChatConnected) {
        if (loraStatus == ChatDisconnected || loraStatus == ChatConnecting || loraStatus == ChatNoDevice) {
            showBothChannelStatus('L', MATRIX_DISCONNECTED, 'W', MATRIX_CONNECTED);
            //return WifiUpLoraDown;
            return;
        }
    }

    showBusy();
    //return Unknown2;

}

void LedHeadlessControlMode::showIndividualChannelStatus (char channelLetter, uint8_t matrixCode) {
    matrix.clear();
    matrix.setTextSize(1);
    matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
    matrix.setTextColor(50);
    matrix.setCursor(1,0);
    matrix.print(channelLetter);
    matrix.drawBitmap(8, 2, MatrixValues[matrixCode], 8, 6, 128);
}

void LedHeadlessControlMode::showBothChannelStatus (char channelLetter1, uint8_t matrixCode1, char channelLetter2, uint8_t matrixCode2) {
  matrix.clear();
  matrix.setTextSize(1);
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(50);
  matrix.setCursor(1,0);
  matrix.drawBitmap(0, 3, MatrixValues[matrixCode1], 8, 6, 128);
  matrix.print(channelLetter1);
  matrix.setCursor(9,0);
  matrix.drawBitmap(8, 3, MatrixValues[matrixCode2], 8, 6, 128);
  matrix.print(channelLetter2);

}


void LedHeadlessControlMode::updateChatDashboard () {
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
        showLedStatus(uartStatus, wifiStatus);
    }
    else if (wifiStatus == ChatNoDevice) {
        showLedStatus(loraStatus, uartStatus);
    }
    else {
        showLedStatus(loraStatus, wifiStatus);
    }
}

void LedHeadlessControlMode::showClusterOk () {
    matrix.clear();
    matrix.drawBitmap(4, 1, MatrixValues[MATRIX_CHECK], 8, 6, 128);
}

void LedHeadlessControlMode::showClusterError () {
    matrix.clear();
    matrix.drawBitmap(4, 1, MatrixValues[MATRIX_X], 8, 6, 128);
}

void LedHeadlessControlMode::showClusterOnboard () {
    matrix.clear();
    matrix.drawBitmap(4, 1, MatrixValues[MATRIX_KEY], 8, 6, 128);
}
