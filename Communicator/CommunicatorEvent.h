#include "Globals.h"

#ifndef COMMUNICATOR_EVENTS_H
#define COMMUNICATOR_EVENTS_H

#define EVENT_TARGET_SIZE 8
#define EVENT_DATA_SIZE 128

enum CommunicatorEventType {
    UserRequestOnboard = 0,
    UserRequestJoinCluster = 1,
    UserRequestNewCluster = 2,
    UserRequestDeletecluster = 3,
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
    UserRequestMeshShowPath = 20
};

struct CommunicatorEvent {
    CommunicatorEventType EventType;
    char EventTarget[EVENT_TARGET_SIZE];
    uint8_t EventData[EVENT_DATA_SIZE];
    uint8_t EventDataLength;
};

#endif