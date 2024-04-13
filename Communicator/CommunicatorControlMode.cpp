#include "CommunicatorControlMode.h"

bool CommunicatorControlMode::init() {
    logConsole("CommunicatorControlMode Initializing");

    // make sure parent is initialized
    if (ControlMode::init()) {
      airChannel = chatter->getChannel(0);
      //bridgeChannel = chatter->getChannel(1);

      //showTitle("Communicator");
      logConsole("CommunicatorControlMode running");
      showTime();
      showStatus("Ready");

      initialized = true;
      
      //logPublicKey();

      enableMessaging();
    }

    return initialized;
}

void CommunicatorControlMode::loop () {
  if (!initialized) {
    logConsole("Messaging not ready.");
    showClusterError();
  }

  // if a device is syncing/etc, handle that now
  //if (adminEnabled) {
  //  handleConnectedDevice();  
  //}

  if (queuedEventType != CommunicatorEventNone) {
    // handle the event
    handleEvent(queuedEventType);

    // clear the queued event
    queuedEventType = CommunicatorEventNone;
  }

  //showTime();
  int numPacketsThisCycle = 0;

  if (listeningForMessages) {
    while (chatter->hasMessage() && numPacketsThisCycle++ < maxReadPacketsPerCycle) {
      showStatus("Receiving");
      if(chatter->retrieveMessage() && chatter->getMessageType() == MessageTypeComplete) {
        //int messageLength = chatter->getMessageSize();
        logConsole("Processing trusted message...");

        messageBufferLength = chatter->getMessageSize();
        memcpy(messageBuffer, chatter->getTextMessage(), messageBufferLength);
        messageBuffer[messageBufferLength] = 0;
        memcpy(otherDeviceId, chatter->getLastSender(), CHATTER_DEVICE_ID_SIZE);
        otherDeviceId[CHATTER_DEVICE_ID_SIZE] = 0;

        showLastMessage ();
      }
      showReady();
    }
  }

  if (loopCount % 10 == 0) {
    //Serial.print("Free Memory: "); Serial.println(freeMemory());
    showTime();
  }
  else if (sendText) {
    showBusy();
    sendEchoText();
    showReady();
    sendText = false;
  }

  if (loopCount % RTC_SYNC_FREQUENCY == 0) {
    // don't do this once the fram has been initialized
    //rtc->syncWithExternalRtc();
  }

  sleepOrBackground(50);
  loopCount++;
}

void CommunicatorControlMode::showLastMessage () {
  Serial.print(otherDeviceId);
  Serial.print(" -> ");
  // log the message
  for (int i = 0; i < messageBufferLength; i++) {
    Serial.print(messageBuffer[i]);
  }
  Serial.println("");

}

void CommunicatorControlMode::sendEchoText () {
  logConsole("Sending echo text..");
  ChatterMessageFlags flags;
  flags.Flag0 = EncodingTypeText;
  flags.Flag1 = BridgeRequestEcho;//BridgeRequestEcho / BridgeRequestBridge
  flags.Flag2 = AckRequestTrue;
  flags.Flag3 = 4;
  flags.Flag4 = 5;
  flags.Flag5 = 6;

  memset(messageBuffer, 0, COMMUNICATOR_MESSAGE_BUFFER_SIZE);
  memcpy(messageBuffer, "Hello ", 6);
  int msgLen = 6;

  // 10 random digits
  for (int i = 0; i < 10; i++) {
    messageBuffer[msgLen++] = (char)((chatter->getRandom() % 10) + 48);
  }

  // which channel do we want to use, 0 or 1
  if (lastChannel + 1 < chatter->getNumChannels()) {
    lastChannel++;
  }
  else {
    lastChannel = 0;
  }

  const char* bridgeId = lastChannel == 0 ? BRIDGE_LORA_DEVICE_ID : BRIDGE_WIFI_DEVICE_ID;
  char fullDeviceId[CHATTER_DEVICE_ID_SIZE+1];
  memcpy(fullDeviceId, chatter->getDeviceId(), CHATTER_DEVICE_ID_SIZE);
  memcpy(fullDeviceId + CHATTER_LOCAL_NET_ID_SIZE + CHATTER_GLOBAL_NET_ID_SIZE, bridgeId, 3);
  fullDeviceId[CHATTER_DEVICE_ID_SIZE] = '\0';

  // attempt to bridge or echo a message to myself
  //      sendViaIntermediary(uint8_t *message, int length, const char* recipientDeviceId, const char* intermediaryDeviceId, ChatterMessageFlags* flags, ChatterChannel* channel);
  if(chatter->sendViaIntermediary(messageBuffer, msgLen, chatter->getDeviceId(), fullDeviceId, &flags, chatter->getChannel(lastChannel))) { // send a message to the bridge
    logConsole("Message successfully sent");
  }
  else {
    logConsole("Message not sent!");
  }
}

