#include "ThermalBackpack.h"

/*
        Chatter* chatter;
        Display* display;
        ThermalEncoder* encoder;
        Camera* camera;
        bool running = false;
*/

bool ThermalBackpack::init () {
    if (chatter->getDeviceStore()->getCustomPreference(StoredPrefBackpacksEnabled) == 'T') {
        if (chatter->getDeviceStore()->getCustomPreference(StoredPrefBackpackThermalEnabled) == 'T') {
            display->showStatus("Thermal", BrightGreen);

            encoder = new ThermalEncoder(THERMAL_HEIGHT, THERMAL_WIDTH, false);      
            camera = new Camera();
            if (camera->isReady()) {
                logConsole("Thermal ready");
                running = true;

                remoteEnabled = chatter->getDeviceStore()->getCustomPreference(StoredPrefBackpackThermalRemoteEnabled) == 'T';
                autoEnabled = chatter->getDeviceStore()->getCustomPreference(StoredPrefBackpackThermalAutoEnabled) == 'T';
            }
            else {
                logConsole("No Thermal!");
            }

        }
    }

    return running;
}

bool ThermalBackpack::isRunning () {
    return running;
}

bool ThermalBackpack::handleUserEvent (CommunicatorEventType eventType) {
  switch(eventType) {
    case UserThermalSnap:
    case UserPressActionButton:
        // show thermal on screen
        logConsole("User snapped thermal");
        control->setInteractiveContext(InteractiveThermal);
        display->clearMessageArea();
        control->showButtons();
        camera->captureImage();
        display->showInterpolatedThermal(camera->getImageData(), false, "Onboard Thermal");
        return true;
    case UserThermalSend:
        if (control->promptSelectDevice()) {
            // send it to the user
            if(encoder->encode (camera->getImageData())) {
                control->setCurrentDeviceFilter(control->getSelectedDeviceId());
                if (control->sendMessage(
                    control->getSelectedDeviceId(), 
                    encoder->getEncodeDecodeBuffer(), 
                    encoder->getEncodedBufferLength(), 
                    MessageTypeThermal)) {
                    logConsole("Thermal backpack sent message");
                    control->requestFullRepaint();

                    return true;
                }
            }
        }
  }

  return false;
}

bool ThermalBackpack::handleMessage (const uint8_t* message, int messageSize, const char* senderId, const char* recipientId) {
    logConsole("Thermal handleMessage not implemented");
    return false;
}
