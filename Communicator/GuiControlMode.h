#include "ControlMode.h"
#include "HeadsUpControlMode.h"
#include "Display.h"
#include "Display_7789.h"
#include <RotaryEncoder.h>
#include "Menu.h"
#include "DeviceAliasIterator.h"
#include "TestIterator.h"
#include "ItemIterator.h"

#ifndef GUICONTROLMODE_H
#define GUICONTROLMODE_H

class GuiControlMode : public HeadsUpControlMode {
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

    protected:
        void adminLoop ();

        // managing state
        void showAdminMenu ();

    private:
        bool fullRepaint = false;
        uint8_t lastChannel = 0;
        bool adminMode = false;
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

};

#endif