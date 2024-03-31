#include "Globals.h"

#ifndef COMMUNICATOR_EVENTS_H
#define COMMUNICATOR_EVENTS_H

#define EVENT_TARGET_SIZE 8
#define EVENT_DATA_SIZE 128

enum CommunicatorEventType {
    UserRequestBleOnboard = 0,
    UserRequestBleJoinCluster = 1,
    UserRequestNewCluster = 2,
    UserRequestDeletecluster = 3,
    UserRequestFactoryReset = 4,
    UserChangedTime = 5,
    UserRequestDirectMessage = 6,
    UserRequestSelfAnnounce = 7,
    PingLoraBridge = 8,
    CommunicatorEventNone = 9,
    UserRequestSecureBroadcast = 10,
    UserRequestOpenBroadcast = 11,
    MenuClosed = 12,
    UserDeleteAllMessages = 13
};

struct CommunicatorEvent {
    CommunicatorEventType EventType;
    char EventTarget[EVENT_TARGET_SIZE];
    uint8_t EventData[EVENT_DATA_SIZE];
    uint8_t EventDataLength;
};

#endif