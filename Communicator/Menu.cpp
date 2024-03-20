#include "Menu.h"

void Menu::defaultMenu () {
    mainMenu();
}

bool Menu::init() {
    defaultMenu();

  return true;
}

void Menu::iteratorMenu () {
  resetMenu();
  iteratorOffset = 0;

  oledMenu.menuId = MENU_ID_ITERATOR;
  uint8_t subsetSize = min (MENU_MAX_ITEMS, iterator->getNumItems());
  oledMenu.noOfmenuItems = subsetSize;
  oledMenu.menuTitle = "Select One";

  populateIteratorMenu();

  // pop up the menu
  //mode = MenuActive;
  oledMenu.selectedMenuItem = ITERATOR_SELECTION_NONE;

  mode = MenuActive;
  needsRepainted = true;
}

void Menu::populateIteratorMenu () {
  uint8_t subsetSize = min (MENU_MAX_ITEMS, iterator->getNumItems());
  oledMenu.noOfmenuItems = subsetSize;
  for (uint8_t i = 0; i < subsetSize; i++) {
    if (iterator->loadItemName(i + iteratorOffset, itemNameBuffer)) {
      oledMenu.menuItems[i+1] = String(itemNameBuffer);
    }
    else {
      Serial.println("unable to load item name");
    }
  }
}

void Menu::mainMenu() {
  resetMenu(); // clear any previous menu
  oledMenu.menuId = MENU_ID_MAIN;
  oledMenu.noOfmenuItems = 7;
  oledMenu.menuTitle = "Main Menu";
  oledMenu.menuItems[MENU_MAIN_DIRECT_MESSAGE] = "Direct Message";
  oledMenu.menuItems[MENU_MAIN_SECURE_BROADCAST] = "Secure Broadcast";
  //oledMenu.menuItems[MENU_MAIN_OPEN_BROADCAST] = "Open Broadcast";
  oledMenu.menuItems[MENU_MAIN_ANNOUNCE_PRESENCE] = "Announce Presence";
  oledMenu.menuItems[MENU_CHOOSE_CLUSTER] = "Choose Cluster";
  oledMenu.menuItems[MENU_MAIN_ONBOARDING] = "Onboarding";
  oledMenu.menuItems[MENU_MAIN_ADMIN] = "Admin";
  oledMenu.menuItems[MENU_MAIN_CANCEL] = "Cancel";
}

void Menu::adminMenu() {
  resetMenu(); // clear any previous menu
  oledMenu.menuId = MENU_ID_ADMIN;
  oledMenu.noOfmenuItems = 6;
  oledMenu.menuTitle = "Admin";

  oledMenu.menuItems[MENU_ADMIN_PING_LORA_BRIDGE] = "Ping Lora Bridge";
  oledMenu.menuItems[MENU_ADMIN_CREATE_CLUSTER] = "Create Cluster";
  oledMenu.menuItems[MENU_ADMIN_DELETE_CLUSTER] = "Delete Cluster";
  oledMenu.menuItems[MENU_ADMIN_SET_TIME] = "Set Time";
  oledMenu.menuItems[MENU_ADMIN_FACTORY_RESET] = "Factory Reset";
  oledMenu.menuItems[MENU_ADMIN_CANCEL] = "Cancel";
}

void Menu::onboardingMenu() {
  resetMenu(); // clear any previous menu
  oledMenu.menuId = MENU_ID_ONBOARDING;
  oledMenu.menuTitle = "Onboard";

  if (onboardAllowed) {
    oledMenu.noOfmenuItems = 2;
    oledMenu.menuItems[MENU_ONBOARDING_JOIN_CLUSTER] = "Join Cluster";
    oledMenu.menuItems[MENU_ONBOARDING_ONBOARD_DEVICE] = "Onboard Device";
  }
  else {
    oledMenu.noOfmenuItems = 1;
    oledMenu.menuItems[MENU_ONBOARDING_JOIN_CLUSTER] = "Join Cluster";
  }
}

