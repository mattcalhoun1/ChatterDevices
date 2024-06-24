#include "GuiControlMode.h"

StartupState GuiControlMode::init() {
    logConsole("GuiControlMode Initializing");
    #ifdef ROTARY_ENABLED
      logConsole("Rotary control is enabled");
      rotary = new RotaryEncoder(PIN_ROTARY_IN1, PIN_ROTARY_IN2, RotaryEncoder::LatchMode::TWO03);
    #endif

    // make sure parent is initialized
    StartupState returnState = HeadsUpControlMode::init();

    // setup the menu
    menu = new Menu((MenuEnabledDisplay*)display, rotary, this, chatter->isRootDevice(chatter->getDeviceId()), chatter->isRootDevice(chatter->getDeviceId()), this);
    menu->init();

    // add self as a touch listener
    if (display->isTouchEnabled()) {
      ((TouchEnabledDisplay*)display)->addTouchListener(this);
      ((TouchEnabledDisplay*)display)->addTouchListener(menu);

      // set keyboard orientation
      ((TouchEnabledDisplay*)display)->setKeyboardOrientation(chatter->getDeviceStore()->getKeyboardOrientedLandscape() ? Landscape : Portrait);
      deviceMeshEnabled = chatter->getDeviceStore()->getMeshEnabled();
    }

    if (returnState == StartupComplete) {
      deviceIterator = new DeviceAliasIterator(chatter->getTrustStore());
      messageIterator = new MessageIterator(chatter->getMessageStore(), chatter->getTrustStore(), chatter);
      nearbyDeviceIterator = new NearbyDeviceIterator(chatter->getPingTable(), chatter->getTrustStore(), chatter);
      clusterIterator = new ClusterAliasIterator(chatter->getClusterStore());

      // show all messages for this cluster, unless user filters down
      setCurrentDeviceFilter(chatter->getDeviceId());

      memset(title, 0, 32);
      sprintf(title, "%s @ %s", chatter->getDeviceAlias(), chatter->getClusterAlias());
      //showTitle(title);
      //showStatus("Ready");

      //showMessageHistory(true);
      //showButtons();
      fullRepaint = true;
    }

    return returnState;
}


void GuiControlMode::handleUnlicensedDevice() {
    awaitingLicense = true;
    showQrCode();
    ((FullyInteractiveDisplay*)display)->setTouchListening(true);
    ((FullyInteractiveDisplay*)display)->setTouchSensitivity(TouchSensitivityHigh);

    while(true) {
      if (!((FullyInteractiveDisplay*)display)->isKeyboardShowing()) {
        ((FullyInteractiveDisplay*)display)->handleIfTouched();        
      }
    }
}

void GuiControlMode::showQrCode () {
  // Create the QR code
    QRCode qrcode;
    char fullLink[128];
    sprintf(fullLink, "%s%s", CHATTER_LICENSING_SITE_PREFIX, chatter->getUniqueHardwareId());

    const uint8_t ecc = 0;  //lowest level of error correction
     const uint8_t version = 4;

    uint8_t qrcodeData[qrcode_getBufferSize(version)];
     qrcode_initText(&qrcode, 
                    qrcodeData, 
                    version,   ecc, 
                    fullLink);
    const int xy_scale = 6;
    int xmax   = display->getScreenWidth()/2;
    int ymax = display->getScreenHeight()/2;
    int offset = (xy_scale*qrcode.size);
    int   x1 = xmax - (offset/2);
    int y1 = ymax - (offset/2);

    int px1 =   x1;
    int py1 = x1;
    int px2 = px1;
    int py2 = py1;

    display->fillRect(0, 0, 270, 320, Black);
    
//     // Top quiet zone
    for (uint8_t y = 0; y < qrcode.size; y++) {
        for   (uint8_t x = 0; x < qrcode.size; x++) {
            bool mod = qrcode_getModule(&qrcode,   x, y);
            px1 = x1 + x * xy_scale;
            py1 = x1 + y * xy_scale;
             px2 = px1 + xy_scale;
            py2 = py1 + xy_scale;
            if(mod){    
              display->fillRect(px1, py1, px2-px1, py2-py1, White);
            }
        }
    }

    display->showText("Please obtain a license at:", 10, 260, TextSmall, Green);
    display->showText("https://chatters.io", 55, 280, TextSmall, Green);

    char idMessage[32];

    char licPt1[5];
    char licPt2[5];
    char licPt3[5];
    char licPt4[5];

    memset(licPt1, 0, 5);
    memset(licPt2, 0, 5);
    memset(licPt3, 0, 5);
    memset(licPt4, 0, 5);

    const char* licBasePtr = chatter->getUniqueHardwareId();
    memcpy(licPt1, licBasePtr, 4);
    memcpy(licPt2, licBasePtr+4, 4);
    memcpy(licPt3, licBasePtr+8, 4);
    memcpy(licPt4, licBasePtr+12, 4);

    sprintf(idMessage, "%s %s %s %s", licPt1, licPt2, licPt3, licPt4);

    display->showText(idMessage, 25, 305, TextSmall, Beige);

}

void GuiControlMode::beginInteractiveIfPossible() {
    ((FullyInteractiveDisplay*)display)->showProgressBar(.75);
    showStatus("touch screen");
    ((FullyInteractiveDisplay*)display)->setTouchListening(true);
}

void GuiControlMode::handleStartupError() {
    // force landscape for easier user input
    if (fullyInteractive) {
      ((FullyInteractiveDisplay*)display)->setKeyboardOrientation(Landscape);
    }

    display->showAlert("Startup Error!", AlertError);
    delay(5000);

    // prompt factory reset
    handleEvent(UserRequestSecureFactoryReset);
}

