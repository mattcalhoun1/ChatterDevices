#include "CommunicatorControlMode.h"

StartupState CommunicatorControlMode::init() {
    logConsole("CommunicatorControlMode Initializing");

    // make sure parent is initialized
    StartupState returnState = ControlMode::init();
    if (returnState == StartupComplete) {
      airChannel = chatter->getChannel(0);
      //bridgeChannel = chatter->getChannel(1);

      logConsole("CommunicatorControlMode running");

      initialized = true;
      
      //logPublicKey();

      // set chatter preferences
      chatter->setKeyForwardingAllowed(isPreferenceEnabled(PreferenceKeyForwarding));
      chatter->setTruststoreLocked(isPreferenceEnabled(PreferenceTruststoreLocked));

      enableMessaging();

      remoteConfigEnabled = chatter->getDeviceStore()->getRemoteConfigEnabled();
      if (remoteConfigEnabled) {
        logConsole("Alert: Remote config is enabled!");
      }
    }

    return returnState;
}

void CommunicatorControlMode::loop () {
  if (!initialized) {
    logConsole("Messaging not ready.");
    showClusterError();
  }

  if (queuedEventType != CommunicatorEventNone) {
    // handle the event
    handleEvent(queuedEventType);

    // clear the queued event
    queuedEventType = CommunicatorEventNone;
  }

  int numPacketsThisCycle = 0;

  if (listeningForMessages) {
    while (chatter->hasMessage() && numPacketsThisCycle++ < maxReadPacketsPerCycle) {
      showStatus("Receiving");
      if(chatter->retrieveMessage() && chatter->getMessageType() == MessageTypeComplete) {
        logConsole("Processing trusted message...");
        messageBufferLength = chatter->getMessageSize();
        memcpy(messageBuffer, chatter->getTextMessage(), messageBufferLength);
        messageBuffer[messageBufferLength] = 0;
        memcpy(otherDeviceId, chatter->getLastSender(), CHATTER_DEVICE_ID_SIZE);
        otherDeviceId[CHATTER_DEVICE_ID_SIZE] = 0;

        CommunicatorEvent evt;
        memcpy(evt.EventTarget, chatter->getLastSender(), CHATTER_DEVICE_ID_SIZE);

        // send ack (later, queue this)
        if (!chatter->isAcknowledgement()) {
          if (memcmp(chatter->getLastRecipient(), chatter->getDeviceId(), CHATTER_DEVICE_ID_SIZE) == 0) {
            logConsole("sending ack..");
            if(!chatter->sendAck(otherDeviceId, chatter->getMessageId())) {
              logConsole("Ack direct failed");
              if (deviceMeshEnabled && chatter->clusterSupportsMesh()) {
                chatter->sendAckViaMesh(otherDeviceId, chatter->getMessageId());
              }
            }

            if (isRemoteConfig(messageBuffer, messageBufferLength)) {
              Serial.print("Received remote command: ");
              for (uint8_t i = 0; i < messageBufferLength; i++) {
                Serial.print((char)messageBuffer[i]);
              }
              Serial.println("");
              evt.EventType = RemoteConfigReceived;
              evt.EventDataLength = messageBufferLength;
              memcpy(evt.EventData, messageBuffer, messageBufferLength);
            }
            else if (isBackpackRequest(messageBuffer, messageBufferLength)) {
              Serial.print("backpack request received: ");
              for (uint8_t i = 0; i < messageBufferLength; i++) {
                Serial.print((char)messageBuffer[i]);
              }
              Serial.println("");
              evt.EventType = RemoteBackpackRequestReceived;
              evt.EventDataLength = messageBufferLength;
              memcpy(evt.EventData, messageBuffer, messageBufferLength);
            }
            else {
              evt.EventType = MessageReceived;
            }
          }
          else {
            evt.EventType = BroadcastReceived;
          }

        }
        else {
          evt.EventType = AckReceived;
        }
        handleEvent(&evt);
      }
    }
    showReady();
  }

  // sync every loop, strategy decides how often
  chatter->syncMesh(MeshCycleFull);

  if (loopCount % 10 == 0) {
    //Serial.print("Free Memory: "); Serial.println(freeMemory());
    showTime();
  }
  else if (sendText) {
    showBusy();
    sendEchoText();
    sendText = false;
  }

  if (RTC_SYNC_ENABLED == true && loopCount % RTC_SYNC_FREQUENCY == 0) {
    rtc->syncWithExternalRtc();
  }

  sleepOrBackground(50);
  loopCount++;
}