void CommunicatorControlMode::sleepOrBackground(unsigned long sleepTime) {
  unsigned long startTime = millis();
  delay(sleepTime);
}

void CommunicatorControlMode::deepSleep() {
  logConsole("sleeping");
  LowPower.deepSleep();
}

bool CommunicatorControlMode::sendPacketBatchAir () {
  bool packetsToSend = true; //assume there might be a batch to send
  
  // this max packets amount will likely change once the first message is picked up, as messages can override the sync/async default of the air channel
  int maxThisCycle = strcmp(chatter->getChannel(0)->getName(), "WiFi") == 0 ? maxAirSendPacketsPerCycleAsync : maxAirSendPacketsPerCycleSync;
  
  for (int i = 0; packetsToSend && i < maxThisCycle; i++) {
    packetsToSend = chatter->getPacketStore()->readNextAirOutPacketDetails(&chatterPacketDetails);
    if(packetsToSend) {
      //logConsole("Want to send via air: " + String((const char*)chatterPacketDetails.messageId));
      char recipient[CHATTER_DEVICE_ID_SIZE+1];
      memset(recipient, 0, CHATTER_DEVICE_ID_SIZE+1);

      bool ackRequested = false;

      // figure out the recipient
      if (chatterPacketDetails.chunkId != 0) {
        int len = chatter->getPacketStore()->readPacketFromAirOut((const char*)chatterPacketDetails.sender, (const char*)chatterPacketDetails.messageId, 0, chatterPacketDetails.content, CHATTER_FULL_BUFFER_LEN);
        chatter->ingestPacketMetadata((const char*)chatterPacketDetails.content, len);
        memcpy(recipient, chatter->getLastRecipient(), CHATTER_DEVICE_ID_SIZE);
        ackRequested = chatter->getMessageFlags().Flag2 == AckRequestTrue;
      }

      // load the packet
      chatterPacketDetails.contentLength = chatter->getPacketStore()->readPacketFromAirOut((const char*)chatterPacketDetails.sender, (const char*)chatterPacketDetails.messageId, chatterPacketDetails.packetId, chatterPacketDetails.content, CHATTER_FULL_BUFFER_LEN);
      if (chatterPacketDetails.chunkId == 0) {
        chatter->ingestPacketMetadata((const char*)chatterPacketDetails.content, chatterPacketDetails.contentLength);
        memcpy(recipient, chatter->getLastRecipient(), CHATTER_DEVICE_ID_SIZE);
      }

      // send to end recipient
      //logConsole("Recipient will be: " + String(recipient));
      bool sent = false;
      if (ackRequested && airChannel->supportsAck()) {
        // if this is synchronous, limit the number of packets
        maxThisCycle = maxAirSendPacketsPerCycleSync;
        sent = airChannel->send(chatterPacketDetails.content, chatterPacketDetails.contentLength, airChannel->getAddress(recipient));
      }
      else {
        // async packet limit per cycle is different
        maxThisCycle = maxAirSendPacketsPerCycleAsync;
        sent = airChannel->fireAndForget(chatterPacketDetails.content, chatterPacketDetails.contentLength, airChannel->getAddress(recipient));
      }
      if(sent) {
        chatter->getPacketStore()->clearPacketFromAirOut((const char*)chatterPacketDetails.sender, (const char*)chatterPacketDetails.messageId, chatterPacketDetails.packetId);
        if (ackRequested && airChannel->supportsAck()) {
          logConsole("Packet sent via air with ack");
        }
        else {
          logConsole("Packet sent fire and forget via air");
        }
      }
      else {
        logConsole("packet not sent via air");
      }
    }
    else {
      // we have cleared the air queue
      queuedAirTraffic = false;
    }

  }
  return true;
}

