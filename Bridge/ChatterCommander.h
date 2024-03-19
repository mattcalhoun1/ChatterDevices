#include <Arduino.h>
#include "ChatterAll.h"
#include "Commander.h"
#include "Globals.h"

#ifndef CHATTERCOMMANDER_H
#define CHATTERCOMMANDER_H
#define CHATTER_COMMANDER_PING_FREQUENCY 30000 // millis between pings

class ChatterCommander : public Commander {
  public:
    ChatterCommander(Chatter* _chatter, const char* _commanderAddress);
    void init ();
    bool isConnected();
    unsigned long lastConnected();
    Command getNextCommand ();
    Command extractCommand (String rawCommand, const char* sender);
    void sendResult (Command command, CommandResult result);
    void sendReceipt (Command command, ReceiptResult result);
    void sendCommand (Command command);
    void sendPing();

  private:
    unsigned long lastResponse = 0;
    Command receiveCommand ();
    unsigned long lastPing = 0;
    Chatter* chatter;
    char commanderAddress[CHATTER_DEVICE_ID_SIZE+1];
    bool isCompleteCommand(String msg);

    const char delimiterCmd = ':';
    const char delimiterId = '_';
    const char delimiterParam = '|';

    long sentCount = 0;
};
#endif