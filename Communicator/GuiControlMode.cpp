#include "GuiControlMode.h"

bool GuiControlMode::init() {
    logConsole("GuiControlMode Initializing");
    #ifdef ROTARY_ENABLED
      logConsole("Rotary control is enabled");
      rotary = new RotaryEncoder(PIN_ROTARY_IN1, PIN_ROTARY_IN2, RotaryEncoder::LatchMode::TWO03);
    #endif

    // make sure parent is initialized
    bool parentInitialized = HeadsUpControlMode::init();

    // setup the menu
    menu = new Menu((MenuEnabledDisplay*)display, rotary, this, chatter->isRootDevice(chatter->getDeviceId()), this);
    menu->init();

    // add self as a touch listener
    if (display->isTouchEnabled()) {
      ((TouchEnabledDisplay*)display)->addTouchListener(this);
      ((TouchEnabledDisplay*)display)->addTouchListener(menu);

      // set keyboard orientation
      ((TouchEnabledDisplay*)display)->setKeyboardOrientation(chatter->getDeviceStore()->getKeyboardOrientedLandscape() ? Landscape : Portrait);
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
      showButtons();
    }
    else {
      // force landscape for easier user input
      if (fullyInteractive) {
        ((FullyInteractiveDisplay*)display)->setKeyboardOrientation(Landscape);
      }

      display->showAlert("Startup Error!", AlertError);
      delay(5000);

      // prompt factory reset
      handleEvent(UserRequestQuickFactoryReset);
    }

    return parentInitialized;
}

void GuiControlMode::loop () {
  // scroll message
  updateMessagePreviewsIfNecessary();

  // check for input
  menu->menuUpdate();

  // if the user is interacting, skip the main loop
  if (!menu->isShowing()) {
    //display->showProgress(((float)(millis() % 100)) / 100.0);
    if (fullRepaint) {
      fullRepaint = false;
      display->clearAll();
      showTitle(title);
      showTime();
      showMessageHistory(true);
      showButtons();
      showReady();
    }

    // execute main loop
    HeadsUpControlMode::loop();

    if (millis() - lastTick > tickFrequency) {
      display->showTick();
      lastTick = tickFrequency;
    }
  }
  else {
    // still let touch events through
    if (fullyInteractive) {
      ((FullyInteractiveDisplay*)display)->handleIfTouched();
      ((FullyInteractiveDisplay*)display)->clearTouchInterrupts();
    }
  }
}

