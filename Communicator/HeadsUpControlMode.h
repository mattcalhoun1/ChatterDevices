#include "CommunicatorControlMode.h"
#include "src/display/Display.h"
#include "src/display/Display_TFT.h"
#include "src/menu/Menu.h"
#include "src/keyboard/Keyboard.h"
#include "src/prefs/CommunicatorPreferenceHandler.h"

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
        void updateChatDashboard (bool forceRepaint);

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
        bool screenLocked = false;

        ThermalEncoder* encoder;
        Display* display = nullptr;
        DisplayColor getDisplayColorForStatus (ChatStatus chatStatus);
        int getSymboForDevice(const char* deviceName);
        unsigned long lastTimeUpdate = 0;

        const char* channelUnknown = "?";
        char dashboardChannels[MAX_CHANNELS][CHANNEL_DISPLAY_SIZE];
        DisplayColor dashboardColors[MAX_CHANNELS];
        ChatStatus lastChannelStatus[MAX_CHANNELS];

        char lastStatus[24];
};

#endif