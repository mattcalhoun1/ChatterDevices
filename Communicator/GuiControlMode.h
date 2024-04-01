#include "ControlMode.h"
#include "HeadsUpControlMode.h"
#include "Display.h"
#include "Display_7789.h"
#include "TouchEnabledDisplay.h"
#include <RotaryEncoder.h>
#include "Menu.h"
#include "DeviceAliasIterator.h"
#include "MessageIterator.h"
#include "TestIterator.h"
#include "ItemIterator.h"

#ifndef GUICONTROLMODE_H
#define GUICONTROLMODE_H

//                                status/etc   ts       :          alias               (           send/rcpient     )   sp  >>  term
#define MESSAGE_TITLE_BUFFER_SIZE 3 + STORAGE_TS_SIZE + 1 +  CHATTER_ALIAS_NAME_SIZE + 1 + CHATTER_DEVICE_ID_SIZE + 1 + 1 + 2 + 2

#define MESSAGE_PREVIEW_BUFFER_SIZE 32

class GuiControlMode : public HeadsUpControlMode, public TouchListener {
    public:
        GuiControlMode(DeviceType _deviceType, bool _admin) : HeadsUpControlMode (_deviceType, _admin) {}
        bool init ();
        void buttonAPressed();
        bool isInteractive () {return true;}

        void showLastMessage ();

        // this is called via interrupt, so should not do anything except ping the rotary
        void handleRotary ();

        bool updateSelection ();
        int getSelection () {return selection;}
        void setSelection (int newSelection) {selection = newSelection;}
        void loop ();

        bool handleEvent (CommunicatorEventType eventType);
        bool initializeNewDevice ();

        bool onboardNewClient (unsigned long timeout);

        bool sendDirectMessage ();
        bool sendViaBridge ();

        bool handleScreenTouched (int touchX, int touchY);
    protected:
        bool updateMessagePreviewsIfNecessary ();
        void showMessageHistory(bool resetOffset);
        void sleepOrBackground (unsigned long sleepTime);

    private:
        bool fullRepaint = false;
        uint8_t lastChannel = 0;
        RotaryEncoder *rotary = nullptr;
        Menu* menu;

        char title[32];

        //ChatterAdmin* admin;
        int selection = 0;
        bool rotaryMoved = false;

        // for onboarding input
        char newDeviceAlias[CHATTER_ALIAS_NAME_SIZE+1];
        char newClusterAlias[CHATTER_ALIAS_NAME_SIZE+1];
        char newFreq[7];

        ItemIterator* deviceIterator;
        ItemIterator* messageIterator;
        uint8_t messagePreviewOffset = 0;
        uint8_t messageHistorySize = 0;

        char messageTitleBuffer[MESSAGE_TITLE_BUFFER_SIZE + 1];
        char messagePreviewBuffer[MESSAGE_PREVIEW_BUFFER_SIZE + 1];
};

#endif