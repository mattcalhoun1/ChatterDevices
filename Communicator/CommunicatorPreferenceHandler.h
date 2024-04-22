#ifndef COMMUNICATORPREFHANDLER_H
#define COMMUNICATORPREFHANDLER_H

enum CommunicatorPreference {
    PreferenceKeyboardLandscape = 0,
    PreferenceMessageHistory = 1,
    PreferenceWifiEnabled = 2,
    PreferenceMeshEnabled = 3
};

class CommunicatorPreferenceHandler {
    public:
        virtual bool isPreferenceEnabled (CommunicatorPreference pref) = 0;
        virtual void enablePreference (CommunicatorPreference pref) = 0;
        virtual void disablePreference (CommunicatorPreference pref) = 0;
};

#endif