void Menu::adminActions() {
  if (oledMenu.menuId == MENU_ID_ADMIN) {  
    switch (oledMenu.selectedMenuItem) {
      case MENU_ADMIN_PING_LORA_BRIDGE:
        resetMenu();
        handler->queueEvent(PingLoraBridge);
        break;
      case MENU_ADMIN_DELETE_CLUSTER:
        resetMenu();
        Serial.println("Delete cluster...");
        handler->handleEvent(UserRequestDeletecluster);
        break;
      case MENU_ADMIN_FACTORY_RESET:
        resetMenu();
        Serial.println("Factory reset...");
        handler->handleEvent(UserRequestFactoryReset);
        break;
      case MENU_ADMIN_CREATE_CLUSTER:
        resetMenu();
        Serial.println("Create cluster...");
        handler->handleEvent(UserRequestNewCluster);
        break;
      case MENU_ADMIN_CANCEL:
        mode = MenuOff;
        resetMenu();
        break;
      case MENU_ADMIN_SET_TIME:
        resetMenu();
        mode = MenuValueEntry;
        for (uint8_t datePart = 0; datePart < 6; datePart++) {
          buttonPressed = false; // clear the button press

          setDatePartOptionsFor(datePart);
          needsRepainted = true;

          selectedDateParts[datePart] = serviceValue(1);
        }

        // apply the user selected date
        eventBuffer.EventType = UserChangedTime;
        eventBuffer.EventDataLength = 12;
        sprintf(
          (char*)eventBuffer.EventData, 
          "%02d%02d%02d%02d%02d%02d", 
          selectedDateParts[2]-2000, selectedDateParts[0], selectedDateParts[1], selectedDateParts[3], selectedDateParts[4], selectedDateParts[5]);
        
        handler->handleEvent(&eventBuffer);

        // set default menu back to main, and clear
        resetMenu();
        mainMenu();
        mode = MenuOff;
        break;

    }
    oledMenu.selectedMenuItem = 0;                // clear menu item selected flag as it has been actioned
  }
}

void Menu::onboardingActions() {
  if (oledMenu.menuId == MENU_ID_ONBOARDING) {  
    switch (oledMenu.selectedMenuItem) {
      case MENU_ONBOARDING_JOIN_CLUSTER:
        resetMenu();
        handler->handleEvent(UserRequestBleJoinCluster);
        break;
      case MENU_ONBOARDING_ONBOARD_DEVICE:
        resetMenu();
        handler->handleEvent(UserRequestBleOnboard);
        break;
    }
    oledMenu.selectedMenuItem = 0;                // clear menu item selected flag as it has been actioned
  }
}

void Menu::setDatePartOptionsFor(uint8_t datePartId) {
  switch (datePartId) {
    case 0:
      oledMenu.menuTitle = "Select Month";
      oledMenu.mValueLow = 1;
      oledMenu.mValueHigh = 12;
      oledMenu.mValueStep = 1;
      oledMenu.mValueEntered = 6;
      break;
    case 1:
      oledMenu.menuTitle = "Select Date";
      oledMenu.mValueLow = 1;
      oledMenu.mValueHigh = 31;
      oledMenu.mValueStep = 1;
      oledMenu.mValueEntered = 15;
      break;
    case 2:
      oledMenu.menuTitle = "Select Year";
      oledMenu.mValueLow = 2024;
      oledMenu.mValueHigh = 2030;
      oledMenu.mValueStep = 1;
      oledMenu.mValueEntered = 2024;
      break;
    case 3:
      oledMenu.menuTitle = "Select Hour";
      oledMenu.mValueLow = 0;
      oledMenu.mValueHigh = 23;
      oledMenu.mValueStep = 1;
      oledMenu.mValueEntered = 12;
      break;
    case 4:
      oledMenu.menuTitle = "Select Minute";
      oledMenu.mValueLow = 0;
      oledMenu.mValueHigh = 59;
      oledMenu.mValueStep = 1;
      oledMenu.mValueEntered = 30;
      break;
    case 5:
      oledMenu.menuTitle = "Select Second";
      oledMenu.mValueLow = 0;
      oledMenu.mValueHigh = 59;
      oledMenu.mValueStep = 1;
      oledMenu.mValueEntered = 30;
      break;
  }
}

void Menu::menuActions () {
  if (oledMenu.menuId == MENU_ID_MAIN) {
    mainActions();
  }
  else if (oledMenu.menuId == MENU_ID_ADMIN) {
    adminActions();
  }
  else if (oledMenu.menuId == MENU_ID_ONBOARDING) {
    onboardingActions();
  }
  else if (oledMenu.menuId == MENU_ID_ITERATOR) {
    iteratorActions();
  }
}

