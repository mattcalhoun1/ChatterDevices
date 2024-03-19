#include "ChatterCommander.h"

ChatterCommander::ChatterCommander (Chatter* _chatter, const char* _commanderAddress) {
  memset(commanderAddress, 0, CHATTER_DEVICE_ID_SIZE+1);
  memcpy(commanderAddress, _commanderAddress, CHATTER_DEVICE_ID_SIZE);
  
  chatter = _chatter;
}

void ChatterCommander::init() {
  if (chatter->isRunning()) {
    logConsole("Commander listening on Chatter");
  }
  else {
    logConsole("Commander disabled, Chatter not running");
  }
}

Command ChatterCommander::getNextCommand () {
  return receiveCommand ();
}

unsigned long ChatterCommander::lastConnected() {
  return lastResponse;
}

bool ChatterCommander::isConnected() {
  return (lastResponse > 0);
}

bool ChatterCommander::isCompleteCommand(String msg) {
  // should contain similar to:
  // COMMAND#id:str
  // COMMAND#id:str|num

  int delim = msg.indexOf(delimiterCmd);
  int idDelim = msg.indexOf(delimiterId);

  if (delim == -1 || idDelim == -1 || idDelim > delim) {
    return false;
  }

  int pipeCount = 0;
  String cmd = msg.substring(0, idDelim);
  if (cmd.length() <= 16) {
    for (int c = delim+1; c < msg.length(); c++) {
      if (msg.charAt(c) == delimiterCmd) {
        return false;
      }
      else if (msg.charAt(c) == delimiterParam) {
        pipeCount++;
      }
    }     
  }

  if (cmd.equalsIgnoreCase(CMD_PING) && pipeCount == 0) {
    return true;
  }
  else if (cmd.equalsIgnoreCase(CMD_GET_THERMAL) && pipeCount == 0) {
    return true;
  }
  else if (cmd.equalsIgnoreCase(CMD_CAMERA_PAN) && pipeCount == 1) {
    return true;
  }
  else if (cmd.equalsIgnoreCase(CMD_CAMERA_TILT) && pipeCount == 1) {
    return true;
  }
  else if (cmd.equalsIgnoreCase(CMD_DISPLAY)) {
    return true;
  }
  else if (cmd.equalsIgnoreCase(CMD_RESULT)) {
    return true;
  }
  else if (cmd.equalsIgnoreCase(CMD_RECEIPT)) {
    return true;
  }
  return false;
}

Command ChatterCommander::extractCommand(String rawCommand, const char* sender) {
  String command = rawCommand;
  if (isCompleteCommand(rawCommand)) {
    String params = "";
    long id = 0;
    int delim = rawCommand.indexOf(delimiterCmd);

    if (delim != -1) {
      params = rawCommand.substring(delim+1);
      params.trim();
      id = atol(rawCommand.substring(rawCommand.indexOf(delimiterId)+1, rawCommand.indexOf(delimiterCmd)).c_str());
      command = rawCommand.substring(0, rawCommand.indexOf(delimiterId));
    }

    CommandType cmdType = getCommandType(command);
    Command cmd;
    cmd.setCommandType(cmdType);
    cmd.setParams(params);
    cmd.setId(id);
    cmd.setSender(sender);
    if (cmdType != Unknown) {
      return cmd;
    }
    else {
      logConsole("Unknown command " + command);
    }
  }
  else {
    logConsole("Received invalid formatted command " + command);
  }

  // Notify we are ready
  //sendPing();

  Command nullCommand;
  nullCommand.setCommandType(Nothing);
  return nullCommand;
}

Command ChatterCommander::receiveCommand () {
  // If there is room in the queue, check for another command
  if (chatter->hasMessage() && chatter->retrieveMessage() && chatter->getMessageType() == MessageTypeComplete) {
    lastResponse = millis();

    int messageLength = chatter->getMessageSize();
    String command = String((char*)chatter->getTextMessage());
    const char* sender = chatter->getLastSender();

    return extractCommand(command, sender);
  }

  Command nullCommand;
  nullCommand.setCommandType(Nothing);
  return nullCommand;
}

void ChatterCommander::sendCommand(Command command) {
  String commandMsg = String(getCommandName(command.getCommandType()) + String(delimiterId) + String(command.getId()) + String(delimiterCmd) + String(command.getParams()));
  chatter->send((uint8_t*)commandMsg.c_str(), commandMsg.length(), command.getRecipient());
}

void ChatterCommander::sendReceipt (Command command, ReceiptResult result) {
  String receiptMsg = String(CMD_RECEIPT + String(delimiterId) + String(command.getId()) + String(delimiterCmd) + String(result));
  chatter->send((uint8_t*)receiptMsg.c_str(), receiptMsg.length(), command.getSender());  
}

void ChatterCommander::sendResult (Command command, CommandResult result) {
  String resultMsg = String(CMD_RESULT + String(delimiterId) + String(command.getId()) + String(delimiterCmd) + String(result));
  chatter->send((uint8_t*)resultMsg.c_str(), resultMsg.length(), command.getSender());  
}

void ChatterCommander::sendPing() {
  if (millis() - CHATTER_COMMANDER_PING_FREQUENCY > lastPing) {
    lastPing = millis();
    String pingMsg = String(CMD_PING + String(delimiterId) + String(sentCount++));
    chatter->send((uint8_t*)pingMsg.c_str(), pingMsg.length(), commanderAddress);
  }
}

