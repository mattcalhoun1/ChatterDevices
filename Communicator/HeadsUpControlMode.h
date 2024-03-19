#include "CommunicatorControlMode.h"
#include "Display.h"
#include "Display_7789.h"
#include "Display_TFT.h"
#include "Menu.h"
#include "Keyboard.h"

#ifndef HEADSUPCONTROLMODE_H
#define HEADSUPCONTROLMODE_H

class HeadsUpControlMode : public CommunicatorControlMode {
    public:
        HeadsUpControlMode (DeviceType _deviceType, bool _admin) : CommunicatorControlMode (_deviceType, _admin) {}
        bool init ();
        void showStatus (const char* status);
        void showStatus (String status);
        void showTime ();
        void showTitle (const char* title);
        void updateChatDashboard ();

        void showBusy ();
        void showReady ();
        void showClusterOk ();
        void showClusterError ();
        void showClusterOnboard ();

        //void captureLiveThermal ();
        //void showLiveThermal ();
        void loop ();

        bool isFullyInteractive () { return fullyInteractive; }
    protected:
        bool fullyInteractive = false;
        ThermalEncoder* encoder;
        Display* display = nullptr;
        DisplayColor getDisplayColorForStatus (ChatStatus chatStatus);
        int getSymboForDevice(const char* deviceName);
        unsigned long lastTimeUpdate = 0;

        const char* channelUnknown = "?";
        const char* dashboardChannels[MAX_CHANNELS];
        DisplayColor dashboardColors[MAX_CHANNELS];
};

#endif