void Menu::mainActions() {
  if (oledMenu.menuId == MENU_ID_MAIN) {  

    switch (oledMenu.selectedMenuItem) {
      case MENU_MAIN_DIRECT_MESSAGE:
        resetMenu();
        handler->handleEvent(UserRequestDirectMessage);
        break;
      case MENU_MAIN_SECURE_BROADCAST:
        resetMenu();
        handler->handleEvent(UserRequestSecureBroadcast);
        break;
     // case MENU_MAIN_OPEN_BROADCAST:
     //   resetMenu();
     //   handler->handleEvent(UserRequestOpenBroadcast);
     //   break;
      case MENU_MAIN_ANNOUNCE_PRESENCE:
        resetMenu();
        handler->queueEvent(UserRequestSelfAnnounce);
        break;
      case MENU_MAIN_ONBOARDING:
        onboardingMenu();
        mode = MenuActive;
        needsRepainted = true;
        break;        
      case MENU_MAIN_ADMIN:
        adminMenu();
        mode = MenuActive;
        needsRepainted = true;
        break;        
      case MENU_MAIN_CANCEL:
        mode = MenuOff;
        resetMenu();
    }
    oledMenu.selectedMenuItem = 0;                // clear menu item selected flag as it has been actioned
  }
  else if (oledMenu.menuId == MENU_ID_MESSAGES) {

    // back to main menu
    if (oledMenu.selectedMenuItem == 1) {
      if (serialDebug) Serial.println("View messages: back to main menu");
      defaultMenu();
      mode = MenuActive;
    }

    oledMenu.selectedMenuItem = 0;                // clear menu item selected flag as it has been actioned
  }
}

void Menu::iteratorActions () {
  if (oledMenu.selectedMenuItem != ITERATOR_SELECTION_NONE) {
    Serial.print("User selected: "); Serial.print(oledMenu.selectedMenuItem + iteratorOffset -1);
    mode = MenuOff;
    iteratorSelection = oledMenu.selectedMenuItem + iteratorOffset -1; // this menu system is indexed to 1 instead of 0
    resetMenu();
  }
}

// actions for value entered put in here
void Menu::menuValues() {

  // action for "demo_value"
  if (oledMenu.menuTitle == "demo_value") {
    String tString = String(oledMenu.mValueEntered);
    if (serialDebug) Serial.println("demo_value: The value entered was " + tString);
    displayMessage("ENTERED", "\nYou entered\nthe value\n    " + tString);
    // alternatively use 'resetMenu()' here to turn menus off after value entered - or use 'defaultMenu()' to re-start the default menu
  }

  // action for "on or off"
  if (oledMenu.menuTitle == "on or off") {
    if (serialDebug) Serial.println("demo_menu: on off selection was " + String(oledMenu.mValueEntered));
    defaultMenu();
    mode = MenuActive;
  }

}

void Menu::notifyButtonPressed () {
    if (millis() - oledMenu.lastMenuActivity > minButtonDelay) {
      // if no menu is up, perform default action, rather than showing menu
      if (mode == MenuOff) {
          //mode = MenuActive;
          //resetMenu();
          handler->queueEvent(UserRequestSelfAnnounce);
      }
      else {
        //needsRepainted = true;
        buttonPressed = true;
        oledMenu.lastMenuActivity = millis();
      }
    }
}

void Menu::notifyRotaryChanged () {
    if (mode == MenuOff) {
        mainMenu(); // restart main menu
        mode = MenuActive;
    }
    //needsRepainted = true;
    if (millis() - oledMenu.lastMenuActivity > minRotaryDelay) {
      rotaryChanged = true;
      oledMenu.lastMenuActivity = millis();
    }
}

void Menu::menuUpdate() {
  if (mode == MenuOff) return;    // if menu system is turned off do nothing more

  // if no recent activity then turn oled off
    if ( (unsigned long)(millis() - oledMenu.lastMenuActivity) > (menuTimeout * 1000) ) {
      resetMenu();
      return;
    }

    switch (mode) {
        //Serial.println(mode);

      // if there is an active menu
      case MenuActive:
        serviceMenu();
        menuActions();
        //menuActions();
        break;

      // if there is an active none blocking 'enter value'
      case MenuValueEntry:
        serviceValue(0);
        if (buttonPressed) {                        // if the button has been pressed
            buttonPressed = false; // reset the button press flag
          menuValues();                                             // a value has been entered so action it
          break;
        }

      // if a message is being displayed
      case MenuMessage:
        if (buttonPressed) {
            buttonPressed = false; // reset button flag
            defaultMenu();    // if button has been pressed return to default menu
            mode = MenuActive;
        } 
        break;
    }
}


// ----------------------------------------------------------------
//                       -service active menu
// ----------------------------------------------------------------

