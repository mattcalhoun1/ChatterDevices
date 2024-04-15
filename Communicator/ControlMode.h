
#include <Arduino.h>
#include <stdint.h>
#include "Globals.h"
#include "ChatterAll.h"

#ifndef CONTROL_MODE_H
#define CONTROL_MODE_H

enum EncodingType {
  EncodingTypeText = 1,
  EncodingTypeBinary = 2
};

enum BridgeRequestType {
  BridgeRequestNone = 1, // ignore
  BridgeRequestEcho = 2, // request message be echoed
  BridgeRequestBridge = 3, // request a bridge to other communication type
  BridgeRequestTime = 4, // request current time
  BridgeRequestIdentity = 5
};

enum AckRequestType {
  AckRequestTrue = 1, // ignore
  AckRequestFalse = 2
};

enum DeviceType {
  DeviceTypeCommunicator = 0,
  DeviceTypeCommunicatorMini = 1
};

/**
 * Base class for the different control modes available for this vehicle.
 */
class ControlMode : ChatStatusCallback {
  public:
    ControlMode (DeviceType _deviceType) { deviceType = _deviceType; }
    virtual void loop();
    virtual bool init();
    virtual void showStatus (const char* status);

    virtual void showTime();
    virtual bool isInteractive () { return false; }

    String getFormattedDate ();
    String getFormattedTime ();

    // callback from chat status
    void updateChatStatus (uint8_t channelNum, ChatStatus newStatus);
    void updateChatStatus (const char* statusMessage);
    virtual void updateChatDashboard (); // redisplay chat dashboard
    void updateChatProgress(float progress);
    void resetChatProgress ();

    // touch screen, etc
    bool isFullyInteractive () { return false; }
    virtual bool initializeNewDevice ();

  protected:
    // miscellaneous
    virtual void logConsole(String msg);
    virtual void sleepOrBackground(unsigned long sleepTime);
    const char* getStatusName (ChatStatus chatStatus);
    bool factoryResetCheck(bool forceReset = false, bool writeZeros = false);

    virtual int freeMemory(); // returns estimated free memory

    RTClockBase* rtc;
    Chatter* chatter;


    // eitehr admin or assistant can be enabled, but not both
    ClusterAdminInterface* cluster = nullptr;
    //bool adminEnabled = false;
    //ClusterAdmin* admin = nullptr;

    ClusterAssistantInterface* assistant = nullptr;
    //bool assistantEnabled = false;

    //ClusterAdmin* admin;
    bool justOnboarded = false;

    long loopCount = 0;

    DeviceType deviceType;

    void restartDevice();
};
#endif