void GuiControlMode::loop () {
  // check for input
  menu->menuUpdate();

  // if the user is interacting, skip the main loop
  if (!menu->isShowing()) {
    // scroll message
    updatePreviewsIfNecessary();

    //display->showProgress(((float)(millis() % 100)) / 100.0);
    if (fullRepaint == true && screenLocked == false) {
      fullRepaint = false;
      display->clearAll();
      showTitle(title);
      showTime(true);
      refreshDisplayContext(true);
      showButtons();
      lastStatus[0] = 0; // force status to repaint
      updateChatDashboard(true); // force repaint of chat dashboard
      showReady();
      display->showCacheUsed(chatter->getMeshPacketStore()->getCachePercentActive(), true);
      display->showBatteryLevel(getBatteryLevel(), true);
    }

    // execute main loop
    HeadsUpControlMode::loop();

    if (millis() - lastTick > tickFrequency) {
      if (unreadMessage) {
        // flash white breifly
        display->alertUnreadMessage(2);

        // if the screen isn't locked, clear the flag
        if (!screenLocked) {
          unreadMessage = false;
        }
      }

      uint8_t connectionQuality = chatter->getConnectionQuality();
      display->showTick(connectionQuality);
      lastTick = tickFrequency;

      // lock screen if no recent user activity
      if (!chatter->isOnboardMode() && !screenLocked && lastTouch + DISPLAY_TFT_SCREEN_LOCK_TIMEOUT < millis()) {
        logConsole("no user activity, lock screen");
        lockScreen();
      }

      // log battery level
      if (!screenLocked) {
        display->showBatteryLevel(getBatteryLevel(), false);
      }

      //Serial.print("Battery: ");Serial.print(getBatteryLevel());Serial.println("%");
    }

    // if learning is enabled, it may be time to trigger a learn message
    syncLearnActivity();
  }
  else {
    // still let touch events through
    if (fullyInteractive) {
      ((FullyInteractiveDisplay*)display)->handleIfTouched();
      //((FullyInteractiveDisplay*)display)->clearTouchInterrupts();
    }
  }
}

void GuiControlMode::notifyMessageReceived() {
  unreadMessage = true;

  // trigger vibrate/etc
}

void GuiControlMode::showButtons () {
  if (fullyInteractive) {
    ((FullyInteractiveDisplay*)display)->showButtons();
  }
}

void GuiControlMode::showMessageHistory(bool resetOffset) {
  if (resetOffset) {
    // reload from message store
    ((MessageIterator*)messageIterator)->init(chatter->getClusterId(), chatter->getDeviceId(), currentDeviceFilter, true);

    // set the offset (if necessary) so that the newest messages are shown by default
    // and the user would have to scroll up to see older ones
    previewSize = messageIterator->getNumItems();

    if (messageIterator->getNumItems() > display->getMaxDisplayableMessages()) {
      previewOffset = messageIterator->getNumItems() - display->getMaxDisplayableMessages();
    }
  }

  // clear the message area
  display->clearMessageArea();

  if (previewSize == 0) {
    if (memcmp(currentDeviceFilter, chatter->getDeviceId(), CHATTER_DEVICE_ID_SIZE) != 0) {
      memset(previewTitleBuffer, 0, MESSAGE_TITLE_BUFFER_SIZE + 1);
      chatter->getTrustStore()->loadAlias(currentDeviceFilter, previewTitleBuffer);
      sprintf(previewTextBuffer, "%s%s", "No messages from ", previewTitleBuffer);
    }
    else {
      sprintf(previewTextBuffer, "%s", "No messages from anyone");
    }

    display->showMainMessage ("No Messages", previewTextBuffer, AlertActivity);
    return;
  }

  for (int msg = previewOffset; msg < messageIterator->getNumItems() && (msg - previewOffset) < display->getMaxDisplayableMessages(); msg++) {
    memset(previewTitleBuffer, 0, MESSAGE_TITLE_BUFFER_SIZE + 1);
    memset(previewTextBuffer, 0, MESSAGE_PREVIEW_BUFFER_SIZE + 1);
    memset(previewTsBuffer, 0, 12);

    messageIterator->loadItemName(msg, previewTitleBuffer);

    // timestamp is in position5 and 11 chars long
    memcpy(previewTsBuffer, previewTitleBuffer + 5, 11);

    // shift the rest of the title left, so we wipe the timestamp
    for (uint8_t c = 5; c + 12 < MESSAGE_TITLE_BUFFER_SIZE + 1; c++) {
      previewTitleBuffer[c] = previewTitleBuffer[c+12];
    }
 
    // if it's a small message, go ahead and print. otherwise, user will have to look
    if (messageIterator->isPreviewable(msg)) {
      chatter->getMessageStore()->loadMessage (messageIterator->getItemVal(msg), (uint8_t*)previewTextBuffer, MESSAGE_PREVIEW_BUFFER_SIZE);
    }
    else {
      sprintf(previewTextBuffer, "%s", "[large message]");
    }
    //logConsole(previewTitleBuffer);
    display->showMessageAndTitle(
      previewTitleBuffer+5, 
      previewTextBuffer, 
      previewTsBuffer, 
      previewTitleBuffer[3] == '<', 
      previewTitleBuffer[1], 
      previewTitleBuffer[0], 
      previewTitleBuffer[0] == SentViaBroadcast ? DarkBlue : Yellow, 
      LightBlue, 
      msg - previewOffset);
  }

  // if there are more messages, indicate that
  display->showMainScrolls(previewOffset > 0, previewSize > previewOffset + display->getMaxDisplayableMessages());
}

void GuiControlMode::showMeshPath (const char* recipientId) {
  populateMeshPath(recipientId);

  display->clearAll();

  // show the message until the user closes
  display->showMessage((const char*)messageBuffer, Beige, 0);

  // pause for a short time
  delay(8000);
}