void Menu::serviceMenu() {
    bool movedForward = false;
    bool movedBackward = false;

    // rotary encoder
    if (rotaryChanged) {
        rotaryChanged = false;
        int newRotaryPostion = rotary->getPosition();
        if (newRotaryPostion != rotaryLastPosition) {
          int diff = newRotaryPostion - rotaryLastPosition;

          if (diff >= itemTrigger) {
              needsRepainted = true;
              rotaryLastPosition = newRotaryPostion;
              oledMenu.highlightedMenuItem++;
              movedForward = true;
              oledMenu.lastMenuActivity = millis();   // log time
          }
          if (diff <= -itemTrigger) {
              needsRepainted = true;
              rotaryLastPosition = newRotaryPostion;
              oledMenu.highlightedMenuItem--;
              movedBackward = true;
              oledMenu.lastMenuActivity = millis();   // log time
          }
        }
    }
    else if (buttonPressed) {
        buttonPressed = false; // reset button press
        oledMenu.selectedMenuItem = oledMenu.highlightedMenuItem;     // flag that the item has been selected
        oledMenu.lastMenuActivity = millis();   // log time
        //if (serialDebug) Serial.println("menu '" + oledMenu.menuTitle + "' item '" + oledMenu.menuItems[oledMenu.highlightedMenuItem] + "' selected");
    }


    if (needsRepainted) {
        needsRepainted = false;
        const int _centreLine = displayMaxLines / 2 + 1;    // mid list point
        display->clearMenu();

        // verify valid highlighted item
        if (oledMenu.highlightedMenuItem > oledMenu.noOfmenuItems) oledMenu.highlightedMenuItem = oledMenu.noOfmenuItems;
        if (oledMenu.highlightedMenuItem < 1) oledMenu.highlightedMenuItem = 1;

        // title
        display->showMenuTitle(oledMenu.menuTitle);

        // for iterator, we might need to slide window around using offset
        // if the user has gone forward and the window is not at the end
        if (oledMenu.menuId == MENU_ID_ITERATOR) {
          if (movedForward && oledMenu.highlightedMenuItem == _centreLine && (oledMenu.highlightedMenuItem + iteratorOffset + _centreLine <= iterator->getNumItems())) {
            iteratorOffset++;
            oledMenu.highlightedMenuItem--;
            populateIteratorMenu();
          }
          else if (movedBackward && oledMenu.highlightedMenuItem <= _centreLine && iteratorOffset > 0) {
            iteratorOffset--;
            oledMenu.highlightedMenuItem++;
            populateIteratorMenu();
          }

          /*for (int i=1; i <= displayMaxLines; i++) {
              int item = oledMenu.highlightedMenuItem - _centreLine + i;

              if (item > 0 && item <= oledMenu.noOfmenuItems) {
                  display->showMenuItem(i, oledMenu.menuItems[item], item == oledMenu.highlightedMenuItem ? Black : White, item == oledMenu.highlightedMenuItem ? White : Black);
              }
          }*/
        }
        //else {
          // menu
          //for (int i=1; i <= displayMaxLines; i++) {
          for (int i=1; i <= displayMaxLines; i++) {
              int item = oledMenu.highlightedMenuItem - _centreLine + i;

              if (item > 0 && item <= oledMenu.noOfmenuItems) {
                  display->showMenuItem(i, oledMenu.menuItems[item], item == oledMenu.highlightedMenuItem ? Black : White, item == oledMenu.highlightedMenuItem ? White : Black);
              }
          }
        //}
    }
}


// ----------------------------------------------------------------
//                        -service value entry
// ----------------------------------------------------------------
// if _blocking set to 1 then all other tasks are stopped until a value is entered

