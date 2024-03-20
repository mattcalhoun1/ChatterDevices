#include "BridgeControlMode.h"

bool BridgeControlMode::init() {
    logConsole("BridgeControlMode Initializing");

    // make sure parent is initialized
    if (ControlMode::init()) {
      airChannel = chatter->getChannel(0);
      bridgeChannel = chatter->getChannel(1);

      showTitle("Bridge");
      logConsole("BridgeControlMode running");
      showTime();
      showStatus("Ready");
      initialized = true;
    }

    return initialized;
}

void BridgeControlMode::loop () {
  if (justOnboarded) {
    // we need a restart, freeze on the checkmark
    logConsole("need restart");
    showClusterOk();
    delay(1000);

    NVIC_SystemReset();
  }
  else if (!initialized) {
    logConsole("not init");
    showClusterError();
    while(true) {
      delay(100);
    }
  }

  //showTime();
  int numPacketsThisCycle = 0;

  while (chatter->hasMessage() && numPacketsThisCycle++ < maxReadPacketsPerCycle) {
    showStatus("Receiving");
    if(chatter->retrieveMessage() && chatter->getMessageType() == MessageTypeComplete) {
      int messageLength = chatter->getMessageSize();
      logConsole("Processing trusted message...");

      // bridge channel means it came from the other bridge device, and should just be broadcast out blindly
      if (strcmp(chatter->getLastChannel()->getName(), BRIDGE_CHANNEL) == 0) {
        // move to out folder, under appropriate recipient folder
        // /msg/air_out/recipient/id/chunk...
        logConsole("Received message FROM bridge channel");
        chatter->getPacketStore()->moveMessageToAirOut((const char*)chatter->getLastSender(), (const char*)chatter->getLastPacketMessageId());
        chatter->getPacketStore()->saveAirMessageTimestamp((const char*)chatter->getLastSender(), (const char*)chatter->getLastPacketMessageId(), rtc->getSortableTime());
        queuedAirTraffic = true;
      }
      else {
        switch (chatter->getMessageFlags().Flag1) {
          case BridgeRequestBridge:
            logConsole("Bridge requested...");
            // move the message to the bridge out folder
            chatter->getPacketStore()->moveMessageToBridgeOut((const char*)chatter->getLastSender(), (const char*)chatter->getLastPacketMessageId());
            chatter->getPacketStore()->saveBridgeMessageTimestamp((const char*)chatter->getLastSender(), (const char*)chatter->getLastPacketMessageId(), rtc->getSortableTime());
            queuedBridgeTraffic = true;

            break;
          case BridgeRequestEcho:
            logConsole("Echo requested...");
            // move the message to air_out 
            chatter->getPacketStore()->moveMessageToAirOut((const char*)chatter->getLastSender(), (const char*)chatter->getLastPacketMessageId());
            chatter->getPacketStore()->saveAirMessageTimestamp((const char*)chatter->getLastSender(), (const char*)chatter->getLastPacketMessageId(), rtc->getSortableTime());
            queuedAirTraffic = true;
            break;
          default:
            logConsole("Unsure what to do with this message??");
            Serial.print("Flag: ");Serial.println(chatter->getMessageFlags().Flag1);
        }
      }
    }
  }

  if (loopCount % 10 == 0) {
    showBusy();

    showTime();
    //Serial.print("Free Memory: "); Serial.println(freeMemory());
  }

  if (loopCount % RTC_SYNC_FREQUENCY == 0) {
    //logConsole("syncing rtc");
    //rtc->syncWithExternalRtc();
  }

  if (lastAnnounce == 0 || millis() - lastAnnounce > announceFrequency) {
    lastAnnounce = millis();
    chatter->selfAnnounce (true);
  }

  sleepOrBackground(100);
  loopCount++;
}

void BridgeControlMode::sleepOrBackground(unsigned long sleepTime) {
  unsigned long startTime = millis();

  if (loopCount % MESSAGE_PRUNE_FREQUENCY == 0) {
    int pruneCount = chatter->getPacketStore()->pruneAgedMessages(rtc->getSortableTimePlusSeconds(-1 * MAX_MESSAGE_AGE_SECONDS));
    if (pruneCount > 0) {
      logConsole("Pruned " + String(pruneCount) + " messages");
    }
  }

  if (queuedAirTraffic || queuedBridgeTraffic) {
    while (millis() - startTime < sleepTime) {
      if (loopCount % 3 == 0) {
        // send out a batch of packets
        if (loopCount % 2) {
          sendPacketBatchAir();
        }
        // temp commented out until we start bridging again
        /*else {
          sendPacketBatchBridge();
        }*/
      }
    }
    delay(10);
  }
  else {
    delay(sleepTime);
  }
}

bool BridgeControlMode::sendPacketBatchAir () {
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
      logConsole("Sending packet size from air out: " + String(chatterPacketDetails.contentLength));
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
}

bool BridgeControlMode::sendPacketBatchBridge () {
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
}
