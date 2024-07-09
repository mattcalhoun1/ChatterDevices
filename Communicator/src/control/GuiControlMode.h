#include "ControlMode.h"
#include "HeadsUpControlMode.h"
#include "InteractiveControlMode.h"
#include "../display/Display.h"
#include "../display/TouchEnabledDisplay.h"
#include <RotaryEncoder.h>
#include "../menu/Menu.h"
#include "../iterator/DeviceAliasIterator.h"
#include "../iterator/MessageIterator.h"
#include "../iterator/TestIterator.h"
#include "../iterator/ItemIterator.h"
#include "../iterator/NearbyDeviceIterator.h"
#include "../iterator/ClusterAliasIterator.h"
#include <SHA256.h>

// temp: thermal
//#include "../backpacks/thermal/Camera.h"
#include "../backpacks/thermal/ThermalEncoder.h"
#include "../backpacks/Backpack.h"
#include "../backpacks/thermal/ThermalBackpack.h"

#include <qrcode.h>

#ifndef GUICONTROLMODE_H
#define GUICONTROLMODE_H

//                                status    channels            ts       :          alias               (           send/rcpient     )   sp  >>  term
#define MESSAGE_TITLE_BUFFER_SIZE 4 +        2 + STORAGE_TS_SIZE + 1 +  CHATTER_ALIAS_NAME_SIZE + 1 + CHATTER_DEVICE_ID_SIZE + 1 + 1 + 2 + 2

#define MESSAGE_PREVIEW_BUFFER_SIZE 64

enum MessageSendResult {
    MessageSentDirect = 0,
    MessageSentMesh = 1,
    MessageSentBridge = 2,
    MessageNotSent = 3
};


class GuiControlMode : public HeadsUpControlMode, public TouchListener, public InteractiveControlMode {
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
        bool userInterrupted();

        void updateMeshCacheUsed (float percent);
        void pingReceived (uint8_t deviceAddress);

        void updateBackupProgress (float pct);
        uint8_t promptBackupPassword (char* buffer);
        void resetBackupProgress ();

        void handleStartupError ();
        void handleUnlicensedDevice ();

        void showButtons();

        bool promptSelectDevice ();
        const char* getSelectedDeviceId() { return otherDeviceId; }
        void setCurrentDeviceFilter(const char* deviceFilter);
        bool sendMessage (const char* deviceId, uint8_t* msg, int msgSize, MessageType _type);
        void requestFullRepaint () { fullRepaint = true; }


    protected:
        void refreshDisplayContext(bool fullRefresh);
        bool updatePreviewsIfNecessary ();
        void showMessageHistory(bool resetOffset);
        void showNearbyDevices(bool resetOffset);
        void sleepOrBackground (unsigned long sleepTime);

        void notifyMessageReceived();

        MessageSendResult attemptDirectSend ();
        void lockScreen ();
        void unlockScreen ();
        void showMeshPath (const char* recipientId);
        bool promptSelectCluster ();
        void promptUserNewTime();
        bool validateDatePart(int partVal, uint8_t partNum);

        bool promptClusterInfo (bool forceInput);
        bool createNewCluster ();
        void showTime(bool forceRepaint);
        void showTime();

        bool isLearningEnabled();
        void enableLearning (unsigned long learnDurationMillis);
        void disableLearning ();

        void showQrCode (bool isLicensed = false);
        bool promptLicense ();

        void changeUserPassword ();

        ThermalEncoder* encoder;
        //Camera* camera;
        Backpack* backpacks[MAX_BACKPACKS];
        uint8_t numBackpacks = 0;
        Backpack* getBackpack (BackpackType type);

    private:
        bool awaitingLicense = false;
        bool fullRepaint = false;
        bool unreadMessage = false;

        uint8_t lastChannel = 0;
        RotaryEncoder *rotary = nullptr;
        Menu* menu;

        char title[32];
        char lastTime[16];

        //ChatterAdmin* admin;
        int selection = 0;
        bool rotaryMoved = false;

        // for onboarding input
        char newDeviceAlias[CHATTER_ALIAS_NAME_SIZE+2];
        char newClusterAlias[CHATTER_ALIAS_NAME_SIZE+1];
        char newDevicePassword[CHATTER_PASSWORD_MAX_LENGTH + 1];
        char newDevicePasswordRetyped[CHATTER_PASSWORD_MAX_LENGTH + 1];
        char newDeviceWifiSsid[WIFI_SSID_MAX_LEN + 1];
        char newDeviceWifiCred[WIFI_CRED_MAX_LEN + 1];
        char newDeviceWifiPreferrred[2];
        char newFreq[7];
        float newFrequency = LORA_DEFAULT_FREQUENCY;
        bool newWifiEnabled = false;
        bool newWifiPreferred = false;

        uint8_t passwordHash[32];
        uint8_t unlockPasswordHash[32];
        SHA256 hasher;

        ItemIterator* deviceIterator;
        ItemIterator* messageIterator;
        ItemIterator* nearbyDeviceIterator;
        ItemIterator* clusterIterator;
        uint8_t previewOffset = 0;
        uint8_t previewSize = 0;

        uint32_t screenLockTimeout = 0;
        void loadScreenLockTimeout ();

        char previewTitleBuffer[MESSAGE_TITLE_BUFFER_SIZE + 1];
        char previewTextBuffer[MESSAGE_PREVIEW_BUFFER_SIZE + 1];
        char previewTsBuffer[12];
        char previewDevIdBuffer[CHATTER_DEVICE_ID_SIZE + 1];
        char previewAliasBuffer[CHATTER_ALIAS_NAME_SIZE + 1];

        char currentDeviceFilter[CHATTER_DEVICE_ID_SIZE + 1];

        CommunicatorEvent eventBuffer;
        char histSenderId[CHATTER_DEVICE_ID_SIZE+1];
        char histRecipientId[CHATTER_DEVICE_ID_SIZE+1];
        char histMessageId[CHATTER_MESSAGE_ID_SIZE+1];

        unsigned long tickFrequency = 3000; // how often the ticker should blink
        unsigned long lastTick = 0;
        unsigned long lastTouch = millis();

        volatile bool actionButtonPressed = false;

        /** learning mode stuff **/

        // how often (millis), roughly, to trigger learn messages
        // in learning mode, we want about 1 message per minute across the entire cluster
        // so this will be calculated, based on the known number of devices
        unsigned long learnMessageFreq = 60000; 
        unsigned long nextScheduledLearn = 0;
        bool syncLearnActivity ();
        unsigned long lastLearnLog = millis();
        unsigned long learnLogFreq = 60000; // how often to log learning stats
        char learningTargetDevice[CHATTER_DEVICE_ID_SIZE+1];

        char learnMessageIdBuffer[CHATTER_MESSAGE_ID_SIZE +1];
        char learnSenderIdBuffer[CHATTER_DEVICE_ID_SIZE + 1];
        char learnRecipientIdBuffer[CHATTER_DEVICE_ID_SIZE + 1];
        char learnAliasBuffer[STORAGE_MAX_ALIAS_LENGTH + 1];
        MessageSendMethod learnSendMethodBuffer;
        MessageStatus learnStatusBuffer;
        MessageType learnMessageTypeBuffer;
        char learnTimestampBuffer[STORAGE_TS_SIZE+1];

        /** end learning mode stuff **/

};

#endif