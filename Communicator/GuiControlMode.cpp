#include "GuiControlMode.h"

bool GuiControlMode::init() {
    logConsole("GuiControlMode Initializing");

    //joystick = new Joystick(VRX_PIN, VRY_PIN);
    //joystick->refresh();
    //if (joystick->getDirection() == JoystickUp) {
    //  adminMode = true;
    //}
    //Serial.println("Starting rotary encoder...");

    rotary = new RotaryEncoder(PIN_ROTARY_IN1, PIN_ROTARY_IN2, RotaryEncoder::LatchMode::TWO03);
    //rotary = new RotaryEncoder(PIN_ROTARY_IN1, PIN_ROTARY_IN2, RotaryEncoder::LatchMode::FOUR3); // or FOUR0
    //rotary->tick();
    //selection = rotary->getPosition();

    //Serial.println("Rotary encoder started");

    // make sure parent is initialized
    if (HeadsUpControlMode::init()) {
      if (adminMode) {
        showTitle("Admin Mode");
        showStatus("Ready");
      }
      else {
        deviceIterator = new DeviceAliasIterator(chatter->getTrustStore());
        //deviceIterator = new TestIterator (90);

        memset(title, 0, 32);
        sprintf(title, "%s @ %s", chatter->getDeviceAlias(), chatter->getClusterAlias());
        showTitle(title);
        showStatus("Ready");
      }

      menu = new Menu((MenuEnabledDisplay*)display, rotary, this, chatter->isRootDevice(chatter->getDeviceId()));
      menu->init();

      return true;
    }

    menu = new Menu((MenuEnabledDisplay*)display, rotary, this, chatter->isRootDevice(chatter->getDeviceId()));
    menu->init();

    return false;
}

void GuiControlMode::loop () {
  // check for input
  /*if (updateSelection()) {
    logConsole("Selection changed to : " + String(selection));
  }*/
  menu->menuUpdate();

  // if the user is interacting, skip the main loop
  if (!menu->isActive()) {
    if (fullRepaint) {
      fullRepaint = false;
      display->clearAll();
      showTitle(title);
      showTime();
      showReady();
    }

    // execute main loop
    HeadsUpControlMode::loop();
  }
}

bool GuiControlMode::handleEvent (CommunicatorEventType eventType) {
  bool result = false;
  bool sentViaBridge = false;
  switch(eventType) {
    case UserRequestDirectMessage:
      memset(otherDeviceId, 0, CHATTER_DEVICE_ID_SIZE+1);
      if (isFullyInteractive()) {
        // select a recipient (or broadcast)
        deviceIterator->init(chatter->getClusterId());
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

void GuiControlMode::showLastMessage () {
  display->showMessage((const char*)messageBuffer, Green);
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

bool GuiControlMode::updateSelection () {
  // get current rotary position
  /*if (rotaryMoved) {
    rotaryMoved = false;
    //rotary->tick();
    int newRotaryPostion = rotary->getPosition();
    Serial.print("Rotary moved to: ");
    Serial.println(newRotaryPostion);
    selection = newRotaryPostion;
    return true;
  }*/

  return false;
}

void GuiControlMode::handleRotary () {
  rotary->tick(); // just call tick() to check the state.
    menu->notifyRotaryChanged();

  //rotaryMoved = true;

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
  display->showMessage("Waiting for device to join...", Green);

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

void GuiControlMode::adminLoop () {
  //if (admin == nullptr) {
  //  //admin = new ChatterAdmin(chatter);
  //  logConsole("New admin object initialized");
  //}
  
  display->showStatus("Admin Mode", Yellow);
  showAdminMenu();

  /*if(joystick->refresh()) {
    if (joystick->getDirection() == JoystickLeft) {
      admin->genesis();
    }
    else if (joystick->getDirection() == JoystickDown) {
      admin->syncDevice();
    }
  }*/

  // wait for input
  delay(100);
}

void GuiControlMode::showAdminMenu () {
  // up = onboard
  // right = trust
  // down = sync truststore
  // left = genesis (randomly generate ids)

  // after better ui / screen: allow alias, guest access, rename alias, choose network name
}



