#include "ControlMode.h"
#include "ChatterAll.h"
#include "CommunicatorEvent.h"
#include "CommunicatorEventHandler.h"
#include "FullyInteractiveDisplay.h"
#include "TouchControl.h"

#if !defined(ADAFRUIT_FEATHER_M4_EXPRESS)
#include <ArduinoLowPower.h>
#endif

#ifndef COMMUNICATORCONTROLMODE_H
#define COMMUNICATORCONTROLMODE_H

class CommunicatorControlMode : public ControlMode, public CommunicatorEventHandler {
    public:
        CommunicatorControlMode (DeviceType _deviceType) : ControlMode (_deviceType) {}
        StartupState init ();
        void loop ();
        bool isInteractive () {return false;}
        virtual void beginInteractiveIfPossible () {}

        bool sendPacketBatchAir ();
        bool sendPacketBatchBridge ();

        bool handleEvent (CommunicatorEventType eventType);
        bool handleEvent (CommunicatorEvent* event);
        bool queueEvent (CommunicatorEventType eventType); // sets event to be handled next opportunity

        virtual void showStatus (const char* status) = 0;
        virtual void showStatus (String status) = 0;
        virtual void showTime () = 0;
        virtual void showTitle (const char* title) = 0;
        virtual void updateChatDashboard () = 0;

        virtual void showLastMessage ();

        virtual void showBusy () = 0; // shows indicator that activity is happening
        virtual void showReady () = 0;
        virtual void showClusterOk () = 0;
        virtual void showClusterError () = 0;
        virtual void showClusterOnboard () = 0;

        void disableMessaging ();
        void enableMessaging () { listeningForMessages = true; }
        virtual bool onboardNewClient (unsigned long timeout);
        void deepSleep ();

    protected:
        virtual void sleepOrBackground(unsigned long sleepTime);
        bool sendText = false;
        uint8_t messageBuffer[GUI_MESSAGE_BUFFER_SIZE];
        int messageBufferLength = 0;
        char otherDeviceId[CHATTER_DEVICE_ID_SIZE+1];
        bool handleConnectedDevice ();
        bool deviceMeshEnabled = true;
        bool initialized = false;
    
    private:
        void sendEchoText();
        //bool sendUserText (CommunicatorEvent* event);
        unsigned long loopCount = 0;
        uint8_t lastChannel = 0;
        ChatterPacketSmall chatterPacketDetails;
        uint8_t maxReadPacketsPerCycle = 5;
        uint8_t maxAirSendPacketsPerCycleSync = 5;
        uint8_t maxAirSendPacketsPerCycleAsync = 1; // for udp, we cant fire all out at once
        uint8_t maxBridgeSendPacketsPerCycle = 5;

        ChatterChannel* bridgeChannel;
        ChatterChannel* airChannel;

        bool queuedBridgeTraffic = false;
        bool queuedAirTraffic = false;

        void logPublicKey ();
        bool listeningForMessages = false;
        CommunicatorEventType queuedEventType = CommunicatorEventNone;
};

#endif