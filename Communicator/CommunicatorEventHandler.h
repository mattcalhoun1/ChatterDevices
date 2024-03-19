#include "CommunicatorEvent.h"

#ifndef COMMUNICATOR_EVENT_HANDLER_H
#define COMMUNICATOR_EVENT_HANDLER_H

class CommunicatorEventHandler {
    public:
        virtual bool handleEvent (CommunicatorEventType eventType) = 0;
        virtual bool handleEvent (CommunicatorEvent* event) = 0;
        virtual bool queueEvent (CommunicatorEventType eventType) = 0; // sets event to be handled next opportunity
};

#endif