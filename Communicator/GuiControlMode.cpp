#include "GuiControlMode.h"

bool GuiControlMode::init() {
    logConsole("GuiControlMode Initializing");
    rotary = new RotaryEncoder(PIN_ROTARY_IN1, PIN_ROTARY_IN2, RotaryEncoder::LatchMode::TWO03);

    // make sure parent is initialized
    bool parentInitialized = HeadsUpControlMode::init();

    // setup the menu
    menu = new Menu((MenuEnabledDisplay*)display, rotary, this, chatter->isRootDevice(chatter->getDeviceId()));
    menu->init();

    // add self as a touch listener
    if (display->isTouchEnabled()) {
      ((TouchEnabledDisplay*)display)->addTouchListener(this);
    }

    if (parentInitialized) {
      deviceIterator = new DeviceAliasIterator(chatter->getTrustStore());

      // load message history, if allowed/configured
      messageIterator = new MessageIterator(chatter->getMessageStore(), chatter->getTrustStore());

      memset(title, 0, 32);
      sprintf(title, "%s @ %s", chatter->getDeviceAlias(), chatter->getClusterAlias());
      showTitle(title);
      showStatus("Ready");

      showMessageHistory(true);
    }

    return parentInitialized;
}

void GuiControlMode::loop () {
  // scroll message
  updateMessagePreviewsIfNecessary();

  // check for input
  menu->menuUpdate();

  // if the user is interacting, skip the main loop
  if (!menu->isActive()) {
    if (fullRepaint) {
      fullRepaint = false;
      display->clearAll();
      showTitle(title);
      showTime();
      showMessageHistory(true);
      showReady();
    }

    // execute main loop
    HeadsUpControlMode::loop();
  }
}

void GuiControlMode::showMessageHistory(bool resetOffset) {
  //logConsole("=== Current Message History ===");

  if (resetOffset) {
    // reload from message store
    messageIterator->init(chatter->getClusterId(), chatter->getDeviceId(), true);
    //sprintf(messagePreviewBuffer, "%s Messages: %d", chatter->getClusterId(), messageIterator->getNumItems());

    // set the offset (if necessary) so that the newest messages are shown by default
    // and the user would have to scroll up to see older ones
    messageHistorySize = messageIterator->getNumItems();
    if (messageIterator->getNumItems() > display->getMaxDisplayableMessages()) {
      messagePreviewOffset = messageIterator->getNumItems() - display->getMaxDisplayableMessages();
    }
  }

  for (int msg = messagePreviewOffset; msg < messageIterator->getNumItems(); msg++) {
    memset(messageTitleBuffer, 0, MESSAGE_TITLE_BUFFER_SIZE + 1);
    memset(messagePreviewBuffer, 0, MESSAGE_PREVIEW_BUFFER_SIZE + 1);
    messageIterator->loadItemName(msg, messageTitleBuffer);

    //logConsole(messageTitleBuffer);

    // if it's a small message, go ahead and print. otherwise, user will have to look
    if (messageIterator->isPreviewable(msg)) {
      chatter->getMessageStore()->loadMessage (messageIterator->getItemVal(msg), (uint8_t*)messagePreviewBuffer, MESSAGE_PREVIEW_BUFFER_SIZE);
    }
    else {
      sprintf(messagePreviewBuffer, "%s", "[large message]");
    }
    //logConsole(messagePreviewBuffer);
    display->showMessageAndTitle(messageTitleBuffer+3, messagePreviewBuffer, messageTitleBuffer[0] == SentViaBroadcast ? Blue : Yellow, White, msg - messagePreviewOffset);
  }

  //logConsole("=== End Message History === ");
}