void GuiControlMode::showButtons () {
  if (fullyInteractive) {
    ((FullyInteractiveDisplay*)display)->showButtons();
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

  // clear the message area
  display->clearMessageArea();

  for (int msg = messagePreviewOffset; msg < messageIterator->getNumItems() && (msg - messagePreviewOffset) < display->getMaxDisplayableMessages(); msg++) {
    memset(messageTitleBuffer, 0, MESSAGE_TITLE_BUFFER_SIZE + 1);
    memset(messagePreviewBuffer, 0, MESSAGE_PREVIEW_BUFFER_SIZE + 1);
    memset(messageTsBuffer, 0, 12);

    messageIterator->loadItemName(msg, messageTitleBuffer);

    // timestamp is in position5 and 11 chars long
    memcpy(messageTsBuffer, messageTitleBuffer + 5, 11);

    // shift the rest of the title left, so we wipe the timestamp
    for (uint8_t c = 5; c + 12 < MESSAGE_TITLE_BUFFER_SIZE + 1; c++) {
      messageTitleBuffer[c] = messageTitleBuffer[c+12];
    }
 
    //logConsole(messageTitleBuffer);

    // if it's a small message, go ahead and print. otherwise, user will have to look
    if (messageIterator->isPreviewable(msg)) {
      chatter->getMessageStore()->loadMessage (messageIterator->getItemVal(msg), (uint8_t*)messagePreviewBuffer, MESSAGE_PREVIEW_BUFFER_SIZE);
    }
    else {
      sprintf(messagePreviewBuffer, "%s", "[large message]");
    }
    //logConsole(messagePreviewBuffer);
    display->showMessageAndTitle(messageTitleBuffer+5, messagePreviewBuffer, messageTsBuffer, messageTitleBuffer[3] == '<', messageTitleBuffer[0] == SentViaBroadcast ? DarkBlue : Yellow, LightBlue, msg - messagePreviewOffset);
  }

  // if there are more messages, indicate that
  display->showMainScrolls(messagePreviewOffset > 0, messageHistorySize > messagePreviewOffset + display->getMaxDisplayableMessages());

  //logConsole("=== End Message History === ");
}

bool GuiControlMode::handleEvent (CommunicatorEventType eventType) {
  bool result = false;
  switch(eventType) {
    case UserRequestScreenLock:
      lockScreen();
      return true;

    case UserRequestPowerOff:
      // are we doing locked or powerdown? need to ask user
      logConsole("Going to sleep");
      display->clearAll();
      display->setBrightness(0);
      disableMessaging();
      deepSleep();

      return true;
    case MenuClosed:
      fullRepaint = true;
      if (fullyInteractive) {
        ((FullyInteractiveDisplay*)display)->resetToDefaultTouchSensitivity();
      }
      return true;
    case UserRequestDirectMessage:
      memset(otherDeviceId, 0, CHATTER_DEVICE_ID_SIZE+1);
      if (isFullyInteractive()) {
        // select a recipient (or broadcast)
        deviceIterator->init(chatter->getClusterId(), chatter->getDeviceId(), true);
        menu->setItemIterator(deviceIterator);
        menu->iteratorMenu(true); // modal

        // menu will be showing, turn up sensitivity
        ((FullyInteractiveDisplay*)display)->setTouchSensitivity(TouchSensitivityHigh);

        // wait for result of contact selections
        while (menu->isShowing()) {
          menu->menuUpdate();
          delay(10);
          ((FullyInteractiveDisplay*)display)->handleIfTouched();
        }
        ((FullyInteractiveDisplay*)display)->resetToDefaultTouchSensitivity();

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

      // fall through to next case

    case UserRequestSecureBroadcast:
    case UserRequestOpenBroadcast:
      // notice that all message sends fall through to this (no break statements above)

      if (isFullyInteractive()) {
        display->clearAll();

        // pop up the keyboard
        if (eventType == UserRequestDirectMessage) {
          messageBufferLength = ((FullyInteractiveDisplay*)display)->getModalInput("Direct Message", "Only the intended recipient may receive this", chatter->getMessageStore()->getMaxSmallMessageSize(), CharacterFilterNone, (char*)messageBuffer, "", 0);
        }
        else {
          messageBufferLength = ((FullyInteractiveDisplay*)display)->getModalInput("Secure Broadcast", "All cluster devices may receive this", chatter->getMessageStore()->getMaxSmallMessageSize(), CharacterFilterNone, (char*)messageBuffer, "", 20000);
        }

        // send it
        if(messageBufferLength > 0) {
          if (eventType == UserRequestSecureBroadcast) {
            display->showAlert("Broadcast", AlertActivity);
            display->resetProgress();
            result = chatter->broadcast(messageBuffer, messageBufferLength);
            if (!result) {
              result = chatter->broadcastSecondary(messageBuffer, messageBufferLength);
            }
          }
          else if (eventType == UserRequestOpenBroadcast) {
            display->showAlert("Open Broadcast", AlertActivity);
            display->resetProgress();
            result = chatter->broadcastUnencrypted(messageBuffer, messageBufferLength, nullptr);
          }
          else {
            result = attemptDirectSend();
            if (result) {
              fullRepaint = true;
            }
            return result;
          }

          if(result) {
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
        int newMessageLength = ((FullyInteractiveDisplay*)display)->getModalInput("Delete All?", "Permanently erase message history", 1, CharacterFilterYesNo, (char*)messageBuffer, "", 0);
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

    case UserRequestQuickFactoryReset:
    case UserRequestSecureFactoryReset:
      if (isFullyInteractive()) {
        int newMessageLength = ((FullyInteractiveDisplay*)display)->getModalInput(eventType == UserRequestQuickFactoryReset ? "Quick Reset?" : "Secure Reset?", "Permanently erase all device data", 1, CharacterFilterYesNo, (char*)messageBuffer, "", 0);
        //int newMessageLength = ((FullyInteractiveDisplay*)display)->getModalInput("Factory Reset?", 1, CharacterFilterYesNo, (char*)messageBuffer, "", 0);
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

void GuiControlMode::updateChatProgress(float progress) {
  display->showProgressBar(progress);
}

void GuiControlMode::resetChatProgress () {
  display->resetProgress();
  display->showProgressBar(0.0);
}

// Sends a direct message and executes any other logic
// as necessary to trigger routing. shows result to user
bool GuiControlMode::attemptDirectSend () {
  display->resetProgress();

  bool sentViaBridge = false;
  bool result = false;
  display->showAlert("Send DM", AlertActivity);

  result = sendDirectMessage();

  if (!result && chatter->clusterHasDevice(ChatterDeviceBridgeLora)) {
    sentViaBridge = sendViaBridge();
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

bool GuiControlMode::sendDirectMessage () {
  display->resetProgress();

  logConsole("Sending DM..");
  ChatterMessageFlags flags;
  flags.Flag0 = EncodingTypeText;
  flags.Flag2 = AckRequestTrue;

  if (chatter->send(messageBuffer, messageBufferLength, otherDeviceId, &flags)) {
    logConsole("DM sent");
    return true;
  } // try secondary channel
  else if (chatter->clusterHasSecondaryChannel() && chatter->sendSecondary(messageBuffer, messageBufferLength, otherDeviceId, &flags)) {
    logConsole("DM sent");
    return true;
  }

  logConsole("DM not sent");
  return false;
}

bool GuiControlMode::sendViaBridge() {
  display->resetProgress();

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

  // attempt to bridge
  if(chatter->sendViaIntermediary(messageBuffer, messageBufferLength, otherDeviceId, fullDeviceId, &flags, chatter->getChannel(lastChannel))) { // send a message to the bridge
    logConsole("Message successfully sent via bridge");
    return true;
  }
  else {
    logConsole("Message not sent!");
    return false;
  }
}


bool GuiControlMode::handleEvent(CommunicatorEvent* event) {
  switch (event->EventType) {
    case UserRequestReply:
      if (isFullyInteractive()) {
        display->clearAll();

        // pop up the keyboard
        messageBufferLength = ((FullyInteractiveDisplay*)display)->getModalInput((const char*)eventBuffer.EventData, chatter->getMessageStore()->getMaxSmallMessageSize(), CharacterFilterNone, (char*)messageBuffer);
        if (messageBufferLength > 0) {
          // send it
          return attemptDirectSend();
        }
        else {
          fullRepaint = true;
        }
      }
      break;
  }

  // let base handle
  return HeadsUpControlMode::handleEvent(event);
}


void GuiControlMode::lockScreen () {
  logConsole("Locking screen");
  screenLocked = true;
  display->setBrightness(0);
}

void GuiControlMode::unlockScreen () {
  logConsole("Unlocking screen");

  // clear screen so no message data is flashed
  display->clearAll();
  
  display->setBrightness(100);
  if (chatter->deviceHasPassword()) {

    // clear password buffer and prompt for a password (not updating hash that was stored during original login)
    memset(newDevicePassword, 0, CHATTER_PASSWORD_MAX_LENGTH + 1);
    int pwLength = promptForPassword(newDevicePassword, CHATTER_PASSWORD_MAX_LENGTH, false);
    hasher.clear();

    // get a hash of the user-provided unlock password
    hasher.update(newDevicePassword, pwLength);

    // get hash of the user-provided unlock password
    hasher.finalize(unlockPasswordHash, hasher.hashSize());
    hasher.clear();

    // clear password buffer
    memset(newDevicePassword, 0, CHATTER_PASSWORD_MAX_LENGTH + 1);

    // compare the hashes
    if (memcmp(unlockPasswordHash, passwordHash, 32) == 0) {
      logConsole("Password match, unlocking");
      fullRepaint = true;
      screenLocked = false;
    }
    else {
      logConsole("Password didn't match");

      // turn the screen back off
      display->setBrightness(0);
    }
  }
  else {
    fullRepaint = true;
    screenLocked = false;
  }
}

bool GuiControlMode::handleScreenTouched (int touchX, int touchY) {
  // if the screen is locked, intercept all touches
  DisplayedButton pressedButton;
  if (screenLocked) {
    pressedButton = ((FullyInteractiveDisplay*)display)->getButtonAt (touchX, touchY);
    if (pressedButton == ButtonLock) {
      unlockScreen();
    }
    return true;
  }
  else {
    // if the keyboard is showing, it gets the event
    if (menu->isShowing()){
      return true;
    }
    else if (fullyInteractive) {
      if (((FullyInteractiveDisplay*)display)->isKeyboardShowing()) {
        return true;
      }
    }

    // if it's a button
    pressedButton = ((FullyInteractiveDisplay*)display)->getButtonAt (touchX, touchY);
    if (pressedButton != ButtonNone) {
      switch (pressedButton) {
        case ButtonLock:
          if (screenLocked) {
            // prompt for password or unlock
            unlockScreen();
          }
          else {
            // show the powerdown menu
            ((FullyInteractiveDisplay*)display)->setTouchSensitivity(TouchSensitivityHigh);
            menu->powerMenu();

            // wait for result of contact selections
            while (menu->isShowing()) {
              menu->menuUpdate();
              delay(10);
              ((FullyInteractiveDisplay*)display)->handleIfTouched();
            }
            ((FullyInteractiveDisplay*)display)->resetToDefaultTouchSensitivity();
          }
          return true;

        case ButtonBroadcast:
          return handleEvent(UserRequestSecureBroadcast);
        case ButtonDM:
          return handleEvent(UserRequestDirectMessage);
        case ButtonMenu:
          ((FullyInteractiveDisplay*)display)->setTouchSensitivity(TouchSensitivityHigh);
          menu->show();
          return true;
      }
    }
    else if (messageHistorySize > 0) {
      // if it's a scrollbar
      ScrollButton scroller = display->getScrollButtonAt(touchX, touchY);
      if (scroller != ScrollNone) {
        if (scroller == ScrollUp && display->isScrollUpEnabled() && messagePreviewOffset > 0) {
          messagePreviewOffset--;
          showMessageHistory(false);
          return true;
        }
        else if (scroller == ScrollDown && display->isScrollDownEnabled() && messagePreviewOffset + display->getMaxDisplayableMessages() < messageHistorySize) {
          messagePreviewOffset++;
          showMessageHistory(false);
          return true;
        }
      }
      else {
        uint8_t selectedMessage = display->getMessagePosition(touchX, touchY);
        if (selectedMessage != DISPLAY_MESSAGE_POSITION_NULL && selectedMessage < messageHistorySize) {
          uint8_t selectedMessageSlot = messageIterator->getItemVal(selectedMessage + messagePreviewOffset);
        
          // queue a reply event to that message slot
          if (chatter->getMessageStore()->loadDeviceIds (selectedMessageSlot, histSenderId, histRecipientId)) {
            // whichever is not this device becomes the target
            if (memcmp(chatter->getDeviceId(), histSenderId, CHATTER_DEVICE_ID_SIZE) != 0) {
              memcpy(eventBuffer.EventTarget, histSenderId, CHATTER_DEVICE_ID_SIZE);
            }
            else {
              memcpy(eventBuffer.EventTarget, histRecipientId, CHATTER_DEVICE_ID_SIZE);
            }

            // if it's a broadcast, let the generic broadcast handle it
            if (memcmp(chatter->getClusterBroadcastId(), eventBuffer.EventTarget, CHATTER_DEVICE_ID_SIZE) == 0) {
              return handleEvent(UserRequestSecureBroadcast);
            }

            // put the alias into event data
            memset(eventBuffer.EventData, 0, EVENT_DATA_SIZE);
            eventBuffer.EventData[0] = '@';
            chatter->getTrustStore()->loadAlias(eventBuffer.EventTarget, (char*)eventBuffer.EventData + 1);
            eventBuffer.EventType = UserRequestReply;
            return handleEvent(&eventBuffer);
          }
        }
      }
    }
  }

  return true;
}

void GuiControlMode::showLastMessage () {
  //display->showMessage((const char*)messageBuffer, Green, 0);
  // refresh the message history if enabled
  showMessageHistory(true);
}


void GuiControlMode::buttonInterrupt () {
  menu->notifyButtonPressed();
  //sendText = true;
}

void GuiControlMode::touchInterrupt () {
  if (fullyInteractive) {
    ((FullyInteractiveDisplay*)display)->touchInterrupt();
  }
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

void GuiControlMode::handleRotaryInterrupt () {
  rotary->tick(); // just call tick() to check the state.
  if (menu->isShowing()) {
    menu->notifyRotaryChanged();
  }
  else {
    rotaryMoved = true;
  }
}

bool GuiControlMode::initializeNewDevice () {
  // default to landscape keyboard
  if (fullyInteractive) {
    ((FullyInteractiveDisplay*)display)->setKeyboardOrientation(Landscape);
  }

  // does the user want to password protect
  int passwordLength = 0;
  memset(newDevicePassword, 0, CHATTER_PASSWORD_MAX_LENGTH + 1);
  while (passwordLength == 0) {
    passwordLength = ((FullyInteractiveDisplay*)display)->getModalInput("Set Password?", "Permanently set device password", 1, CharacterFilterYesNo, newDevicePassword, "", 0);
  }
  if (newDevicePassword[0] == 'y') {
    passwordLength = 0;
    while (passwordLength == 0) {
      passwordLength = ((FullyInteractiveDisplay*)display)->getModalInput("Password", "Never forget this password!", CHATTER_PASSWORD_MAX_LENGTH, CharacterFilterNone, newDevicePassword, "", 0);
    }
  }
  else {
    passwordLength = 0;
  }

  int deviceAliasLength = 0;
  memset(newDeviceAlias, 0, CHATTER_ALIAS_NAME_SIZE+1);
  sprintf(newDeviceAlias, "%s.%d", "Me", random(2048));

  // prompt for device name
  while (deviceAliasLength == 0) {
    deviceAliasLength = ((FullyInteractiveDisplay*)display)->getModalInput("Device Name", "Unique(ish) name others will see", 12, CharacterFilterAlphaNumeric, newDeviceAlias, newDeviceAlias, 0);
  }
  newDeviceAlias[deviceAliasLength] = 0;//term it, if the user backspaced some

  int clusterAliasLength = 0;
  memset(newClusterAlias, 0, CHATTER_ALIAS_NAME_SIZE+1);
  sprintf(newClusterAlias, "%s.%d", "Clst", random(2048));

  // prompt for cluster name
  while (clusterAliasLength == 0) {
    clusterAliasLength = ((FullyInteractiveDisplay*)display)->getModalInput("Cluster Name", "Unique(ish) name for your private cluster", 12, CharacterFilterAlphaNumeric, newClusterAlias, newClusterAlias, 0);
  }
  newClusterAlias[clusterAliasLength] = 0;

  int newFreqLength = 0;
  memset(newFreq, 0, 7);
  float newFrequency = LORA_DEFAULT_FREQUENCY;
  bool goodFreq = false;
  sprintf(newFreq, "%.1f", newFrequency);
  while (!goodFreq) {
    char subtitle[32];
    sprintf(subtitle, "Between %.1f and %.1f", LORA_MIN_FREQUENCY, LORA_MAX_FREQUENCY);
    newFreqLength = ((FullyInteractiveDisplay*)display)->getModalInput("LoRa Frequency", subtitle, 5, CharacterFilterNumeric, newFreq, newFreq, 0);

    if (newFreqLength > 0) {
      newFrequency = atof(newFreq);
      if (newFrequency >= LORA_MIN_FREQUENCY || newFrequency <= LORA_MAX_FREQUENCY) {
        goodFreq = true;
      }
    }
  }

  // wifi settings
  memset(newDeviceWifiSsid, 0, WIFI_SSID_MAX_LEN  + 1);
  memset(newDeviceWifiCred, 0, WIFI_CRED_MAX_LEN  + 1);
  int ssidLength = 0;
  bool wifiEnabled = false;
  bool wifiPreferred = false;

  #if defined(CHATTER_WIFI_ENABLED)
  while (ssidLength == 0) {
    ssidLength = ((FullyInteractiveDisplay*)display)->getModalInput("Enable WiFi?", "Common WiFi network (for UDP/etc)", 1, CharacterFilterYesNo, newDeviceWifiSsid, "", 0);
  }
  if (newDeviceWifiSsid[0] == 'y') {
    wifiEnabled = true;
    ssidLength = 0;
    while (ssidLength == 0) {
      ssidLength = ((FullyInteractiveDisplay*)display)->getModalInput("WiFi SSID", "Enter the WiFi SSID (name)", WIFI_SSID_MAX_LEN, CharacterFilterNone, newDeviceWifiSsid, "", 0);
    }
    newDeviceWifiSsid[ssidLength] = 0;

    ssidLength = 0;
    while (ssidLength == 0) {
      ssidLength = ((FullyInteractiveDisplay*)display)->getModalInput("WiFi Password", "Enter the WiFi Password", WIFI_CRED_MAX_LEN, CharacterFilterNone, newDeviceWifiCred, "", 0);
    }
    newDeviceWifiCred[ssidLength] = 0;

    Serial.print("Newly entered creds: ");Serial.println(newDeviceWifiCred);

    ssidLength = 0;
    while (ssidLength == 0) {
      ssidLength = ((FullyInteractiveDisplay*)display)->getModalInput("Prefer WiFi?", "Should WiFi be the preferred channel?", 1, CharacterFilterYesNo, newDeviceWifiPreferrred, "", 0);
    }
    wifiPreferred = newDeviceWifiPreferrred[0] == 'y';
  }
  else {
    sprintf(newDeviceWifiSsid, "none", 4);
    sprintf(newDeviceWifiCred, "none", 4);
  }
  #else
    sprintf(newDeviceWifiSsid, "none", 4);
    sprintf(newDeviceWifiCred, "none", 4);
  #endif


  ClusterAdmin* admin = new ClusterAdmin(chatter);

  display->showAlert("Initializing", AlertWarning);
  bool result = admin->genesis(newDeviceAlias, newDevicePassword, passwordLength, newClusterAlias, newFrequency, wifiEnabled, newDeviceWifiSsid, newDeviceWifiCred, wifiPreferred);
  display->showProgressBar(1.0);
  return result;
}

bool GuiControlMode::onboardNewClient (unsigned long timeout) {
  int cycleLength = 5000;
  //float pct = 0.0;

  display->clearAll();
  display->showTitle("Onboard Device");
  char msg[24];
  sprintf(msg, "%s: %s", "Cluster ID", chatter->getClusterId());
  display->showMessage(msg, BrightGreen, 0);

  unsigned long startTime = millis();
  while (millis() - startTime < timeout) {
    //pct = (millis() % cycleLength) / (double)cycleLength;
    //display->showProgress(pct);

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

uint8_t GuiControlMode::promptForPassword (char* passwordBuffer, uint8_t maxPasswordLength, bool updateHash) {
  if (!fullyInteractive) {
    // non-interactive password entry not currently supported.
    // loop infinitely for now
    logConsole("Error: need password, but not interactive!");
    while(true) {
      delay(1000);
    }
  }

  uint8_t pwLength = 0;
  while (pwLength == 0) {
    pwLength = ((FullyInteractiveDisplay*)display)->getModalInput("Password", maxPasswordLength, CharacterFilterNone, passwordBuffer);
  }

  if (updateHash) {
    hasher.clear();
    hasher.update(passwordBuffer, pwLength);
    hasher.finalize(passwordHash, hasher.hashSize());
    hasher.clear();
  }
  return pwLength;
}


uint8_t GuiControlMode::promptForPassword (char* passwordBuffer, uint8_t maxPasswordLength) {
  // prompt for password, and hold onto the hash of it, so we can allow screen unlock to work later
  return promptForPassword(passwordBuffer, maxPasswordLength, true);
}

void GuiControlMode::promptFactoryReset () {
  handleEvent(UserRequestSecureFactoryReset);
}

bool GuiControlMode::promptYesNo (const char* message) {
  char promptResp[2];
  uint8_t respLen = ((FullyInteractiveDisplay*)display)->getModalInput(message, 1, CharacterFilterYesNo, promptResp);
  return respLen > 0 && promptResp[0] == 'y';
}

void GuiControlMode::sleepOrBackground(unsigned long sleepTime) {
  unsigned long startTime = millis();

  while (millis() - startTime < sleepTime) {
    if (menu->isShowing() == false) {
      if (display->isTouchEnabled()) {
        ((TouchEnabledDisplay*)display)->handleIfTouched();
        ((FullyInteractiveDisplay*)display)->clearTouchInterrupts();
      }
    }
    delay(50);
  }
}


