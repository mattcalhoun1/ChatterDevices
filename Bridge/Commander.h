#include <Arduino.h>
#include "Globals.h"
#include "ChatterAll.h"

#ifndef COMMANDER_H
#define COMMANDER_H
#define COMMANDER_PING_FREQUENCY 500 // millis between pings

#define CMD_PING "PING"
#define CMD_GET_THERMAL "GETTHERMAL"
#define CMD_CAMERA_PAN "CAMPAN"
#define CMD_CAMERA_TILT "CAMTILT"
#define CMD_DISPLAY "DISPLAY"
#define CMD_UNKNOWN "UNKNOWN"

#define CMD_RESULT "RESULT" // for giving result of command
#define CMD_RECEIPT "RECEIPT" // for confirming receipt of a message

enum CommandType {
  Ping = 0,
  GetThermal = 1,
  CameraPan =2,
  CameraTilt = 3,
  DisplayText = 4,
  Nothing = 5,
  Unknown = 6,
  Receipt = 7,
  Result = 8
};

enum CommandResult {
  Success = 0,
  Fail = 1
};

enum ReceiptResult {
  Ok = 0,
  NotOk = 1
};

class Command {
  public:
    virtual CommandType getCommandType ();
    void setCommandType (CommandType commandType);
    String getParams ();
    void setParams(String params);
    void setId (long id);
    long getId ();
    void setSender(const char* sender);
    const char* getSender ();
    void setRecipient(const char* recipient);
    const char* getRecipient ();
  protected:
    CommandType commandType;
    String params;
    long id;
    char sender[CHATTER_DEVICE_ID_SIZE + 1];
    char recipient[CHATTER_DEVICE_ID_SIZE + 1];
};

class Commander {
  public:
    virtual void init () = 0;
    virtual bool isConnected() = 0;
    virtual unsigned long lastConnected() = 0;
    virtual Command getNextCommand () = 0;
    virtual Command extractCommand (String rawCommand, const char* sender) = 0;
    virtual void sendResult (Command command, CommandResult result) = 0;
    virtual void sendReceipt (Command command, ReceiptResult result) = 0;
    virtual void sendCommand (Command command) = 0;
    virtual void sendPing() = 0;
    String getCommandName (CommandType commandType);
    CommandType getCommandType (String commandName);
    void logConsole(String str);
};
#endif