bool CommunicatorControlMode::isRemoteConfig (const uint8_t* msg, int msgLength) {
  return msgLength >= 5 && memcmp("CFG:", msg, 4) == 0;
}

bool CommunicatorControlMode::isBackpackRequest (const uint8_t* msg, int msgLength) {
  return msgLength >= 4 && memcmp("BK:", msg, 3) == 0;
}


bool CommunicatorControlMode::executeRemoteConfig (CommunicatorEvent* event) {
  switch (event->EventData[4]) {
    case RemoteConfigBattery:
      logConsole("Battery level requested");

      // grab the battery level
      sprintf((char*)messageBuffer, "%s %03d", "Battery:", getBatteryLevel());
      Serial.print("Sending: ");Serial.println((const char*)messageBuffer);

      // send to requestor
      if(!chatter->send(messageBuffer, strlen((char*)messageBuffer), event->EventTarget)) {
        logConsole("Send direct failed");
        if (deviceMeshEnabled && chatter->clusterSupportsMesh()) {
          ChatterMessageFlags flags;
          chatter->sendViaMesh(messageBuffer, strlen((char*)messageBuffer), event->EventTarget, &flags);
        }
      }

      return true;
    case RemoteConfigPath:
      logConsole("Path requested");

      if (event->EventDataLength == 5+CHATTER_DEVICE_ID_SIZE) {
        memset(otherDeviceId, 0, CHATTER_DEVICE_ID_SIZE+1);
        memcpy(otherDeviceId, event->EventData+5, CHATTER_DEVICE_ID_SIZE);
        logConsole("Finding path to: ", otherDeviceId);
        populateMeshPath(otherDeviceId);

        Serial.print("Sending: ");Serial.println((const char*)messageBuffer);

        // send to requestor
        if(!chatter->send(messageBuffer, strlen((char*)messageBuffer), event->EventTarget)) {
          logConsole("Send direct failed");
          if (deviceMeshEnabled && chatter->clusterSupportsMesh()) {
            ChatterMessageFlags flags;
            chatter->sendViaMesh(messageBuffer, strlen((char*)messageBuffer), event->EventTarget, &flags);
          }
        }
      }
      else {
        logConsole("invalid path request");
      }

      return true;
    case RemoteConfigMeshCacheClear:
      logConsole("Remote mesh cache clear");
      handleEvent(UserRequestClearMeshCache);

      return true;
    case RemoteConfigMeshGraphClear:
      logConsole("Remote mesh graph clear");
      handleEvent(UserRequestClearMeshGraphNoPrompt);
      return true;
    case RemoteConfigPingTableClear:
      logConsole("Remote ping table clear");
      handleEvent(UserRequestClearPingTable);
      return true;
    case RemoteConfigEnableLearn:
      logConsole("Remote enable of learn mode");
      if (!chatter->getDeviceStore()->getMeshLearningEnabled()) {
        chatter->getDeviceStore()->setMeshLearningEnabled(true);
      }
      return true;
    case RemoteConfigDisableLearn:
      logConsole("Remote disable of learn mode");
      if (chatter->getDeviceStore()->getMeshLearningEnabled()) {
        chatter->getDeviceStore()->setMeshLearningEnabled(false);
      }
      return true;
    case RemoteConfigMessagesClear:
      logConsole("Remote clearing messages");
      handleEvent(UserDeleteAllMessagesNoPrompt);
  }

  logConsole("unknown remote config");
  return false;
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
  #if !defined(ADAFRUIT_FEATHER_M4_EXPRESS)
  LowPower.deepSleep();
  #endif
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
  int size;
  switch (event->EventType) {
    //case UserRequestDirectMessage:
    //  return sendUserText(event);
    case UserChangedTime:
      logConsole("User changed time to: ");
      logConsole((char*)event->EventData);
      chatter->getRtc()->setNewDateTime((const char*)event->EventData);
      showTime();
      break;
    case MessageReceived:
      if (BACKPACK_ENABLED) {
        // if backpack enabled, notify the backpack
        Serial1.print("MSG:");
        Serial1.print(chatter->getLastSender());
        size = chatter->getMessageSize();
        Serial1.write((byte*)&size, 2);
        for (int i = 0; i < size; i++) {
          Serial1.print(chatter->getTextMessage()[i]);
        }
        Serial1.print('\n');
      }
      break;
    case AckReceived:
      if (BACKPACK_ENABLED) {
        Serial1.print("ACK:");
        Serial1.print(chatter->getLastSender());
        size = chatter->getMessageSize();
        Serial1.write((byte*)&size, 2);
        for (int i = 0; i < size; i++) {
          Serial1.print(chatter->getTextMessage()[i]);
        }
        Serial1.print('\n');
      }
      break;
    case RemoteConfigReceived:
      if (remoteConfigEnabled) {
        if (chatter->isRootDevice(event->EventTarget)) {
          return executeRemoteConfig(event);
        }
        else {
          logConsole("Receoved remote cfg from nonroot, ignoring");
        }
      }
      else {
        logConsole("Received remote cfg, not enabled");
      }
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
  bool isConnected = false;
  bool isComplete = false;
  bool isTrying = true;

  switch(eventType) {
    case UserRequestQuickFactoryReset:
      logConsole("FACTORY RESET TRIGGERED");
      factoryResetCheck(true);
      break;
    case UserRequestSecureFactoryReset:
      logConsole("FACTORY RESET TRIGGERED");
      factoryResetCheck(true, true);
      break;
    case UserRequestOnboard:
      if(onboardNewClient (CLUSTER_ONBOARD_TIMEOUT)) {
        logConsole("New device onboarded");
      }
      else {
        logConsole("no devices onboarded");
      }

      break;
    case UserRequestJoinCluster:
      assistant = new ChatterClusterAssistant(chatter, LORA_RFM9X_CS, LORA_RFM9X_INT, LORA_RFM9X_RST, LORA_RFM9X_BUSY, LORA_CHANNEL_LOG_ENABLED, STRONG_ENCRYPTION_ENABLED);
      if(assistant->init()) {
        assistant->beginOnboarding();

        while (isComplete == false && isTrying == true) {
          // step forward in onbaord
          assistant->onboardNextStep();
          isConnected = assistant->isConnected();
          isComplete = assistant->isOnboardComplete();
          isTrying = assistant->isOnboardingInProgress();

          if (isConnected == false) {
            updateChatStatus("Waiting for Connect");
          }

        }

        if (isComplete) {
          // queue the mesh data to get cleared on next startup
          chatter->getDeviceStore()->setClearMeshOnStartup(true);
        }

        return isComplete;
      }
      else {
        logConsole("Assistant failed to init");
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
    cluster = new ChatterClusterAdminInterface(chatter, LORA_RFM9X_CS, LORA_RFM9X_INT, LORA_RFM9X_RST, LORA_RFM9X_BUSY, LORA_CHANNEL_LOG_ENABLED, STRONG_ENCRYPTION_ENABLED);
    if (cluster->init()) {
      logConsole("Cluster Admin Interface Initialized");
      return cluster->onboardSynchronous();
    }
    else {
      logConsole("Cluster Admin Interface failed to initialize");
      return false;
    }
  }
  else if (cluster->onboardSynchronous()) {
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

void CommunicatorControlMode::populateMeshPath (const char* recipientId) {
  meshPathLength = chatter->findMeshPath (chatter->getDeviceId(), recipientId, meshPath);

  // copy the path into the message buffer so we can display
  memset(messageBuffer, 0, GUI_MAX_MESSAGE_LENGTH+1);
  uint8_t* pos = messageBuffer;

  if (meshPathLength > 0) {
    for (uint8_t p = 0; p < meshPathLength; p++) {
      memset(meshDevIdBuffer, 0, CHATTER_DEVICE_ID_SIZE + 1);
      memset(meshAliasBuffer, 0, CHATTER_ALIAS_NAME_SIZE + 1);

      // find cluster device with that address
      chatter->loadDeviceId(meshPath[p], meshDevIdBuffer);

      // laod from truststore
      if (chatter->getTrustStore()->loadAlias(meshDevIdBuffer, meshAliasBuffer)) {
        memcpy(pos, meshAliasBuffer, strlen(meshAliasBuffer));
        pos += strlen(meshAliasBuffer);
      }
      else {
        sprintf((char*)pos, "%c%s%c", '[', meshDevIdBuffer, ']');
        pos += (2 + CHATTER_DEVICE_ID_SIZE);
      }

      if (p+1 < meshPathLength) {
        memcpy(pos, " -> ", 4);
        pos += 4;
      }
    }
  }
  else {
    memcpy(pos, "no path!", 8);
  }  
}

void CommunicatorControlMode::notifyMessageReceived() {
  logConsole("Message received");
}

uint8_t CommunicatorControlMode::getBatteryLevel () {
  #if defined(VBATPIN)
    float measuredvbat = analogRead(VBATPIN);
    measuredvbat *= 2;    // we divided by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024; // convert to voltage
    //Serial.print("VBat: " ); Serial.println(measuredvbat);
    if (measuredvbat >= 4.1) {
      return 100;
    }
    else if (measuredvbat >= 4.0) {
      return 90;
    }
    else if (measuredvbat >= 3.8) {
      return 70;
    }
    else if (measuredvbat >= 3.7) {
      return 50;
    }
    else if (measuredvbat >= 3.5) {
      return 30;
    }
    else {
      return 15;
    }
  #else
    return 99;
  #endif
}

bool CommunicatorControlMode::isPreferenceEnabled (CommunicatorPreference pref) {
  switch (pref) {
    case PreferenceMessageHistory:
      return chatter->getDeviceStore()->getMessageHistoryEnabled();
    case PreferenceKeyboardLandscape:
      return chatter->getDeviceStore()->getKeyboardOrientedLandscape();
    case PreferenceWifiEnabled:
      return chatter->getDeviceStore()->getWifiEnabled();
    case PreferenceMeshEnabled:
      return chatter->getDeviceStore()->getMeshEnabled();
    case PreferenceWiredEnabled:
      return chatter->getDeviceStore()->getUartEnabled();
    case PreferenceLoraEnabled:
      return chatter->getDeviceStore()->getLoraEnabled();
    case PreferenceMeshLearningEnabled:
      return chatter->getDeviceStore()->getMeshLearningEnabled();
    case PreferenceRemoteConfigEnabled:
      return chatter->getDeviceStore()->getRemoteConfigEnabled();
    case PreferenceDstEnabled:
      return chatter->getDeviceStore()->getDstEnabled();
    case PreferenceIgnoreExpiryEnabled:
      return chatter->getDeviceStore()->getAllowExpiredMessages();
    case PreferenceBackpacksEnabled:
      return chatter->getDeviceStore()->getCustomPreference(StoredPrefBackpacksEnabled) == 'T';
    case PreferenceBackpackThermalEnabled:
      return chatter->getDeviceStore()->getCustomPreference(StoredPrefBackpackThermalEnabled) == 'T';
    case PreferenceBackpackThermalRemoteEnabled:
      return chatter->getDeviceStore()->getCustomPreference(StoredPrefBackpackThermalRemoteEnabled) == 'T';
    case PreferenceBackpackThermalAutoEnabled:
      return chatter->getDeviceStore()->getCustomPreference(StoredPrefBackpackThermalAutoEnabled) == 'T';
    case PreferenceBackpackRelayEnabled:
      return chatter->getDeviceStore()->getCustomPreference(StoredPrefBackpackRelayEnabled) == 'T';
    case PreferenceBackpackRelayRemoteEnabled:
      return chatter->getDeviceStore()->getCustomPreference(StoredPrefBackpackRelayRemoteEnabled) == 'T';
    case PreferenceTruststoreLocked:
      return chatter->getDeviceStore()->getCustomPreference(StoredPrefTruststoreLocked) == 'T';
    case PreferenceKeyForwarding:
      return chatter->getDeviceStore()->getCustomPreference(StoredPrefKeyForwarding) != 'F';
  }

  logConsole("Unknown preference read attempt");
  return false;
}

void CommunicatorControlMode::enablePreference (CommunicatorPreference pref) {
  switch (pref) {
    case PreferenceMessageHistory:
      chatter->getDeviceStore()->setMessageHistoryEnabled(true);
      logConsole("Message history enabled");

      // reset device
      restartDevice();

      break;
    case PreferenceKeyboardLandscape:
      chatter->getDeviceStore()->setKeyboardOrientedLandscape(true);

      logConsole("Keyboard landscape enabled");
      restartDevice();
      break;
    case PreferenceWifiEnabled:
      chatter->getDeviceStore()->setWifiEnabled(true);
      logConsole("Wifi enabled");

      // reset device
      restartDevice();

      break;
    case PreferenceMeshEnabled:
      chatter->getDeviceStore()->setMeshEnabled(true);
      logConsole("Mesh enabled");

      // reset device
      restartDevice();

      break;
    case PreferenceWiredEnabled:
      chatter->getDeviceStore()->setUartEnabled(true);
      logConsole("Wired enabled");
      // reset device
      restartDevice();
      break;
    case PreferenceLoraEnabled:
      chatter->getDeviceStore()->setLoraEnabled(true);
      logConsole("Lora enabled");
      // reset device
      restartDevice();
      break;
    case PreferenceMeshLearningEnabled:
      chatter->getDeviceStore()->setMeshLearningEnabled(true);
      break;
    case PreferenceRemoteConfigEnabled:
      chatter->getDeviceStore()->setRemoteConfigEnabled(true);

      // reset device
      restartDevice();
      break;
    case PreferenceDstEnabled:
      chatter->getDeviceStore()->setDstEnabled(true);

      restartDevice();
      break;
    case PreferenceIgnoreExpiryEnabled:
      chatter->getDeviceStore()->setAllowExpiredMessages(true);
      break;
    case PreferenceBackpacksEnabled:
      chatter->getDeviceStore()->setCustomPreference(StoredPrefBackpacksEnabled, 'T');
      restartDevice();
      break;

    case PreferenceBackpackThermalEnabled:
      chatter->getDeviceStore()->setCustomPreference(StoredPrefBackpackThermalEnabled, 'T');
      restartDevice();
      break;

    case PreferenceBackpackThermalRemoteEnabled:
      chatter->getDeviceStore()->setCustomPreference(StoredPrefBackpackThermalRemoteEnabled, 'T');
      restartDevice();
      break;

    case PreferenceBackpackThermalAutoEnabled:
      chatter->getDeviceStore()->setCustomPreference(StoredPrefBackpackThermalAutoEnabled, 'T');
      restartDevice();
      break;

    case PreferenceBackpackRelayEnabled:
      chatter->getDeviceStore()->setCustomPreference(StoredPrefBackpackRelayEnabled, 'T');
      restartDevice();
      break;

    case PreferenceBackpackRelayRemoteEnabled:
      chatter->getDeviceStore()->setCustomPreference(StoredPrefBackpackRelayRemoteEnabled, 'T');
      restartDevice();
      break;

    case PreferenceTruststoreLocked:
      chatter->getDeviceStore()->setCustomPreference(StoredPrefTruststoreLocked, 'T');
      chatter->setTruststoreLocked(true);
      break;

    case PreferenceKeyForwarding:
      chatter->getDeviceStore()->setCustomPreference(StoredPrefKeyForwarding, 'T');
      chatter->setKeyForwardingAllowed(true);
      break;

    default:
      logConsole("Unknown preference enable attempt");
  }
}

void CommunicatorControlMode::disablePreference (CommunicatorPreference pref) {
  switch (pref) {
    case PreferenceMessageHistory:
      chatter->getDeviceStore()->setMessageHistoryEnabled(false);
      logConsole("Message history disabled");

      // reset device
      restartDevice();
      break;
    case PreferenceKeyboardLandscape:
      chatter->getDeviceStore()->setKeyboardOrientedLandscape(false);

      logConsole("Keyboard landscape disabled");
      restartDevice();
      break;
    case PreferenceWifiEnabled:
      chatter->getDeviceStore()->setWifiEnabled(false);
      logConsole("Wifi disabled");

      // reset device
      restartDevice();

      break;
    case PreferenceMeshEnabled:
      chatter->getDeviceStore()->setMeshEnabled(false);
      logConsole("Mesh disabled");

      // reset device
      restartDevice();

      break;
    case PreferenceLoraEnabled:
      chatter->getDeviceStore()->setLoraEnabled(false);
      logConsole("Lora disabled");

      // reset device
      restartDevice();

      break;
    case PreferenceWiredEnabled:
      chatter->getDeviceStore()->setUartEnabled(false);
      logConsole("Wired disabled");

      // reset device
      restartDevice();

      break;
    case PreferenceMeshLearningEnabled:
      chatter->getDeviceStore()->setMeshLearningEnabled(false);
      break;

    case PreferenceRemoteConfigEnabled:
      chatter->getDeviceStore()->setRemoteConfigEnabled(false);

      // reset device
      restartDevice();

      break;
    case PreferenceDstEnabled:
      chatter->getDeviceStore()->setDstEnabled(false);

      restartDevice();
      break;
    case PreferenceIgnoreExpiryEnabled:
      chatter->getDeviceStore()->setAllowExpiredMessages(false);
      break;

    case PreferenceBackpacksEnabled:
      chatter->getDeviceStore()->setCustomPreference(StoredPrefBackpacksEnabled, 'F');
      restartDevice();
      break;

    case PreferenceBackpackThermalEnabled:
      chatter->getDeviceStore()->setCustomPreference(StoredPrefBackpackThermalEnabled, 'F');
      restartDevice();
      break;

    case PreferenceBackpackThermalRemoteEnabled:
      chatter->getDeviceStore()->setCustomPreference(StoredPrefBackpackThermalRemoteEnabled, 'F');
      restartDevice();
      break;

    case PreferenceBackpackThermalAutoEnabled:
      chatter->getDeviceStore()->setCustomPreference(StoredPrefBackpackThermalAutoEnabled, 'F');
      restartDevice();
      break;

    case PreferenceBackpackRelayEnabled:
      chatter->getDeviceStore()->setCustomPreference(StoredPrefBackpackRelayEnabled, 'F');
      restartDevice();
      break;

    case PreferenceBackpackRelayRemoteEnabled:
      chatter->getDeviceStore()->setCustomPreference(StoredPrefBackpackRelayRemoteEnabled, 'F');
      restartDevice();
      break;

    case PreferenceTruststoreLocked:
      chatter->getDeviceStore()->setCustomPreference(StoredPrefTruststoreLocked, 'F');
      chatter->setTruststoreLocked(false);
      break;

    case PreferenceKeyForwarding:
      chatter->getDeviceStore()->setCustomPreference(StoredPrefKeyForwarding, 'F');
      chatter->setKeyForwardingAllowed(false);
      break;

    default:
      logConsole("Unknown preference disable attempt");
  }
}
