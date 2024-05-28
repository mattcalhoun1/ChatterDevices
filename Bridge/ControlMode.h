
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
  BridgeRequestIdentity = 5 // requesting info about a device, given an id
};

enum AckRequestType {
  AckRequestTrue = 1, // ignore
  AckRequestFalse = 2
};

/**
 * Base class for the different control modes available for this vehicle.
 */
class ControlMode : ChatStatusCallback {
  public:
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

    virtual void showBusy () = 0; // shows indicator that activity is happening

    virtual void showClusterOk () = 0;
    virtual void showClusterError () = 0;
    virtual void showClusterOnboard () = 0;

  protected:
    // miscellaneous
    virtual void logConsole(String msg);
    virtual void logConsole(const char* msg);
    virtual void logConsole(const char* msg, const char* msg2);
    virtual void sleepOrBackground(unsigned long sleepTime);
    const char* getStatusName (ChatStatus chatStatus);

    bool factoryResetCheck (ChatterDeviceType deviceType);
    bool joinClusterCheck ();

    virtual int freeMemory(); // returns estimated free memory

    RTClockBase* rtc;
    Chatter* chatter;

    //ClusterAssistantInterface* assistant;
    //bool assistantEnabled = false;

    //ClusterAdmin* admin;
    bool justOnboarded = false;

    long loopCount = 0;
};
#endif