bool GuiControlMode::handleEvent (CommunicatorEventType eventType) {
  bool result = false;
  bool sentViaBridge = false;
  switch(eventType) {
    case MenuClosed:
      fullRepaint = true;
      return true;
    case UserRequestDirectMessage:
      memset(otherDeviceId, 0, CHATTER_DEVICE_ID_SIZE+1);
      if (isFullyInteractive()) {
        // select a recipient (or broadcast)
        deviceIterator->init(chatter->getClusterId(), chatter->getDeviceId(), true);
        menu->setItemIterator(deviceIterator);
        menu->iteratorMenu(); // modal

        // wait for result of contact selections
        while (menu->isActive()) {
          menu->menuUpdate();
          delay(10);
        }

        if (menu->getIteratorSelection() != ITERATOR_SELECTION_NONE) {
          // find the associated device id for the given slot
          uint8_t selectedSlot = deviceIterator->getItemVal(menu->getIteratorSelection());
          chatter->getTrustStore()->loadDeviceId(selectedSlot, otherDeviceId);
          logConsole("User chose to DM :");
          logConsole(otherDeviceId);

          // fall through to the other send message logic
        }
        else {
          logConsole("Canceled");
          return false;
        }
      }

    case UserRequestSecureBroadcast:
    case UserRequestOpenBroadcast:
      if (isFullyInteractive()) {
        display->clearAll();

        // pop up the keyboard
        messageBufferLength = ((FullyInteractiveDisplay*)display)->getModalInput("Message", 120, CharacterFilterNone, (char*)messageBuffer);

        // send it
        if(messageBufferLength > 0) {
          if (eventType == UserRequestSecureBroadcast) {
            display->showAlert("Broadcast", AlertActivity);
            result = chatter->broadcast(messageBuffer, messageBufferLength);
          }
          else if (eventType == UserRequestOpenBroadcast) {
            display->showAlert("Open Broadcast", AlertActivity);
            result = chatter->broadcastUnencrypted(messageBuffer, messageBufferLength, nullptr);
          }
          else {
            display->showAlert("Send DM", AlertActivity);
            result = sendDirectMessage();

            if (!result && chatter->clusterHasDevice(ChatterDeviceBridgeLora)) {
              sentViaBridge = sendViaBridge();
            }
          }

          if (sentViaBridge) {
            display->showAlert("Sent (Bridge)", AlertWarning);
          }
          else if(result) {
            display->showAlert("Sent", AlertSuccess);
          } 
          else {
            display->showAlert("Not Sent!", AlertError);
          }

          // full repaint
          delay(1000);
          fullRepaint = true;
          return result;
        }

        fullRepaint = true;
        return false;
      }
      break;
    case UserRequestSelfAnnounce:
      // show alert
      display->showAlert("Announcing", AlertActivity);

      // let base handle, show result
      result = HeadsUpControlMode::handleEvent(eventType);
      if (result) {
        display->showAlert("Sent", AlertSuccess);
      }
      else {
        display->showAlert("Not Sent", AlertError);
      }
      delay(2000);// give user a chance to see
      fullRepaint = true;
      return result;
    case UserRequestBleJoinCluster:
      display->showAlert("Join Cluster", AlertActivity);

      // disable message receiving, put the radio in sleep mode


      result = HeadsUpControlMode::handleEvent(eventType);
      if (result) {
        display->showAlert("Joined", AlertSuccess);
        delay(2000);
        restartDevice();
      }
      else {
        display->showAlert("Not Found", AlertError);
        delay(2000);// give user a chance to see
      }

      fullRepaint = true;
      return result;
    case UserDeleteAllMessages:
      if (isFullyInteractive()) {
        int newMessageLength = ((FullyInteractiveDisplay*)display)->getModalInput("Delete All (y/n)", 10, CharacterFilterAlpha, (char*)messageBuffer);
        if (newMessageLength > 0 && (messageBuffer[0] == 'y' || messageBuffer[0] == 'Y')) {
          chatter->getMessageStore()->clearAllMessages();
          fullRepaint = true;
        }
        else {
          logConsole("delete all canceled.");
          fullRepaint = true;
        }
      }
      return true;

    case UserRequestFactoryReset:
      if (isFullyInteractive()) {
        int newMessageLength = ((FullyInteractiveDisplay*)display)->getModalInput("Confirm (y/n)", 10, CharacterFilterAlpha, (char*)messageBuffer);
        if (newMessageLength > 0 && (messageBuffer[0] == 'y' || messageBuffer[0] == 'Y')) {
          logConsole("Factory reset confirmed");
          fullRepaint = true;
        }
        else {
          logConsole("Factory reset cancelled");
          fullRepaint = true;
          return true;
        }
      }
  }

  // let base class handle
  return HeadsUpControlMode::handleEvent(eventType);
}

bool GuiControlMode::handleScreenTouched (int touchX, int touchY) {
  if (messageHistorySize > 0) {
    uint8_t selectedMessage = display->getMessagePosition(touchX, touchY);
    if (selectedMessage != DISPLAY_MESSAGE_POSITION_NULL && selectedMessage < messageHistorySize) {
      uint8_t selectedMessageSlot = messageIterator->getItemVal(selectedMessage + messagePreviewOffset);
      Serial.print("Selected message at fram slot: "); Serial.println(selectedMessageSlot);
      
      // queue a reply event to that message slot
    }
  }

  return true;
}

void GuiControlMode::showLastMessage () {
  //display->showMessage((const char*)messageBuffer, Green, 0);
  // refresh the message history if enabled
  showMessageHistory(true);
}


bool GuiControlMode::sendDirectMessage () {
  logConsole("Sending DM..");
  ChatterMessageFlags flags;
  flags.Flag0 = EncodingTypeText;
  flags.Flag2 = AckRequestTrue;

  if (chatter->send(messageBuffer, messageBufferLength, otherDeviceId, &flags)) {
    logConsole("DM sent");
    return true;
  }

  logConsole("DM not sent");
  return false;
}

