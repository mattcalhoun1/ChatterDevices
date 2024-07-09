#ifndef COMMUNICATORPREFHANDLER_H
#define COMMUNICATORPREFHANDLER_H

enum CommunicatorPreference {
    PreferenceKeyboardLandscape = 0,
    PreferenceMessageHistory = 1,
    PreferenceWifiEnabled = 2,
    PreferenceMeshEnabled = 3,
    PreferenceWiredEnabled = 4,
    PreferenceLoraEnabled = 5,
    PreferenceMeshLearningEnabled = 6,
    PreferenceRemoteConfigEnabled = 7,
    PreferenceDstEnabled = 8,
    PreferenceIgnoreExpiryEnabled = 9,
    PreferenceBackpacksEnabled = 10,
    PreferenceBackpackThermalEnabled = 11,
    PreferenceBackpackThermalRemoteEnabled = 12,
    PreferenceBackpackThermalAutoEnabled = 13,
    PreferenceBackpackRelayEnabled = 14,
    PreferenceBackpackRelayRemoteEnabled = 15
};

enum StoredPreference {
    StoredPrefBackpacksEnabled = 0,
    StoredPrefBackpackThermalEnabled = 1,
    StoredPrefBackpackThermalRemoteEnabled = 2,
    StoredPrefBackpackThermalAutoEnabled = 3,
    StoredPrefBackpackRelayEnabled = 4,
    StoredPrefBackpackRelayRemoteEnabled = 5
};

class CommunicatorPreferenceHandler {
    public:
        virtual bool isPreferenceEnabled (CommunicatorPreference pref) = 0;
        virtual void enablePreference (CommunicatorPreference pref) = 0;
        virtual void disablePreference (CommunicatorPreference pref) = 0;
};

#endif