void GuiControlMode::showNearbyDevices(bool resetOffset) {
  if (resetOffset) {
    // reload from ping table
    //Serial.println("nearby dev iterator");
    nearbyDeviceIterator->init(chatter->getClusterId(), chatter->getDeviceId(), true);
    //Serial.println("nearby dev iterator initialized");
    previewOffset = 0; // 0 will have closest device
    previewSize = nearbyDeviceIterator->getNumItems();
  }

  // clear the message area
  display->clearMessageArea();

  if (previewSize == 0) {
    display->showMainMessage ("No Neighbors", "No devices are nearby", AlertActivity);
    return;
  }

  uint8_t nearAddr;
  int16_t nearRssi;
  uint8_t nearDirectRating;
  uint8_t nearIndirectRating;
  uint8_t nearPingQuality;
  bool nearTrusted;
  char nearChannel;
  char nearSecondaryChannel;

  for (int prevNum = previewOffset; prevNum < nearbyDeviceIterator->getNumItems() && (prevNum - previewOffset) < display->getMaxDisplayableMessages(); prevNum++) {
    memset(previewTitleBuffer, 0, MESSAGE_TITLE_BUFFER_SIZE + 1);
    memset(previewTextBuffer, 0, MESSAGE_PREVIEW_BUFFER_SIZE + 1);
    memset(previewTsBuffer, 0, 12);
    memset(previewDevIdBuffer, 0, CHATTER_DEVICE_ID_SIZE + 1);
    memset(previewAliasBuffer, 0, CHATTER_ALIAS_NAME_SIZE + 1);

    nearbyDeviceIterator->loadItemName(prevNum, previewTitleBuffer);

    nearTrusted = previewTitleBuffer[0];
    nearDirectRating = previewTitleBuffer[1];
    nearIndirectRating = previewTitleBuffer[2];
    nearPingQuality = previewTitleBuffer[3];

    memcpy(&nearRssi, previewTitleBuffer+4, 2);


    nearChannel = previewTitleBuffer[6];
    nearSecondaryChannel = previewTitleBuffer[7];

    memcpy(previewTsBuffer, previewTitleBuffer + 8, 8);
    memcpy(previewDevIdBuffer, previewTitleBuffer + 8 + 8, CHATTER_DEVICE_ID_SIZE);

    memcpy(previewAliasBuffer, previewTitleBuffer + 8 + 8 + CHATTER_DEVICE_ID_SIZE, CHATTER_ALIAS_NAME_SIZE);

    // change the title buffer to just include the 
    display->showNearbyDevice(
      previewAliasBuffer,
      previewDevIdBuffer,
      nearPingQuality,
      nearDirectRating,
      nearIndirectRating,
      previewTsBuffer,
      nearTrusted,
      nearRssi,
      nearChannel,
      nearSecondaryChannel,
      DarkBlue, 
      LightBlue, 
      prevNum - previewOffset      
    );
  }

  // if there are more messages, indicate that
  display->showMainScrolls(previewOffset > 0, previewSize > previewOffset + display->getMaxDisplayableMessages());
}