bool GuiControlMode::sendViaBridge() {
 logConsole("Sending via bridge..");
  ChatterMessageFlags flags;
  flags.Flag0 = EncodingTypeText;
  flags.Flag1 = BridgeRequestEcho;//BridgeRequestEcho / BridgeRequestBridge
  flags.Flag2 = AckRequestTrue;

  // look up via name
  char fullDeviceId[CHATTER_DEVICE_ID_SIZE+1];
  memcpy(fullDeviceId, chatter->getDeviceId(), CHATTER_DEVICE_ID_SIZE);
  memcpy(fullDeviceId + CHATTER_LOCAL_NET_ID_SIZE + CHATTER_GLOBAL_NET_ID_SIZE, BRIDGE_LORA_DEVICE_ID, 3);
  fullDeviceId[CHATTER_DEVICE_ID_SIZE] = '\0';

  // attempt to bridge or echo a message to myself
  //      sendViaIntermediary(uint8_t *message, int length, const char* recipientDeviceId, const char* intermediaryDeviceId, ChatterMessageFlags* flags, ChatterChannel* channel);
  if(chatter->sendViaIntermediary(messageBuffer, messageBufferLength, otherDeviceId, fullDeviceId, &flags, chatter->getChannel(lastChannel))) { // send a message to the bridge
    logConsole("Message successfully sent via bridge");
  }
  else {
    logConsole("Message not sent!");
  }
}

void GuiControlMode::buttonAPressed () {
  menu->notifyButtonPressed();
  //sendText = true;
}

bool GuiControlMode::updateMessagePreviewsIfNecessary () {
  // get current rotary position
  if (rotaryMoved) {
    rotaryMoved = false;
    int newRotaryPostion = rotary->getPosition();
    if (newRotaryPostion > selection) {
      if (messagePreviewOffset + display->getMaxDisplayableMessages() < messageHistorySize) {
        messagePreviewOffset++;
        showMessageHistory(false);
      }
    }
    else if (newRotaryPostion < selection) {
      // if the offset is > 0, decrement
      if (messagePreviewOffset > 0) {
        messagePreviewOffset--;
        showMessageHistory(false);
      }
    }
    selection = newRotaryPostion;
    return true;
  }

  return false;
}

void GuiControlMode::handleRotary () {
  rotary->tick(); // just call tick() to check the state.
  if (menu->isActive()) {
    menu->notifyRotaryChanged();
  }
  else {
    rotaryMoved = true;
  }
}

bool GuiControlMode::initializeNewDevice () {
  int deviceAliasLength = 0;
  memset(newDeviceAlias, 0, CHATTER_ALIAS_NAME_SIZE+1);

  // prompt for device name
  while (deviceAliasLength == 0) {
    deviceAliasLength = ((FullyInteractiveDisplay*)display)->getModalInput("Alias", 12, CharacterFilterAlphaNumeric, newDeviceAlias);
  }
  newDeviceAlias[deviceAliasLength] = 0;//term it, if the user backspaced some
  Serial.print("New Device name: "); Serial.println(newDeviceAlias);


  int clusterAliasLength = 0;
  memset(newClusterAlias, 0, CHATTER_ALIAS_NAME_SIZE+1);

  // prompt for cluster name
  while (clusterAliasLength == 0) {
    clusterAliasLength = ((FullyInteractiveDisplay*)display)->getModalInput("Cluster", 12, CharacterFilterAlphaNumeric, newClusterAlias);
  }

  int newFreqLength = 0;
  memset(newFreq, 0, 7);
  float newFrequency = LORA_DEFAULT_FREQUENCY;
  bool goodFreq = false;
  sprintf(newFreq, "%.1f", newFrequency);
  while (!goodFreq) {
    newFreqLength = ((FullyInteractiveDisplay*)display)->getModalInput("LoRa Freq", 5, CharacterFilterNumeric, newFreq, newFreq);

    if (newFreqLength > 0) {
      newFrequency = atof(newFreq);
      if (newFrequency >= LORA_MIN_FREQUENCY || newFrequency <= LORA_MAX_FREQUENCY) {
        goodFreq = true;
      }
    }
  }

  ClusterAdmin* admin = new ClusterAdmin(chatter);

  Serial.print("New Device name (later): "); Serial.println(newDeviceAlias);

  display->showAlert("Initializing", AlertWarning);
  return admin->genesis(newDeviceAlias, newClusterAlias, newFrequency);
}

bool GuiControlMode::onboardNewClient (unsigned long timeout) {
  int cycleLength = 5000;
  int bleLoopCount = 0;
  float pct = 0.0;

  display->clearAll();
  display->showTitle("Onboarding...");
  display->showMessage("Waiting for device to join...", Green, 0);

  unsigned long startTime = millis();
  while (millis() - startTime < timeout) {
    pct = (millis() % cycleLength) / (double)cycleLength;
    display->showProgress(pct);

    if(handleConnectedDevice ()) {
      showClusterOk();
      logConsole("Onboarded a device successfully");
      display->showAlert("Trusted", AlertSuccess);
      delay(2000);
      fullRepaint = true;
      return true;
    }
  }

  display->showAlert("No Devices", AlertWarning);
  delay(2000);
  fullRepaint = true;

  return false;
}

void GuiControlMode::sleepOrBackground(unsigned long sleepTime) {
  unsigned long startTime = millis();

  while (millis() - startTime < sleepTime) {
    if (menu->isActive() == false) {
      if (display->isTouchEnabled()) {
        ((TouchEnabledDisplay*)display)->handleIfTouched();
      }
    }
    delay(50);
  }
}


