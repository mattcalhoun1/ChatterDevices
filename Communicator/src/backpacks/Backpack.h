#include "../globals/Globals.h"
#include "ChatterAll.h"
#include "../display/Display.h"
#include "../events/CommunicatorEvent.h"
#include "../control/InteractiveControlMode.h"

#ifndef BACKPACK_H
#define BACKPACK_H

enum BackpackType {
    BackpackTypeThermal = (uint8_t)'T',
    BackpackTypeRelay = (uint8_t)'R'
};

class Backpack {
    public:
        Backpack (Chatter* _chatter, Display* _display, InteractiveControlMode* _control) { chatter = _chatter, display = _display; control = _control; }

        virtual bool handleUserEvent (CommunicatorEventType eventType) = 0;
        virtual bool handleMessage (const uint8_t* message, int messageSize, const char* senderId, const char* recipientId) = 0;
        virtual bool init () = 0;
        virtual bool isRunning () = 0;
        virtual const char* getName () = 0;
        virtual BackpackType getType () = 0;

    protected:
        Chatter* chatter;
        Display* display;
        InteractiveControlMode* control;
        void logConsole (const char* msg);
};

#endif