int Menu::serviceValue(bool _blocking) {

  const int _valueSpacingX = 30;      // spacing for the displayed value y position
  const int _valueSpacingY = 5;       // spacing for the displayed value y position

  if (_blocking) {
    mode = MenuBlocking;
    oledMenu.lastMenuActivity = millis();   // log time of last activity (for timeout)
  }
  uint32_t tTime;

  do {
    if (rotaryChanged) {
        rotaryChanged = false;
        int newRotaryPostion = rotary->getPosition();
        int diff = newRotaryPostion - rotaryLastPosition;

        if (diff >= itemTrigger) {
            needsRepainted = true;
            rotaryLastPosition = newRotaryPostion;
            oledMenu.mValueEntered+= oledMenu.mValueStep;
            oledMenu.lastMenuActivity = millis();   // log time
        }
        else if (diff <= -itemTrigger) {
            needsRepainted = true;
            rotaryLastPosition = newRotaryPostion;
            oledMenu.mValueEntered-= oledMenu.mValueStep;
            oledMenu.lastMenuActivity = millis();   // log time
        }

        if (oledMenu.mValueEntered < oledMenu.mValueLow) {
            oledMenu.mValueEntered = oledMenu.mValueLow;
        }
        else if (oledMenu.mValueEntered > oledMenu.mValueHigh) {
            oledMenu.mValueEntered = oledMenu.mValueHigh;
        }
    }

    if (needsRepainted) {
      needsRepainted = false;
        display->clearMenu();
        display->clearMenuTitle();
        display->showMenuTitle(oledMenu.menuTitle);

      // value selected
      String entered = String(oledMenu.mValueEntered);
      display->showMenuItem(1, entered, White, Black);

      // range
      display->showMenuItem(3, String(oledMenu.mValueLow) + " to " + String(oledMenu.mValueHigh), White, Black);

      // bar
        int Tlinelength = map(oledMenu.mValueEntered, oledMenu.mValueLow, oledMenu.mValueHigh, display->getMenuAreaX() + 5 , display->getMenuAreaWidth() - 5);
        display->fillRect(display->getMenuAreaX() + 5, display->getMenuAreaY() + 40, display->getMenuAreaWidth() - 5, 3, Black);
        display->fillRect(display->getMenuAreaX() + 5, display->getMenuAreaY() + 40, Tlinelength, 3, Blue);
    }

      tTime = (unsigned long)(millis() - oledMenu.lastMenuActivity);      // time since last activity

  } while (_blocking && buttonPressed == false && tTime < (menuTimeout * 1000));        // if in blocking mode repeat until button is pressed or timeout

  if (_blocking) mode = MenuOff;

  return oledMenu.mValueEntered;        // used when in blocking mode

}


// ----------------------------------------------------------------
//                           -list create
// ----------------------------------------------------------------
// create a menu from a list
// e.g.       String tList[]={"main menu", "2", "3", "4", "5", "6"};
//            createList("demo_list", 6, &tList[0]);

void Menu::createList(String _title, int _noOfElements, String *_list) {
  resetMenu();                      // clear any previous menu
  mode = MenuActive;                  // enable menu mode
  oledMenu.noOfmenuItems = _noOfElements;    // set the number of items in this menu
  oledMenu.menuTitle = _title;               // menus title (used to identify it)

  for (int i=1; i <= _noOfElements; i++) {
    oledMenu.menuItems[i] = _list[i-1];        // set the menu items
  }
}


// ----------------------------------------------------------------
//                         -message display
// ----------------------------------------------------------------
// 21 characters per line, use "\n" for next line
// assistant:  <     line 1        ><     line 2        ><     line 3        ><     line 4         >

 void Menu::displayMessage(String _title, String _message) {
  resetMenu();
  mode = MenuMessage;

    display->clearMenu();
    display->showMenuTitle(_title);
    display->showMenuItem(1, _message, White, Black);
 }


// ----------------------------------------------------------------
//                        -reset menu system
// ----------------------------------------------------------------

void Menu::resetMenu() {
  // reset all menu variables / flags
    mode = MenuOff;
    oledMenu.selectedMenuItem = 0;
    //rotaryEncoder.encoder0Pos = 0;
    oledMenu.noOfmenuItems = 0;
    oledMenu.menuTitle = "";
    oledMenu.highlightedMenuItem = 0;
    oledMenu.mValueEntered = 0;
    //rotaryEncoder.reButtonPressed = 0;
    buttonPressed = false;
    rotaryChanged = false;

  oledMenu.lastMenuActivity = millis();   // log time

  // clear oled display
  display->clearMenuTitle();
    display->clearMenu();
}


    /*

    // demonstrate usage of 'enter a value' (blocking) which is quick and easy but stops all other tasks until the value is entered
    if (oledMenu.selectedMenuItem == 6) {
      if (serialDebug) Serial.println("demo_menu: blocking enter a value");
      // set perameters
        resetMenu();
        mode = MenuValueEntry;
        oledMenu.menuTitle = "Select Message";
        oledMenu.mValueLow = 0;
        oledMenu.mValueHigh = 50;
        oledMenu.mValueStep = 1;
        oledMenu.mValueEntered = 5;
      int tEntered = serviceValue(1);      // request value
      Serial.println("The value entered was " + String(tEntered));
      defaultMenu();
      mode = MenuActive;
    }

    // demonstrate usage of message
    if (oledMenu.selectedMenuItem == 7) {
      if (serialDebug) Serial.println("demo_menu: message");
      displayMessage("Message", "Hello\nThis is a demo\nmessage.");    // 21 chars per line, "\n" = next line
    }

    // turn menu/oLED off
    else if (oledMenu.selectedMenuItem == 8) {
      if (serialDebug) Serial.println("demo_menu: menu off");
      resetMenu();    // turn menus off
    }
    */
