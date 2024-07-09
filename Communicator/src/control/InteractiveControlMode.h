#include "../display/FullyInteractiveDisplay.h"

#ifndef INTERACTIVECONTROLMODE_H
#define INTERACTIVECONTROLMODE_H
class InteractiveControlMode {
    public:
        virtual void setInteractiveContext (InteractiveContext _context) { interactiveContext = _context; }
        virtual void showButtons () = 0;
    

        virtual bool promptSelectDevice () = 0;
        virtual const char* getSelectedDeviceId() = 0;
        virtual bool sendMessage (const char* deviceId, uint8_t* msg, int msgSize, MessageType _type) = 0;
        virtual void setCurrentDeviceFilter (const char* _device) = 0;
        virtual void requestFullRepaint () = 0;

    protected:
        InteractiveContext interactiveContext = InteractiveHome;
};


#endif