bool CommunicatorControlMode::sendPacketBatchBridge () {
  bool packetsToSend = true; //assume there might be a batch to send
  for (int i = 0; packetsToSend && i < maxBridgeSendPacketsPerCycle; i++) {
    packetsToSend = chatter->getPacketStore()->readNextBridgeOutPacketDetails(&chatterPacketDetails);
    if(packetsToSend) {
      //logConsole("Want to send via bridge: " + String((const char*)chatterPacketDetails.messageId));

      // load the packet
      chatterPacketDetails.contentLength = chatter->getPacketStore()->readPacketFromBridgeOut((const char*)chatterPacketDetails.sender, (const char*)chatterPacketDetails.messageId, chatterPacketDetails.packetId, chatterPacketDetails.content, CHATTER_FULL_BUFFER_LEN);
      if(bridgeChannel->send(chatterPacketDetails.content, chatterPacketDetails.contentLength, 0)) {
        logConsole("Packet sent via bridge");
        chatter->getPacketStore()->clearPacketFromBridgeOut((const char*)chatterPacketDetails.sender, (const char*)chatterPacketDetails.messageId, chatterPacketDetails.packetId);
      }
      else {
        logConsole("packet not sent via bridge");
      }
    }
    else {
      // we have cleared the bridge queue
      queuedBridgeTraffic = false;
    }
  }
  return true;
}

bool CommunicatorControlMode::handleEvent (CommunicatorEvent* event) {
  switch (event->EventType) {
    //case UserRequestDirectMessage:
    //  return sendUserText(event);
    case UserChangedTime:
      logConsole("User changed time to: ");
      logConsole((char*)event->EventData);
      chatter->getRtc()->setNewDateTime((const char*)event->EventData);
      showTime();
      break;
  }

  return false;
}

void CommunicatorControlMode::disableMessaging () {
  listeningForMessages = false;

  // put radio in sleep mode
  chatter->getChannel(0)->putToSleep();
}

bool CommunicatorControlMode::onboardNewClient (unsigned long timeout) {
  disableMessaging();
  unsigned long startTime = millis();
  while (millis() - startTime < timeout) {
    if(handleConnectedDevice ()) {
      showClusterOk();
      logConsole("Onboarded a device successfully");
      enableMessaging();
      return true;
    }
  }

  enableMessaging();
  return false;
}

bool CommunicatorControlMode::queueEvent(CommunicatorEventType eventType) {
  // this method can be called safely from interrupts, so dont do anything other than simple assignment operations in here
  queuedEventType = eventType;
  return true;
}

bool CommunicatorControlMode::handleEvent (CommunicatorEventType eventType) {
  switch(eventType) {
    case UserRequestFactoryReset:
      logConsole("FACTORY RESET TRIGGERED");
      factoryResetCheck(true);
      break;
    case UserRequestBleOnboard:
      if(onboardNewClient (CLUSTER_ONBOARD_TIMEOUT)) {
        logConsole("New device onboarded");
      }
      else {
        logConsole("no devices onboarded");
      }

      break;
    case UserRequestBleJoinCluster:
      assistant = new ChatterClusterAssistant(chatter, LORA_RFM9X_CS, LORA_RFM9X_INT, LORA_RFM9X_RST, LORA_CHANNEL_LOG_ENABLED);
      if(assistant->init()) {
        return assistant->attemptOnboard ();
      }
      else {
        logConsole("Assistant failed to init");
      }
      break;
    case UserRequestSelfAnnounce:
      // announce self
      if (chatter->isRunning()) {
        if (chatter->selfAnnounce(true)) {
          showStatus("Presence broadcast");
          return true;
        }
        else {
          showStatus("Broadcast failed");
        }
      }
      else {
        showStatus("No cluster connection");
      }
      break;
    case PingLoraBridge:
    logConsole("Ping lora bridge");
      // send a ping, requesting echo to the bridge
      if (chatter->isRunning()) {
        sendEchoText();
        logConsole("ping sent.");
        return true;
      }
      else {
        showStatus("No cluster connection");
      }
      break;
  }

  return false;
}


bool CommunicatorControlMode::handleConnectedDevice () {
  if (cluster == nullptr) {
    cluster = new ChatterClusterAdminInterface(chatter, LORA_RFM9X_CS, LORA_RFM9X_INT, LORA_RFM9X_RST, LORA_CHANNEL_LOG_ENABLED);
    if (cluster->init()) {
      logConsole("Cluster Admin Interface Initialized");
      return cluster->isConnected();
    }
    else {
      logConsole("Cluster Admin Interface failed to initialize");
      return false;
    }
  }
  else if (cluster->isConnected()) {
    return true;
  }

  return false;
}

void CommunicatorControlMode::logPublicKey () {
  Encryptor* encryptor = chatter->getEncryptor();
  encryptor->loadPublicKey();
  encryptor->hexify(encryptor->getPublicKeyBuffer(), ENC_PUB_KEY_SIZE);
  const char* hexifiedPubKey = encryptor->getHexBuffer();

  Serial.print("Device Public Key: ");
  for (int hexCount = 0; hexCount < ENC_PUB_KEY_SIZE; hexCount++) {
      Serial.print(hexifiedPubKey[hexCount]);
  }
  Serial.println("");
}