bool GuiControlMode::handleEvent (CommunicatorEventType eventType) {
  bool result = false;
  switch(eventType) {
    case ThermalReceived:
      // show thermal on screen
      camera->captureImage();
      display->showInterpolatedThermal(camera->getImageData(), false, "Live");
      delay(2000);
      return true;
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
      if (!promptSelectDevice()) {
        logConsole("Canceled");
        return false;
      }
      // fall through to the other send message logic

    case UserRequestSecureBroadcast:
    case UserRequestOpenBroadcast:
      // notice that all message sends fall through to this (no break statements above)

      if (isFullyInteractive()) {
        display->clearAll();

        // pop up the keyboard
        if (eventType == UserRequestDirectMessage) {
          messageBufferLength = ((FullyInteractiveDisplay*)display)->getModalInput(newDeviceAlias, "Send to the selected device", chatter->getMessageStore()->getMaxSmallMessageSize(), CharacterFilterNone, (char*)messageBuffer, "", 30000);
        }
        else {
          messageBufferLength = ((FullyInteractiveDisplay*)display)->getModalInput("Secure Broadcast", "Cast to all trusted devices", chatter->getMessageStore()->getMaxSmallMessageSize(), CharacterFilterNone, (char*)messageBuffer, "", 30000);
        }

        // send it
        if(messageBufferLength > 0) {
          if (eventType == UserRequestSecureBroadcast) {
            // change filter to show all messages
            setCurrentDeviceFilter(chatter->getDeviceId());

            display->showAlert("Broadcast", AlertActivity);
            display->resetProgress();
            result = chatter->broadcast(messageBuffer, messageBufferLength);
            if (!result) {
              result = chatter->broadcastSecondary(messageBuffer, messageBufferLength);
            }
          }
          else if (eventType == UserRequestOpenBroadcast) {
            // change filter to show all messages
            setCurrentDeviceFilter(chatter->getDeviceId());

            display->showAlert("Open Broadcast", AlertActivity);
            display->resetProgress();
            result = chatter->broadcastUnencrypted(messageBuffer, messageBufferLength, nullptr);
          }
          else {
            // change filter to show just messages to/from the selected user
            setCurrentDeviceFilter(otherDeviceId);

            MessageSendResult rs = attemptDirectSend();
            result = rs != MessageNotSent;
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
    case UserRequestJoinCluster:
      if (chatter->getClusterStore()->getNumClusters() >= 2) {
        display->showAlert("Delete a cluster first", AlertError);
        delay(5000);
        return true;
      }


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
    case UserRequestChangeCluster:
      
      // prompt for which cluster to switch to
      if (promptSelectCluster()) {
        // make sure the given cluster exists
        if (chatter->getClusterStore()->isActive(otherClusterId))
          // set that as the default cluster
          chatter->getDeviceStore()->setDefaultClusterId(otherClusterId);

          // restart
          restartDevice();
      }
      else {
        display->showAlert("Cluster not active", AlertError);
      }
      return true;


      return true;
    case UserRequestDeleteCluster:
      // there must be > 1 clusters
      if (chatter->getClusterStore()->getNumClusters() <= 1) {
        display->showAlert("Minimum 1 cluster", AlertError);
        delay(5000);
        return true;
      }

      if (promptSelectCluster()) {
        if (chatter->getClusterStore()->isActive(otherClusterId)) {
          if(chatter->getClusterStore()->deleteCluster(otherClusterId)) {
            display->showAlert("Cluster deleted", AlertSuccess);
          }
          else {
            display->showAlert("Delete failed", AlertError);
          }
        }
      }

      return true;
    case UserRequestNewCluster:
      return createNewCluster();
    case UserRequestFlipScreen:
      //logConsole("Flipping screen");
      if (display->getDisplayContext() == DisplayFullHistory) {
        display->setDisplayContext(DisplayNearbyDevices);
      }
      else {
        display->setDisplayContext(DisplayFullHistory);
      }
      fullRepaint = true;
      return true;
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
          return true;
        }
      }
      return true;
    case UserDeleteAllMessagesNoPrompt:
      chatter->getMessageStore()->clearAllMessages();
      fullRepaint = true;
      return true;
    case UserRequestClearMeshCache:
      chatter->getMeshPacketStore()->clearAllPackets();
      display->showAlert("Mesh Clear", AlertSuccess);
      delay(3000);
      return true;
    case UserRequestClearMeshGraph:
      if (isFullyInteractive()) {
        int newMessageLength = ((FullyInteractiveDisplay*)display)->getModalInput("Mesh Reset?", "Connections will be re-learned. Continue?", 1, CharacterFilterYesNo, (char*)messageBuffer, "", 0);
        if (newMessageLength > 0 && (messageBuffer[0] == 'y' || messageBuffer[0] == 'Y')) {
          chatter->resetMesh();
          display->showAlert("Reset Complete", AlertSuccess);
          delay(3000);
          fullRepaint = true;
        }
        else {
          logConsole("delete all canceled.");
          fullRepaint = true;
        }
      }
      return true;
    case UserRequestClearMeshGraphNoPrompt:
      chatter->resetMesh();
      fullRepaint = true;
      return true;
    case UserRequestFilterMessages:
      if (!promptSelectDevice()) {
        logConsole("Canceled");
        return false;
      }

      // change filter to this device and do full repaint
      setCurrentDeviceFilter(otherDeviceId);
      return true;

    case UserRequestMeshShowPath:
      // user must choose a recipient
      if (promptSelectDevice()) {
        showMeshPath(otherDeviceId);
      }
      return true;
    case UserRequestRemoteBattery:
      // send remote battery request
      if (promptSelectDevice()) {
        logConsole("Sending remote battery request to: ", otherDeviceId);
        sprintf((char*)messageBuffer, "%s:%c", "CFG", RemoteConfigBattery);
        messageBufferLength = 5; // CFG:B
        attemptDirectSend();

      }
      return true;
    case UserRequestClearPingTable:
      logConsole("Clearing ping table");
      chatter->getPingTable()->reset();
      return true;
    case UserRequestRemoteMessagesClear:
      // send remote battery request
      if (promptSelectDevice()) {
        logConsole("Sending messages clear req to: ", otherDeviceId);
        sprintf((char*)messageBuffer, "%s:%c", "CFG", RemoteConfigMessagesClear);
        messageBufferLength = 5; // CFG:B
        attemptDirectSend();
      }
      return true;
    case UserRequestRemoteClearMeshGraph:
      // send remote battery request
      if (promptSelectDevice()) {
        logConsole("Sending remote graph clear req to: ", otherDeviceId);
        sprintf((char*)messageBuffer, "%s:%c", "CFG", RemoteConfigMeshGraphClear);
        messageBufferLength = 5; // CFG:B
        attemptDirectSend();
      }
      return true;
    case UserRequestRemoteClearMeshCache:
      // send remote battery request
      if (promptSelectDevice()) {
        logConsole("Sending remote graph clear req to: ", otherDeviceId);
        sprintf((char*)messageBuffer, "%s:%c", "CFG", RemoteConfigMeshCacheClear);
        messageBufferLength = 5; // CFG:B
        attemptDirectSend();
      }
      return true;
    case UserRequestRemoteClearPingTable:
      // send remote battery request
      if (promptSelectDevice()) {
        logConsole("Sending remote graph clear req to: ", otherDeviceId);
        sprintf((char*)messageBuffer, "%s:%c", "CFG", RemoteConfigPingTableClear);
        messageBufferLength = 5; // CFG:B
        attemptDirectSend();
      }
      return true;
    case UserRequestRemoteEnableLearn:
      // send remote battery request
      if (promptSelectDevice()) {
        logConsole("Sending remote graph clear req to: ", otherDeviceId);
        sprintf((char*)messageBuffer, "%s:%c", "CFG", RemoteConfigEnableLearn);
        messageBufferLength = 5; // CFG:B
        attemptDirectSend();
      }
      return true;
    case UserRequestRemoteDisableLearn:
      // send remote battery request
      if (promptSelectDevice()) {
        logConsole("Sending remote graph clear req to: ", otherDeviceId);
        sprintf((char*)messageBuffer, "%s:%c", "CFG", RemoteConfigDisableLearn);
        messageBufferLength = 5; // CFG:B
        attemptDirectSend();
      }
      return true;
    case UserRequestRemotePath:
      char fromDevice[CHATTER_DEVICE_ID_SIZE+1];
      char toDevice[CHATTER_DEVICE_ID_SIZE+1];
      memset(fromDevice, 0, CHATTER_DEVICE_ID_SIZE+1);
      memset(toDevice, 0, CHATTER_DEVICE_ID_SIZE+1);
      if (promptSelectDevice()) {
        memcpy(fromDevice, otherDeviceId, CHATTER_DEVICE_ID_SIZE);
        if (promptSelectDevice()) {
          memcpy(toDevice, otherDeviceId, CHATTER_DEVICE_ID_SIZE);

          logConsole("Requesting path from: ", fromDevice);
          logConsole("to: ", toDevice);

          // put 'from' device back in other device id for the send
          memcpy(otherDeviceId, fromDevice, CHATTER_DEVICE_ID_SIZE);

          // send remote path request
          sprintf((char*)messageBuffer, "%s:%c", "CFG", RemoteConfigPath);
          memcpy(messageBuffer+5, toDevice, CHATTER_DEVICE_ID_SIZE);
          messageBufferLength = 5+CHATTER_DEVICE_ID_SIZE; // CFG:P[device id]
          messageBuffer[messageBufferLength]=0;
          attemptDirectSend();
        }
      }
      return true;
    case DeviceBackup:
      logConsole("backup");
      if (chatter->performLocalBackup()) {
        display->showAlert("Backup Complete", AlertSuccess);
      }
      else {
        display->showAlert("Backup Failed", AlertError);
      }
      delay(5000);
      return true;
    case DeviceRestore:
      logConsole("restore...");
      return true;

    case UserRequestChangeTime:
      promptUserNewTime ();
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
  bool parentResult = HeadsUpControlMode::handleEvent(eventType);

  // better repaint, we have no idea what happened
  fullRepaint = true;

  return parentResult;
}

void GuiControlMode::promptUserNewTime () {
  uint8_t timePositions[6] = {0, 2, 4, 6, 8, 10}; // buffer positions of 2 digit pieces
  char userEnteredPart[3];
  char fullDateTime[13];
  memset(fullDateTime, 0, 13);

  char partTitle[7];
  char partSubtitle[32];
  int userInputLength = 0;
  for (uint8_t pos = 0; pos < 6; pos++) {
    switch (pos) {
      case 0:
        sprintf(partTitle, "%s", "Year");
        sprintf(partSubtitle, "%s", "2 digit year (ex: 24)");
        break;
      case 1:
        sprintf(partTitle, "%s", "Month");
        sprintf(partSubtitle, "%s", "Current month (ex: 12)");
        break;
      case 2:
        sprintf(partTitle, "%s", "Day");
        sprintf(partSubtitle, "%s", "Current day (ex: 31)");
        break;
      case 3:
        sprintf(partTitle, "%s", "Hour");
        sprintf(partSubtitle, "%s", "Current hour, 24hr format (ex: 13)");
        break;
      case 4:
        sprintf(partTitle, "%s", "Minute");
        sprintf(partSubtitle, "%s", "Current minutes (ex: 59)");
        break;
      case 5:
        sprintf(partTitle, "%s", "Second");
        sprintf(partSubtitle, "%s", "Current seconds (ex: 59)");
        break;
    }

    bool partOk = false;
    while (!partOk) {
      memset(userEnteredPart, 0, 3);
      userInputLength = ((FullyInteractiveDisplay*)display)->getModalInput(partTitle, partSubtitle, 2, CharacterFilterNumeric, userEnteredPart, "", 0);
      if (userInputLength > 0) {
        // if the user only entered one digit, pad it to the right
        if (userInputLength == 1) {
          userEnteredPart[1] = userEnteredPart[0];
          userEnteredPart[0] = '0';
        }
        int userVal = atoi(userEnteredPart);
        // validate
        if (validateDatePart(userVal, pos)) {
          // if it IS ok, move on to next date part
          memcpy(fullDateTime + timePositions[pos], userEnteredPart, 2);
          partOk = true;
        }

        // if it's not ok, reprompt
      }
      else {
        // get out of loop and don't update
        return;
      }
    }
  }

  // if we get this far, we've got a valid date
  Serial.print("user wants new time of: ");Serial.println(fullDateTime);
  rtc->setNewDateTime(fullDateTime);
}

void GuiControlMode::setCurrentDeviceFilter(const char* deviceFilter) {
  if (memcmp(deviceFilter, currentDeviceFilter, CHATTER_DEVICE_ID_SIZE) != 0) {
    memcpy(currentDeviceFilter, deviceFilter, CHATTER_DEVICE_ID_SIZE);
    currentDeviceFilter[CHATTER_DEVICE_ID_SIZE] = 0;
    fullRepaint = true;
  }
}

bool GuiControlMode::validateDatePart(int partVal, uint8_t partNum) {
  switch (partNum)
  {
  case 0:
    return partVal > 23 && partVal < 100;
  case 1:
    return partVal > 0 && partVal < 13;
  case 2:
    return partVal > 0 && partVal < 32;
  case 3:
    return partVal >= 0 && partVal < 24;
  case 4:
  case 5:
    return partVal >= 0 && partVal < 60;  
  default:
    return false;
  }
}


bool GuiControlMode::promptSelectDevice() {
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

      // populate recipient name for modal display
      memset(newDeviceAlias, 0, CHATTER_ALIAS_NAME_SIZE+2);
      deviceIterator->loadItemName(menu->getIteratorSelection(), newDeviceAlias+1);
      newDeviceAlias[0] = '@';

      chatter->getTrustStore()->loadDeviceId(selectedSlot, otherDeviceId);
      logConsole("User chose device :");
      logConsole(otherDeviceId);

      return true;
    }
  }
  else {
    logConsole("not interactive");
  }

  return false;
}

