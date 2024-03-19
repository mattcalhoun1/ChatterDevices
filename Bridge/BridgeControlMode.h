#include "ControlMode.h"
#include "ChatterAll.h"

#ifndef BRIDGECONTROLMODE_H
#define BRIDGECONTROLMODE_H

class BridgeControlMode : public ControlMode {
    public:
        virtual bool init ();
        void loop ();
        bool isInteractive () {return false;}

        bool sendPacketBatchAir ();
        bool sendPacketBatchBridge ();


        virtual void showStatus (const char* status) = 0;
        virtual void showStatus (String status) = 0;
        virtual void showTime () = 0;
        virtual void showTitle (const char* title) = 0;
        virtual void updateChatDashboard () = 0;

    protected:
        void sleepOrBackground(unsigned long sleepTime);
    
    private:
        unsigned long loopCount = 0;
        ChatterPacketSmall chatterPacketDetails;
        uint8_t maxReadPacketsPerCycle = 5;
        uint8_t maxAirSendPacketsPerCycleSync = 5;
        uint8_t maxAirSendPacketsPerCycleAsync = 1; // for udp, we cant fire all out at once
        uint8_t maxBridgeSendPacketsPerCycle = 5;

        ChatterChannel* bridgeChannel;
        ChatterChannel* airChannel;

        bool queuedBridgeTraffic = false;
        bool queuedAirTraffic = false;
        bool initialized = false;
};

#endif