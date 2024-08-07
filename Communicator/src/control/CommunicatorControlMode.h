#include "ControlMode.h"
#include "ChatterAll.h"
#include "../events/CommunicatorEvent.h"
#include "../events/CommunicatorEventHandler.h"
#include "../display/FullyInteractiveDisplay.h"
#include "../touch/TouchControl.h"
#include "RemoteConfig.h"
#include "../prefs/CommunicatorPreferenceHandler.h"

#if !defined(ADAFRUIT_FEATHER_M4_EXPRESS)
#include <ArduinoLowPower.h>
#endif

#ifndef COMMUNICATORCONTROLMODE_H
#define COMMUNICATORCONTROLMODE_H

class CommunicatorControlMode : public ControlMode, public CommunicatorEventHandler, public CommunicatorPreferenceHandler {
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

        // these ack questions shoudl not bubble up to this layer at this point
        bool shouldAcknowledge (uint8_t address, uint8_t* message, uint8_t length) { return false; }


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
        virtual void notifyMessageReceived();

        uint8_t getBatteryLevel (); // 0 - 100

        bool isPreferenceEnabled (CommunicatorPreference pref);
        void enablePreference (CommunicatorPreference pref);
        void disablePreference (CommunicatorPreference pref);                

    protected:
        virtual void sleepOrBackground(unsigned long sleepTime);
        bool sendText = false;
        uint8_t messageBuffer[GUI_MESSAGE_BUFFER_SIZE];
        int messageBufferLength = 0;
        MessageType messageBufferType;
        
        char otherDeviceId[CHATTER_DEVICE_ID_SIZE+1];
        char otherClusterId[CHATTER_LOCAL_NET_ID_SIZE+CHATTER_GLOBAL_NET_ID_SIZE+1];
        bool handleConnectedDevice ();
        bool deviceMeshEnabled = true;
        bool initialized = false;
    
        void populateMeshPath (const char* recipientId);
    
        uint8_t meshPath[CHATTER_MESH_MAX_HOPS];
        uint8_t meshPathLength = 0;
        char meshDevIdBuffer[CHATTER_DEVICE_ID_SIZE + 1];
        char meshAliasBuffer[CHATTER_ALIAS_NAME_SIZE + 1];        

    private:
        void sendEchoText();
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
        bool remoteConfigEnabled = false;
        CommunicatorEventType queuedEventType = CommunicatorEventNone;

        bool isRemoteConfig (const uint8_t* msg, int msgLength);
        bool executeRemoteConfig (CommunicatorEvent* event);
        bool isBackpackRequest (const uint8_t* msg, int msgLength);

};

#endif