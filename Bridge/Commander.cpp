#include "Commander.h"

void Commander::logConsole (String msg) {
  if (LOG_ENABLED) {
    Serial.println(msg);
  }
}

String Commander::getCommandName (CommandType commandType) {
  switch (commandType) {
    case Ping:
      return CMD_PING;
    case GetThermal:
      return CMD_GET_THERMAL;
    case CameraPan:
      return CMD_CAMERA_PAN;
    case CameraTilt:
      return CMD_CAMERA_TILT;
    case DisplayText:
      return CMD_DISPLAY;
    case Result:
      return CMD_RESULT;
    case Receipt:
      return CMD_RECEIPT;
    default:
      return CMD_UNKNOWN;
  }
}

CommandType Commander::getCommandType (String commandName) {
  if (commandName.equalsIgnoreCase(CMD_PING)) {
    return Ping;
  }
  else if (commandName.equalsIgnoreCase(CMD_GET_THERMAL)) {
    return GetThermal;
  }
  else if (commandName.equalsIgnoreCase(CMD_CAMERA_PAN)) {
    return CameraPan;
  }
  else if (commandName.equalsIgnoreCase(CMD_CAMERA_TILT)) {
    return CameraTilt;
  }
  else if (commandName.equalsIgnoreCase(CMD_DISPLAY)) {
    return DisplayText;
  }
  else if (commandName.equalsIgnoreCase(CMD_RECEIPT)) {
    return Receipt;
  }
  else if (commandName.equalsIgnoreCase(CMD_RESULT)) {
    return Result;
  }

  return Unknown;
}


CommandType Command::getCommandType () {
  return commandType;
}

void Command::setCommandType (CommandType commandType) {
  this->commandType = commandType;
}

String Command::getParams () {
  return params;
}

void Command::setParams (String params) {
  this->params = params;
}


void Command::setId (long id) {
  this->id = id;
}

long Command::getId () {
  return id;
}

void Command::setSender (const char* sender) {
  memset(this->sender, 0, CHATTER_DEVICE_ID_SIZE+1);
  memcpy(this->sender, sender, CHATTER_DEVICE_ID_SIZE);
}

const char* Command::getSender () {
  return this->sender;
}

void Command::setRecipient (const char* recipient) {
  memset(this->recipient, 0, CHATTER_DEVICE_ID_SIZE+1);
  memcpy(this->recipient, recipient, CHATTER_DEVICE_ID_SIZE);
}

const char* Command::getRecipient() {
  return recipient;
}