bool GuiControlMode::promptSelectCluster() {
  memset(otherClusterId, 0, CHATTER_LOCAL_NET_ID_SIZE+CHATTER_GLOBAL_NET_ID_SIZE+1);
  if (isFullyInteractive()) {
    // select a recipient (or broadcast)
    clusterIterator->init(chatter->getClusterId(), chatter->getDeviceId(), true);
    menu->setItemIterator(clusterIterator);
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
      uint8_t selectedSlot = clusterIterator->getItemVal(menu->getIteratorSelection());

      chatter->getClusterStore()->loadClusterId(selectedSlot, otherClusterId);
      logConsole("User chose cluster :");
      logConsole(otherClusterId);
      return true;
    }
  }
  else {
    logConsole("not interactive");
  }

  return false;
}

void GuiControlMode::updateChatProgress(float progress) {
  display->showProgressBar(progress);
}

void GuiControlMode::resetChatProgress () {
  display->resetProgress();
  display->showProgressBar(0.0);
}

void GuiControlMode::hideChatProgress () {
  fullRepaint = true;
}

bool GuiControlMode::userInterrupted() {
  if(((FullyInteractiveDisplay*)display)->wasTouched()) {
    logConsole("user interrupted!");
    return true;
  }
  return false;

}

void GuiControlMode::updateMeshCacheUsed (float percent) {
  display->showCacheUsed(percent);
}

void GuiControlMode::pingReceived (uint8_t deviceAddress) {
  if (display->getDisplayContext() == DisplayNearbyDevices) {
    // refresh? that might be distracting?
    showNearbyDevices(true);
  }
}

void GuiControlMode::resetBackupProgress () {
  display->resetProgress();
}

void GuiControlMode::updateBackupProgress (float pct) {
  display->showProgressBar(pct);
}

uint8_t GuiControlMode::promptBackupPassword (char* buffer) {
  return promptForPassword (buffer, 16, false);
}


