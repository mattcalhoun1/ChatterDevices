#include "../globals/Globals.h"

#ifndef COMMUNICATOR_EVENTS_H
#define COMMUNICATOR_EVENTS_H

#define EVENT_TARGET_SIZE 8
#define EVENT_DATA_SIZE 128

enum CommunicatorEventType {
    UserRequestOnboard = 0,
    UserRequestJoinCluster = 1,
    UserRequestNewCluster = 2,
    UserRequestDeleteCluster = 3,
    UserRequestQuickFactoryReset = 4,
    UserRequestSecureFactoryReset = 5,
    UserChangedTime = 6,
    UserRequestDirectMessage = 7,
    UserRequestSelfAnnounce = 8,
    PingLoraBridge = 9,
    CommunicatorEventNone = 10,
    UserRequestSecureBroadcast = 11,
    UserRequestOpenBroadcast = 12,
    MenuClosed = 13,
    UserDeleteAllMessages = 14,
    UserRequestReply = 15,
    UserRequestScreenLock = 16,
    UserRequestPowerOff = 17,
    UserRequestClearMeshCache = 18,
    UserRequestFlipScreen = 19,
    UserRequestMeshShowPath = 20,
    UserRequestChangeTime = 21,
    UserRequestClearMeshGraph = 22,
    MessageReceived = 23,
    AckReceived = 24,
    DeviceBackup = 25,
    DeviceRestore = 26,
    UserRequestChangeCluster = 27,
    UserRequestFilterMessages = 28,
    BroadcastReceived = 29,
    RemoteConfigReceived = 30,
    UserRequestRemoteBattery = 31,
    UserRequestRemotePath = 32,
    UserRequestRemoteClearMeshGraph = 33,
    UserRequestRemoteClearMeshCache = 34,
    UserRequestRemoteClearPingTable = 35,
    UserRequestRemoteEnableLearn = 36,
    UserRequestRemoteDisableLearn = 37,
    UserRequestRemoteMessagesClear = 38,
    UserRequestClearMeshGraphNoPrompt = 40,
    UserDeleteAllMessagesNoPrompt = 41,
    UserRequestClearPingTable = 42,
    ThermalReceived = 43,
    UserRequestShowId = 44,
    UserRequestScreenTimeout1Min = 45,
    UserRequestScreenTimeout2Min = 46,
    UserRequestScreenTimeout5Min = 47,
    UserRequestScreenTimeoutNever = 48,
    UserRequestChangePassword = 49,
    UserThermalSnap = 50,
    UserThermalSend = 51,
    UserThermalExit = 52,
    UserPressActionButton = 53,
    RemoteBackpackRequestReceived = 54,
    UserTriggerRemoteThermal = 55,
    UserTriggerRemoteRelay = 56,
    UserRequestPublicKeyTest = 57,
    UserTriggerFccSpam1 = 100,
    UserTriggerFccSpam32 = 101,
    UserTriggerFccSpam64 = 102,
    UserTriggerFccHop = 103,
    UserRequestTzChange = 104
};

struct CommunicatorEvent {
    CommunicatorEventType EventType;
    char EventTarget[EVENT_TARGET_SIZE];
    uint8_t EventData[EVENT_DATA_SIZE];
    uint8_t EventDataLength;
};

#endif