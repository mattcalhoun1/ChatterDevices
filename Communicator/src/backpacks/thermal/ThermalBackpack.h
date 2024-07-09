#include "../../globals/Globals.h"
#include "ChatterAll.h"
#include "../../display/Display.h"
#include "../../events/CommunicatorEvent.h"
#include "../Backpack.h"
#include "Camera.h"
#include "ThermalEncoder.h"
#include "../../control/InteractiveControlMode.h"
#include "../../prefs/CommunicatorPreferenceHandler.h"

#ifndef THERMALBACKPACK_H
#define THERMALBACKPACK_H

class ThermalBackpack : public Backpack {
    public:
        ThermalBackpack (Chatter* _chatter, Display* _display, InteractiveControlMode* _control) : Backpack(_chatter, _display, _control) {}

        bool handleUserEvent (CommunicatorEventType eventType);
        bool handleMessage (const uint8_t* message, int messageSize, const char* senderId, const char* recipientId);
        bool init ();
        bool isRunning ();
        const char* getName () { return "Thermal"; }
        BackpackType getType () { return BackpackTypeThermal; }


    protected:
        ThermalEncoder* encoder;
        Camera* camera;
        bool running = false;
        bool remoteEnabled = false;
        bool autoEnabled = false;
};

#endif