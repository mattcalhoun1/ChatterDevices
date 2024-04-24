#include "ControlMode.h"
#include "HeadsUpControlMode.h"
#include "Display.h"
#include "TouchEnabledDisplay.h"
#include <RotaryEncoder.h>
#include "Menu.h"
#include "DeviceAliasIterator.h"
#include "MessageIterator.h"
#include "TestIterator.h"
#include "ItemIterator.h"
#include <SHA256.h>

#ifndef GUICONTROLMODE_H
#define GUICONTROLMODE_H

//                                status/etc   ts       :          alias               (           send/rcpient     )   sp  >>  term
#define MESSAGE_TITLE_BUFFER_SIZE 3 + STORAGE_TS_SIZE + 1 +  CHATTER_ALIAS_NAME_SIZE + 1 + CHATTER_DEVICE_ID_SIZE + 1 + 1 + 2 + 2

#define MESSAGE_PREVIEW_BUFFER_SIZE 32

class GuiControlMode : public HeadsUpControlMode, public TouchListener {
    public:
        GuiControlMode(DeviceType _deviceType) : HeadsUpControlMode (_deviceType) {}
        StartupState init ();
        void buttonInterrupt();
        void touchInterrupt();
        bool isInteractive () {return true;}
        void beginInteractiveIfPossible();

        void showLastMessage ();

        // these are called via interrupt, so should not do anything update flags
        void handleRotaryInterrupt ();

        bool updateSelection ();
        int getSelection () {return selection;}
        void setSelection (int newSelection) {selection = newSelection;}
        void loop ();

        bool handleEvent (CommunicatorEventType eventType);
        bool handleEvent (CommunicatorEvent* event);

        bool initializeNewDevice ();

        bool onboardNewClient (unsigned long timeout);

        bool sendDirectMessage ();
        bool sendViaBridge ();
        bool sendViaMesh ();

        bool handleScreenTouched (int touchX, int touchY);

        uint8_t promptForPassword (char* passwordBuffer, uint8_t maxPasswordLength);
        uint8_t promptForPassword (char* passwordBuffer, uint8_t maxPasswordLength, bool updateHash);

        void promptFactoryReset();
        bool promptYesNo (const char* message);

        void updateChatProgress (float progress);
        void resetChatProgress ();
        void hideChatProgress ();
        void updateMeshCacheUsed (float percent);


        void handleStartupError ();

    protected:
        bool updateMessagePreviewsIfNecessary ();
        void showMessageHistory(bool resetOffset);
        void showButtons ();
        void sleepOrBackground (unsigned long sleepTime);

        bool attemptDirectSend ();
        void lockScreen ();
        void unlockScreen ();

    private:

        bool screenLocked = false;
        bool fullRepaint = false;
        uint8_t lastChannel = 0;
        RotaryEncoder *rotary = nullptr;
        Menu* menu;

        char title[32];

        //ChatterAdmin* admin;
        int selection = 0;
        bool rotaryMoved = false;

        // for onboarding input
        char newDeviceAlias[CHATTER_ALIAS_NAME_SIZE+2];
        char newClusterAlias[CHATTER_ALIAS_NAME_SIZE+1];
        char newDevicePassword[CHATTER_PASSWORD_MAX_LENGTH + 1];
        char newDeviceWifiSsid[WIFI_SSID_MAX_LEN + 1];
        char newDeviceWifiCred[WIFI_CRED_MAX_LEN + 1];
        char newDeviceWifiPreferrred[2];
        char newFreq[7];

        uint8_t passwordHash[32];
        uint8_t unlockPasswordHash[32];
        SHA256 hasher;

        ItemIterator* deviceIterator;
        ItemIterator* messageIterator;
        uint8_t messagePreviewOffset = 0;
        uint8_t messageHistorySize = 0;

        char messageTitleBuffer[MESSAGE_TITLE_BUFFER_SIZE + 1];
        char messagePreviewBuffer[MESSAGE_PREVIEW_BUFFER_SIZE + 1];
        char messageTsBuffer[12];

        CommunicatorEvent eventBuffer;
        char histSenderId[CHATTER_DEVICE_ID_SIZE+1];
        char histRecipientId[CHATTER_DEVICE_ID_SIZE+1];

        unsigned long tickFrequency = 3000; // how often the ticker should blink
        unsigned long lastTick = 0;
};

#endif