// Sends a direct message and executes any other logic
// as necessary to trigger routing. shows result to user
MessageSendResult GuiControlMode::attemptDirectSend () {
  display->resetProgress();

  MessageSendResult messageSendResult = MessageNotSent;

  bool sentViaBridge = false;
  bool result = false;
  bool sentViaMesh = false;

  display->showAlert("Send DM", AlertActivity);

  result = sendDirectMessage();

  if (!result && chatter->clusterHasDevice(ChatterDeviceBridgeLora)) {
    sentViaBridge = sendViaBridge();
  }
  
  if (!result && !sentViaBridge && deviceMeshEnabled &&  chatter->clusterSupportsMesh()) {
    sentViaMesh = sendViaMesh();
  }

  if (sentViaMesh) {
    display->showAlert("Q'd Mesh", AlertWarning);
    messageSendResult = MessageSentMesh;
  }
  else if (sentViaBridge) {
    display->showAlert("Sent (Bridge)", AlertWarning);
    messageSendResult = MessageSentBridge;
  }
  else if(result) {
    display->showAlert("Sent", AlertSuccess);
    messageSendResult = MessageSentDirect;
  } 
  else {
    display->showAlert("Not Sent!", AlertError);
  }

  // full repaint
  delay(1000);
  fullRepaint = true;
  return messageSendResult;
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

bool GuiControlMode::sendViaMesh() {
  display->resetProgress();

  logConsole("Sending via mesh..");
  ChatterMessageFlags flags;

  // drop message into mesh
  if (chatter->sendViaMesh(messageBuffer, messageBufferLength, otherDeviceId, &flags)) {
    logConsole("Message successfully sent via mesh");
    return true;
  }
  else {
    logConsole("Message not sent via mesh!");
    return false;
  }
}

bool GuiControlMode::handleEvent(CommunicatorEvent* event) {
  switch (event->EventType) {
    case UserRequestReply:
      if (isFullyInteractive()) {
        display->clearAll();

        // pop up the keyboard
        messageBufferLength = ((FullyInteractiveDisplay*)display)->getModalInput((const char*)eventBuffer.EventData, "Send to the selected device", chatter->getMessageStore()->getMaxSmallMessageSize(), CharacterFilterNone, (char*)messageBuffer, "", 30000);
        if (messageBufferLength > 0) {
          // send it
          return attemptDirectSend() != MessageNotSent;
        }
        else {
          fullRepaint = true;
        }
      }
      break;
    case BroadcastReceived:
      notifyMessageReceived();
      
      // if it was broadcast, change filter to everyone
      setCurrentDeviceFilter(chatter->getDeviceId());

      // if screen is locked, no repainting
      if (screenLocked) {
        return true;
      }

      showMessageHistory(true);
      return true;
    case MessageReceived:
      notifyMessageReceived();

      // change to the device in quesiton, if necessary
      setCurrentDeviceFilter(event->EventTarget);

      // if screen is locked, do nothing
      if (screenLocked) {
        return true;
      }
      showMessageHistory(true);
      return true;

    case AckReceived:
      // if screen is locked, do nothing
      if (screenLocked) {
        return true;
      }
      // change to the device in quesiton, if necessary
      setCurrentDeviceFilter(event->EventTarget);
      showMessageHistory(true);
      return true;

  }

  // let base handle
  bool parentResult = HeadsUpControlMode::handleEvent(event);

  // better repaint, we dont know what chagned
  fullRepaint = true;

  return parentResult;
}


void GuiControlMode::lockScreen () {
  logConsole("Locking screen");
  screenLocked = true;
  display->setBrightness(0);
}

void GuiControlMode::unlockScreen () {
  logConsole("Unlocking screen");

  // clear unread message flag
  unreadMessage = false;

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
  lastTouch = millis();

  if (awaitingLicense) {
    if (((FullyInteractiveDisplay*)display)->isKeyboardShowing()) {
      return true;
    }
    promptLicense();
    return true;
  }

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
          if (screenLocked) { // this looks like a redundant check
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
        case ButtonFlip:
          return handleEvent(UserRequestFlipScreen);
        case ButtonBroadcast:
          return handleEvent(UserRequestSecureBroadcast);
        case ButtonDM:
          return handleEvent(UserRequestDirectMessage);
        case ButtonFilter:
          return handleEvent(UserRequestFilterMessages);
        case ButtonMenu:
          ((FullyInteractiveDisplay*)display)->setTouchSensitivity(TouchSensitivityHigh);
          menu->show();
          return true;
      }
    }
    else if (previewSize > 0) {
      // if it's a scrollbar
      ScrollButton scroller = display->getScrollButtonAt(touchX, touchY);
      if (scroller != ScrollNone) {
        if (scroller == ScrollUp && display->isScrollUpEnabled() && previewOffset > 0) {
          previewOffset-= display->getMaxDisplayableMessages() > previewOffset ? previewOffset : display->getMaxDisplayableMessages();
          refreshDisplayContext(false);
          return true;
        }
        else if (scroller == ScrollDown && display->isScrollDownEnabled() && previewOffset + display->getMaxDisplayableMessages() < previewSize) {
          previewOffset+= previewOffset + (display->getMaxDisplayableMessages()*2) < previewSize ? display->getMaxDisplayableMessages() : (previewSize - previewOffset) - display->getMaxDisplayableMessages();
          refreshDisplayContext(false);
          return true;
        }
      }
      else {
        uint8_t selectedPreview = display->getMessagePosition(touchX, touchY);
        bool messageTargetFound = false;

        if (selectedPreview != DISPLAY_MESSAGE_POSITION_NULL && selectedPreview < previewSize) {
          if (display->getDisplayContext() == DisplayNearbyDevices) {
            // get the id of the target
            uint8_t selectedDevAddr = nearbyDeviceIterator->getItemVal(selectedPreview + previewOffset);
            chatter->loadDeviceId(selectedDevAddr, eventBuffer.EventTarget);
            messageTargetFound = true;
          }
          else { // showing messages, select the appropriate device
            uint8_t selectedMessageSlot = messageIterator->getItemVal(selectedPreview + previewOffset);

            // queue a reply event to that message slot
            if (chatter->getMessageStore()->loadDeviceIds (selectedMessageSlot, histSenderId, histRecipientId)) {
              // whichever is not this device becomes the target
              if (memcmp(chatter->getDeviceId(), histSenderId, CHATTER_DEVICE_ID_SIZE) != 0) {
                memcpy(eventBuffer.EventTarget, histSenderId, CHATTER_DEVICE_ID_SIZE);
              }
              else {
                memcpy(eventBuffer.EventTarget, histRecipientId, CHATTER_DEVICE_ID_SIZE);
              }
              messageTargetFound = true;
            }
          }

          // if a target was placed in the event
          if (messageTargetFound) {
            // if it's a broadcast, let the generic broadcast handle it
            if (memcmp(chatter->getClusterBroadcastId(), eventBuffer.EventTarget, CHATTER_DEVICE_ID_SIZE) == 0) {
              return handleEvent(UserRequestSecureBroadcast);
            }

            // put the alias into event data
            memset(eventBuffer.EventData, 0, EVENT_DATA_SIZE);
            eventBuffer.EventData[0] = '@';

            // if not able to lado alias, place the address instead
            if (!chatter->getTrustStore()->loadAlias(eventBuffer.EventTarget, (char*)eventBuffer.EventData + 1)) {
              memcpy(eventBuffer.EventData + 1, eventBuffer.EventTarget, CHATTER_DEVICE_ID_SIZE);
            }

            // set the other device id to the target
            memset(otherDeviceId, 0, CHATTER_DEVICE_ID_SIZE+1);
            memcpy(otherDeviceId, eventBuffer.EventTarget, CHATTER_DEVICE_ID_SIZE);

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
  display->setDisplayContext(DisplayFullHistory);
  refreshDisplayContext(true);
}


void GuiControlMode::refreshDisplayContext(bool fullRefresh) {
  switch (display->getDisplayContext()) {
    case DisplayFullHistory:
      //logConsole("refresh full message history");
      showMessageHistory(fullRefresh);
      break;
    case DisplayNearbyDevices:
      //logConsole("refresh display nearby devices");
      showNearbyDevices(fullRefresh);
      break;
  }
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

bool GuiControlMode::updatePreviewsIfNecessary () {
  // get current rotary position
  if (rotaryMoved) {
    rotaryMoved = false;
    int newRotaryPostion = rotary->getPosition();
    if (newRotaryPostion > selection) {
      if (previewOffset + display->getMaxDisplayableMessages() < previewSize) {
        previewOffset++;
        refreshDisplayContext(false);
      }
    }
    else if (newRotaryPostion < selection) {
      // if the offset is > 0, decrement
      if (previewOffset > 0) {
        previewOffset--;
        refreshDisplayContext(false);
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

bool GuiControlMode::promptClusterInfo (bool forceInput) {
  int clusterAliasLength = 0;
  memset(newClusterAlias, 0, CHATTER_ALIAS_NAME_SIZE+1);
  sprintf(newClusterAlias, "%s.%d", "Clst", random(2048));

  // prompt for cluster name
  while (clusterAliasLength == 0) {
    clusterAliasLength = ((FullyInteractiveDisplay*)display)->getModalInput("Cluster Name", "Unique(ish) name for your private cluster", 12, CharacterFilterAlphaNumeric, newClusterAlias, newClusterAlias, 0);

    if (clusterAliasLength == 0 && !forceInput) {
      return false;
    }
  }
  newClusterAlias[clusterAliasLength] = 0;

  int newFreqLength = 0;
  newFrequency = LORA_DEFAULT_FREQUENCY;

  memset(newFreq, 0, 7);
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
    else if (!forceInput) {
      return false;
    }
  }

  // wifi settings
  memset(newDeviceWifiSsid, 0, WIFI_SSID_MAX_LEN  + 1);
  memset(newDeviceWifiCred, 0, WIFI_CRED_MAX_LEN  + 1);
  int ssidLength = 0;
  newWifiEnabled = false;
  newWifiPreferred = false;

  // wifi not enabled for v1.0
  sprintf(newDeviceWifiSsid, "none", 4);
  sprintf(newDeviceWifiCred, "none", 4);

  /*
  while (ssidLength == 0) {
    ssidLength = ((FullyInteractiveDisplay*)display)->getModalInput("Enable WiFi?", "Common WiFi network (for UDP/etc)", 1, CharacterFilterYesNo, newDeviceWifiSsid, "", 0);

    // if no input, allow skipping
    if (ssidLength == 0 && !forceInput) {
      return false;
    }
  }
  if (newDeviceWifiSsid[0] == 'y') {
    newWifiEnabled = true;
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

    //Serial.print("Newly entered creds: ");Serial.println(newDeviceWifiCred);

    ssidLength = 0;
    while (ssidLength == 0) {
      ssidLength = ((FullyInteractiveDisplay*)display)->getModalInput("Prefer WiFi?", "Should WiFi be the preferred channel?", 1, CharacterFilterYesNo, newDeviceWifiPreferrred, "", 0);
    }
    newWifiPreferred = newDeviceWifiPreferrred[0] == 'y';
  }
  else {
    sprintf(newDeviceWifiSsid, "none", 4);
    sprintf(newDeviceWifiCred, "none", 4);
  }*/

  return true;
}

bool GuiControlMode::createNewCluster () {
  if (promptClusterInfo(false)) {
    ClusterAdmin* admin = new ClusterAdmin(chatter);
    display->showAlert("Initializing", AlertWarning);
    bool result = admin->generateCluster(newClusterAlias, newFrequency, newWifiEnabled, newDeviceWifiSsid, newDeviceWifiCred, newWifiPreferred);
    chatter->getDeviceStore()->setClearMeshOnStartup(true);
    display->showProgressBar(1.0);

    restartDevice();
  }
  return true;
}

bool GuiControlMode::promptLicense () {
  /*if (fullyInteractive) {
    ((FullyInteractiveDisplay*)display)->setTouchListening(true);
  }*/

  // does the user want to password protect
  int licenseLength = -1;
  char userLicenseKey[20];
  memset(userLicenseKey, 0, 20);
  while (licenseLength < 16) {
    licenseLength = ((FullyInteractiveDisplay*)display)->getModalInput("License Key", "Enter your license key", 19, CharacterFilterAlphaNumeric, userLicenseKey, "", 0);
  }

  Serial.print("done accepting license input, length: ");Serial.println(licenseLength);

  if (licenseLength >= 16) {
    // remove all spaces
    char cleanedLicenseKey[17];
    uint8_t cleanLength = 0;

    memset(cleanedLicenseKey, 0, 17);

    for (uint8_t i = 0; i < licenseLength; i++) {
      if (userLicenseKey[i] != ' ') {
        cleanedLicenseKey[cleanLength++] = userLicenseKey[i];
      }
    }

    if (cleanLength == 16) {
      // try this key
      if (chatter->getLicenseManager()->attemptUnlock (cleanedLicenseKey)) {
        display->showAlert("Unlocked!", AlertSuccess);
      }
      else {
        display->showAlert("Invalid Key", AlertError);
      }
      delay(5000);
    }

    restartDevice();
  }

  return false;
}

bool GuiControlMode::initializeNewDevice () {
  bool result = false;  
  
  // wipe all data
  chatter->factoryReset(true);

  if (fullyInteractive) {
    ((FullyInteractiveDisplay*)display)->setTouchListening(true);
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
  ClusterAdmin* admin = new ClusterAdmin(chatter);

  if (promptClusterInfo(true)) {
    display->showAlert("Initializing", AlertWarning);
    result = admin->genesis(newDeviceAlias, newDevicePassword, passwordLength, newClusterAlias, newFrequency, newWifiEnabled, newDeviceWifiSsid, newDeviceWifiCred, newWifiPreferred);
    display->showProgressBar(1.0);

    // queue the mesh data to get cleared on next startup
    chatter->getDeviceStore()->setClearMeshOnStartup(true);

    restartDevice();
  }

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
  memset(promptResp, 0, 2);
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

void GuiControlMode::showTime () {
    memcpy(lastTime, rtc->getViewableTime(), 16);
    HeadsUpControlMode::showTime();
}

void GuiControlMode::showTime (bool forceRepaint) {
  if (forceRepaint || memcmp(lastTime, rtc->getViewableTime(), 16) != 0) {
    showTime();
  }
}

bool GuiControlMode::syncLearnActivity () {
    // if we are learning and there is no scheduled learn message, schedule one now
    if (!chatter->isOnboardMode() && isPreferenceEnabled(PreferenceMeshLearningEnabled)) {
        showStatus("learn");
        if (nextScheduledLearn == 0) {
            // we are aiming to send messages at the given rate, across the entire cluster.
            // so if we are aiming for 1 message per minute, each device should send once per (minute * num devices)
            // to hit that average
            deviceIterator->init(chatter->getClusterId(), chatter->getDeviceId(), true);
            nextScheduledLearn = millis() + random(5000, 20000) + learnMessageFreq * deviceIterator->getNumItems();
        }
        else if (millis() >= nextScheduledLearn) {
            nextScheduledLearn = 0; // clear out so it will be re-scheduled

            // send a learning message. the content of the message
            // is just a timestamp so the recipient can easily see how long
            // delivery took
            memset(messageBuffer, 0, GUI_MESSAGE_BUFFER_SIZE);
            sprintf((char*)messageBuffer, "%s%d", "LRN:", rtc->getEpoch());
            messageBufferLength = strlen((const char*)messageBuffer);

            // choose a random recipient from our known recipients
            // note: until mesh key exchange works, this can only work
            // if device keys have been exchanged before hand
            deviceIterator->init(chatter->getClusterId(), chatter->getDeviceId(), true);
            uint8_t selectedDevice = random(0, deviceIterator->getNumItems());
            if (chatter->getTrustStore()->loadDeviceId(deviceIterator->getItemVal(selectedDevice), learningTargetDevice)) {
                learningTargetDevice[CHATTER_DEVICE_ID_SIZE] = 0;
                //Serial.print("want to send to: ");
                //Serial.print(learningTargetDevice);
                //Serial.print(" Msg: ");
                //Serial.println((const char*)messageBuffer);
                
                memcpy(otherDeviceId, learningTargetDevice, CHATTER_DEVICE_ID_SIZE+1);
                logConsole("learn target: ", (const char*)learningTargetDevice);
                if (attemptDirectSend() != MessageNotSent) {
                  logConsole("learn message sent");
                }
                else {
                  logConsole("learn message not sent!");
                }
            }
        }

        // log the latest message status
        // loop over each device, find all messages to/from it, calc status , print to serial
        // need filter in message store
        if (lastLearnLog + learnLogFreq < millis()) {
          lastLearnLog = millis();
          Serial.println("=== Learning Data ===");
          Serial.print("{");
          deviceIterator->init(chatter->getClusterId(), chatter->getDeviceId(), true);
          for (uint8_t devNum = 0; devNum < deviceIterator->getNumItems(); devNum++) {
            if(deviceIterator->loadItemName(devNum, learnAliasBuffer)) {
              chatter->getTrustStore()->loadDeviceId(deviceIterator->getItemVal(devNum), learningTargetDevice);
              learningTargetDevice[CHATTER_DEVICE_ID_SIZE] = 0;

              if (devNum > 0) {
                Serial.print(",");
              }
              Serial.print("\"");Serial.print(learnAliasBuffer);Serial.print("\":{");

              ((MessageIterator*)messageIterator)->init(chatter->getClusterId(), chatter->getDeviceId(), learningTargetDevice, true);

              uint8_t totalSends = 0;
              uint8_t totalAcks = 0;
              uint8_t totalSendsInProgress = 0;
              uint8_t totalReceives = 0;
              uint8_t totalReceivesWithSeconds = 0;
              unsigned long totalReceiveSeconds = 0;

              for (uint8_t messageNum = 0; messageNum < messageIterator->getNumItems(); messageNum++) {
                // if its from the other device and is a learning message, subtract the learning ts from the received ts to get delivery time
                bool isLarge = chatter->getMessageStore()->loadMessageDetails (messageIterator->getItemVal(messageNum), learnSenderIdBuffer, learnRecipientIdBuffer, learnMessageIdBuffer, learnTimestampBuffer, learnStatusBuffer, learnSendMethodBuffer);
                bool thisDeviceSent = memcmp(chatter->getDeviceId(), learnSenderIdBuffer, CHATTER_DEVICE_ID_SIZE) == 0;

                if (thisDeviceSent) {
                  // if the status is acknowledged, its success
                  if ((char)learnStatusBuffer == 'A') {
                    totalSends += 1;
                    totalAcks += 1;
                  }
                  else {
                    uint32_t timeSent = rtc->getEpoch (learnTimestampBuffer);
                    // if 15 minutes has passed since the send, its a fail
                    if (rtc->getEpoch() - timeSent > (60*15)) {
                      totalSends += 1;
                    }
                    else {
                      totalSendsInProgress += 1;
                    }
                  }

                }
                else {
                  // this was a receive. if it's a learning message, it should have an epoch
                  totalReceives += 1;
                  uint32_t timeSent = 0;

                  if (isLarge == false) {
                    int msgLength = chatter->getMessageStore()->loadMessage (messageIterator->getItemVal(messageNum), messageBuffer, GUI_MESSAGE_BUFFER_SIZE);
                    if (msgLength >= 10 && memcmp("LRN:", messageBuffer, 4) == 0) {
                      for (uint8_t i = 4; i < msgLength; i++) {
                        timeSent *= 10;
                        timeSent += messageBuffer[i]- '0';
                      }

                      uint32_t timeToDeliver = rtc->getEpoch(learnTimestampBuffer) - timeSent;
                      totalReceivesWithSeconds += 1;
                      totalReceiveSeconds += timeToDeliver;
                    }

                  }
                }
              }

              Serial.print("\"TotalSends\":");Serial.print(totalSends);
              Serial.print(", \"TotalAcks\":");Serial.print(totalAcks);
              Serial.print(", \"TotalReceives\":");Serial.print(totalReceives);
              Serial.print(", \"TotalReceivesWithSeconds\":");Serial.print(totalReceivesWithSeconds);
              Serial.print(", \"AverageTimeToReceive:\":");Serial.print(totalReceiveSeconds / totalReceivesWithSeconds);


              Serial.print("}");
            }
          }
          Serial.println("}");
          Serial.println("=== End Learning Data ===");
        }

        return true;
    }

    return false;
}
