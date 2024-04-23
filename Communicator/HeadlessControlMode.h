#include "CommunicatorControlMode.h"

#ifndef HEADLESSCONTROLMODE_H
#define HEADLESSCONTROLMODE_H

class HeadlessControlMode : public CommunicatorControlMode {
    public:
        HeadlessControlMode (DeviceType _deviceType) : CommunicatorControlMode (_deviceType) {}
        StartupState init ();
        void showStatus (const char* status);
        void showStatus (String status);
        void showTime ();
        void showTitle (const char* title);
        void updateChatDashboard ();
        //void updateChatStatusBuffer(ChatStatus chatStatus);

        void showBusy (); // shows indicator that activity is happening
        void showReady ();
        void showClusterOk ();
        void showClusterError ();
        void showClusterOnboard ();

    protected:
        unsigned long lastTimeUpdate = 0;
        //char chatStatusBuffer[16];

        // for displaying latest status
        ChatStatus loraStatus = ChatNoDevice;
        ChatStatus wifiStatus = ChatNoDevice;
        ChatStatus uartStatus = ChatNoDevice;
};

#endif