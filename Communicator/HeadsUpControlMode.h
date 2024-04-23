#include "CommunicatorControlMode.h"
#include "Display.h"
#include "Display_TFT.h"
#include "Menu.h"
#include "Keyboard.h"
#include "CommunicatorPreferenceHandler.h"

#ifndef HEADSUPCONTROLMODE_H
#define HEADSUPCONTROLMODE_H

class HeadsUpControlMode : public CommunicatorControlMode, public CommunicatorPreferenceHandler {
    public:
        HeadsUpControlMode (DeviceType _deviceType) : CommunicatorControlMode (_deviceType) {}
        StartupState init ();
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

        bool isPreferenceEnabled (CommunicatorPreference pref);
        void enablePreference (CommunicatorPreference pref);
        void disablePreference (CommunicatorPreference pref);                
    protected:
        bool fullyInteractive = false;
        ThermalEncoder* encoder;
        Display* display = nullptr;
        DisplayColor getDisplayColorForStatus (ChatStatus chatStatus);
        int getSymboForDevice(const char* deviceName);
        unsigned long lastTimeUpdate = 0;

        const char* channelUnknown = "?";
        char dashboardChannels[MAX_CHANNELS][CHANNEL_DISPLAY_SIZE];
        DisplayColor dashboardColors[MAX_CHANNELS];
};

#endif