#include "RelayBackpack.h"

bool RelayBackpack::init () {
    if (chatter->getDeviceStore()->getCustomPreference(StoredPrefBackpacksEnabled) == 'T') {
        if (chatter->getDeviceStore()->getCustomPreference(StoredPrefBackpackRelayEnabled) == 'T') {
            display->showStatus("Relay", BrightGreen);

            // set the rx pin for analog out
            pinMode(RELAY_OUT_PIN, OUTPUT);
            digitalWrite(RELAY_OUT_PIN, LOW);


            if (true) {
                logConsole("Relay ready");
                running = true;

                remoteEnabled = chatter->getDeviceStore()->getCustomPreference(StoredPrefBackpackRelayRemoteEnabled) == 'T';
            }
            else {
                logConsole("No relay!");
            }

        }
    }

    return running;
}

bool RelayBackpack::isRunning () {
    return running;
}

bool RelayBackpack::handleUserEvent (CommunicatorEventType eventType) {
  switch(eventType) {
    case UserPressActionButton:
        // show thermal on screen
        logConsole("User triggered relay via button");

        triggerRelay();

        return true;
  }

  return false;
}

bool RelayBackpack::handleMessage (const uint8_t* message, int messageSize, const char* senderId, const char* recipientId) {
    // only one option at the moment, return a current image
    if (remoteEnabled) {
        logConsole("Relay backpack handling remote request");
        display->showStatus("Relay remote", BrightYellow);
        
        triggerRelay();
    }
    else {
        logConsole("Remote relay requested, but not enabled in settings");
    }

    return false;
}


void RelayBackpack::triggerRelay() {
    // set high for 5 seconds, then set low
    digitalWrite(RELAY_OUT_PIN, HIGH);

    logConsole("Relay triggered");

    unsigned long startTime = millis();
    while (millis() - startTime < 5000) {
        delay(100);
    }
    digitalWrite(RELAY_OUT_PIN, LOW);

    logConsole("Relay untriggered");
}