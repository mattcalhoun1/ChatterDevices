#include <Arduino.h>
#include "../../globals/Globals.h"
#include "ChatterAll.h"
#include "../../display/Display.h"
#include "../../events/CommunicatorEvent.h"
#include "../Backpack.h"
#include "../../control/InteractiveControlMode.h"
#include "../../prefs/CommunicatorPreferenceHandler.h"

#ifndef RELAYBACKPACK_H
#define RELAYBACKPACK_H

#define RELAY_OUT_PIN 0

class RelayBackpack : public Backpack {
    public:
        RelayBackpack (Chatter* _chatter, Display* _display, InteractiveControlMode* _control) : Backpack(_chatter, _display, _control) {}

        bool handleUserEvent (CommunicatorEventType eventType);
        bool handleMessage (const uint8_t* message, int messageSize, const char* senderId, const char* recipientId);
        bool init ();
        bool isRunning ();
        const char* getName () { return "Relay"; }
        BackpackType getType () { return BackpackTypeRelay; }


    protected:
        void triggerRelay ();

        bool running = false;
        bool remoteEnabled = false;
        bool autoEnabled = false;
};

#endif