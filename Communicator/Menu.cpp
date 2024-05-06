#include "Menu.h"

bool Menu::init() {
    mainMenu(false);

  return true;
}

void Menu::show() {
  mainMenu(true);
  mode = MenuActive;
  needsRepainted = true;
}

bool Menu::handleScreenTouched(int touchX, int touchY) {
  if (mode == MenuActive && millis() - oledMenu.lastMenuActivity >= minTouchDelay) {
    // if user touched scroll button, handle that
    MenuScrollButton scrollBtn = display->getMenuScrollButtonAt(touchX, touchY);
    if (scrollBtn == MenuScrollUp) {
      scrollUpTouched = true;
      oledMenu.lastMenuActivity = millis();
    }
    else if (scrollBtn == MenuScrollDown) {
      scrollDownTouched = true;
      oledMenu.lastMenuActivity = millis();
    }
    else {
      // check if a menu item was touched
      uint8_t touchedItem = display->getMenuItemAt(touchX, touchY);

      //Serial.print("Touched item: ");Serial.print(touchedItem);Serial.print(" at (");Serial.print(touchX);Serial.print(",");Serial.print(touchY);Serial.println(")");

      if (touchedItem != MENU_ITEM_NONE) {
        oledMenu.lastMenuActivity = millis();

        // trigger the button press logic
        if (touchedItem >= 0 && touchedItem < oledMenu.noOfmenuItems) {
          // figure out the offset
          oledMenu.highlightedMenuItem = touchedItem + 1 + scrollOffset;
          buttonPressed = true;
          //Serial.print("User touched: "); Serial.println(touchedItem + 1 + scrollOffset);
        }
      }
      else {
        //Serial.println("User off menu");
        resetMenu();
      }
    }

    return true;
  }

  return false;
}

void Menu::iteratorMenu (bool fullRepaint) {
  resetMenu(fullRepaint);
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

  // highlight the center item to make he menu full
  oledMenu.highlightedMenuItem = max(0, subsetSize >= MENU_DEFAULT_HIGHLIGHTED_ITEM ? MENU_DEFAULT_HIGHLIGHTED_ITEM : subsetSize);
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

void Menu::mainMenu(bool fullRepaint) {
  resetMenu(fullRepaint); // clear any previous menu
  oledMenu.menuId = MENU_ID_MAIN;
  oledMenu.noOfmenuItems = 4;
  oledMenu.menuTitle = "Main Menu";

  oledMenu.menuItems[MENU_MAIN_CLUSTER] = "Cluster";
  oledMenu.menuItems[MENU_MAIN_DEVICE] = "Device";
  oledMenu.menuItems[MENU_MAIN_MESH] = "Mesh";
  oledMenu.menuItems[MENU_MAIN_POWER] = "Power";

  // highlight the center item to make he menu full
  oledMenu.highlightedMenuItem = MENU_DEFAULT_HIGHLIGHTED_ITEM;
  oledMenu.lastMenuActivity = millis();
}

void Menu::deviceMenu() {
  resetMenu(true); // clear any previous menu
  oledMenu.menuId = MENU_ID_DEVICE;
  oledMenu.noOfmenuItems = 6;
  oledMenu.menuTitle = "Device";

  oledMenu.menuItems[MENU_DEVICE_CLEAR_MESSAGES] = "Clear Messages";
  oledMenu.menuItems[MENU_DEVICE_MESSAGE_HISTORY] = prefHandler->isPreferenceEnabled(PreferenceMessageHistory) ? "Disable History" : "Enable History";
  oledMenu.menuItems[MENU_DEVICE_KEYBOARD_ORIENTATION] = prefHandler->isPreferenceEnabled(PreferenceKeyboardLandscape) ? "Keyboard Small" : "Keyboard Large";
  oledMenu.menuItems[MENU_DEVICE_WIFI_ENABLE] = prefHandler->isPreferenceEnabled(PreferenceWifiEnabled) ? "Disable Wifi" : "Enable Wifi";
  oledMenu.menuItems[MENU_DEVICE_SET_TIME] = "Set Time";
  oledMenu.menuItems[MENU_DEVICE_SECURE_FACTORY_RESET] = "Factory Reset";

  // highlight the center item to make he menu full
  oledMenu.highlightedMenuItem = MENU_DEFAULT_HIGHLIGHTED_ITEM;
  oledMenu.lastMenuActivity = millis();
}

void Menu::clusterMenu() {
  resetMenu(true); // clear any previous menu
  oledMenu.menuId = MENU_ID_CLUSTER;
  oledMenu.menuTitle = "Cluster";

    oledMenu.menuItems[MENU_CLUSTER_CHANGE_CLUSTER] = "Change Cluster";
    oledMenu.menuItems[MENU_CLUSTER_JOIN_CLUSTER]   = "Join Cluster";
    oledMenu.menuItems[MENU_CLUSTER_CREATE_CLUSTER] = "Create Cluster";
    oledMenu.menuItems[MENU_CLUSTER_DELETE_CLUSTER] = "Delete Cluster";
    oledMenu.menuItems[MENU_CLUSTER_ONBOARD_DEVICE] = "Onboard Device";

  if (onboardAllowed) {
    oledMenu.noOfmenuItems = 5;
  }
  else {
    oledMenu.noOfmenuItems = 4;
  }
  oledMenu.highlightedMenuItem = MENU_DEFAULT_HIGHLIGHTED_ITEM;
  oledMenu.lastMenuActivity = millis();

  mode = MenuActive;
  needsRepainted = true;
}

void Menu::meshMenu() {
  resetMenu(true); // clear any previous menu
  oledMenu.menuId = MENU_ID_MESH;
  oledMenu.noOfmenuItems = 4;
  oledMenu.menuTitle = "Mesh";

  oledMenu.menuItems[MENU_MESH_SHOW_PATH] = "Show Path";
  oledMenu.menuItems[MENU_MESH_ENABLE] = prefHandler->isPreferenceEnabled(PreferenceMeshEnabled) ? "Disable Mesh" : "Enable Mesh";
  oledMenu.menuItems[MENU_MESH_CLEAR_PACKETS] = "Clear Cache";
  oledMenu.menuItems[MENU_MESH_CLEAR_GRAPH] = "Mesh Reset";

  // highlight the center item to make he menu full
  oledMenu.highlightedMenuItem = MENU_DEFAULT_HIGHLIGHTED_ITEM;
  oledMenu.lastMenuActivity = millis();

  mode = MenuActive;
  needsRepainted = true;
}

void Menu::powerMenu() {
  resetMenu(true); // clear any previous menu
  oledMenu.menuId = MENU_ID_POWER;
  oledMenu.menuTitle = "Power";

  oledMenu.noOfmenuItems = 2;
  oledMenu.menuItems[MENU_POWER_LOCK_SCREEN] = "Lock Screen";
  oledMenu.menuItems[MENU_POWER_POWEROFF] = "Deep Sleep";

  oledMenu.highlightedMenuItem = MENU_DEFAULT_HIGHLIGHTED_ITEM;
  oledMenu.lastMenuActivity = millis();

  mode = MenuActive;
  needsRepainted = true;
}

void Menu::deviceActions() {
  if (oledMenu.menuId == MENU_ID_DEVICE) {  
    switch (oledMenu.selectedMenuItem) {
      // preference toggles
      case MENU_DEVICE_KEYBOARD_ORIENTATION:
        if (prefHandler->isPreferenceEnabled(PreferenceKeyboardLandscape)) {
          prefHandler->disablePreference(PreferenceKeyboardLandscape);
        }
        else {
          prefHandler->enablePreference(PreferenceKeyboardLandscape);
        }
        resetMenu();
        break;
      case MENU_DEVICE_CLEAR_MESSAGES:
        handler->handleEvent(UserDeleteAllMessages);
        resetMenu();
        break;
      case MENU_DEVICE_MESSAGE_HISTORY:
        // a change to message history will trigger a reboot
        if (prefHandler->isPreferenceEnabled(PreferenceMessageHistory)) {
          prefHandler->disablePreference(PreferenceMessageHistory);
        } 
        else {
          prefHandler->enablePreference(PreferenceMessageHistory);
        }
        resetMenu();
        break;
      case MENU_DEVICE_WIFI_ENABLE:
        // a change to message history will trigger a reboot
        if (prefHandler->isPreferenceEnabled(PreferenceWifiEnabled)) {
          prefHandler->disablePreference(PreferenceWifiEnabled);
        } 
        else {
          prefHandler->enablePreference(PreferenceWifiEnabled);
        }
        resetMenu();
        break;
      //case MENU_ADMIN_PING_LORA_BRIDGE:
      //  resetMenu();
      //  handler->queueEvent(PingLoraBridge);
      //  break;
      case MENU_DEVICE_SECURE_FACTORY_RESET:
        resetMenu();
        Serial.println("Secure Factory reset...");
        handler->handleEvent(UserRequestSecureFactoryReset);
        break;
      case MENU_DEVICE_SET_TIME:
        resetMenu();
        handler->handleEvent(UserRequestChangeTime);
        break;
    }
    oledMenu.selectedMenuItem = 0;                // clear menu item selected flag as it has been actioned
  }
}

void Menu::meshActions() {
  if (oledMenu.menuId == MENU_ID_MESH) {  
    switch (oledMenu.selectedMenuItem) {
      // preference toggles
      case MENU_MESH_SHOW_PATH:
        resetMenu();
        handler->handleEvent(UserRequestMeshShowPath);
        break;
      case MENU_MESH_ENABLE:
        // a change to message history will trigger a reboot
        if (prefHandler->isPreferenceEnabled(PreferenceMeshEnabled)) {
          prefHandler->disablePreference(PreferenceMeshEnabled);
        } 
        else {
          prefHandler->enablePreference(PreferenceMeshEnabled);
        }
        resetMenu();
        break;
      case MENU_MESH_CLEAR_PACKETS:
        resetMenu();
        handler->handleEvent(UserRequestClearMeshCache);
        break;
      case MENU_MESH_CLEAR_GRAPH:
        resetMenu();
        handler->handleEvent(UserRequestClearMeshGraph);
        break;
    }
    oledMenu.selectedMenuItem = 0;                // clear menu item selected flag as it has been actioned
  }
}

void Menu::clusterActions() {
  if (oledMenu.menuId == MENU_ID_CLUSTER) {  
    switch (oledMenu.selectedMenuItem) {
      case MENU_CLUSTER_DELETE_CLUSTER:
        resetMenu();
        Serial.println("Delete cluster...");
        handler->handleEvent(UserRequestDeletecluster);
        break;
      case MENU_CLUSTER_CREATE_CLUSTER:
        resetMenu();
        Serial.println("Create cluster...");
        handler->handleEvent(UserRequestNewCluster);
        break;
      case MENU_CLUSTER_JOIN_CLUSTER:
        resetMenu();
        handler->handleEvent(UserRequestJoinCluster);
        break;
      case MENU_CLUSTER_ONBOARD_DEVICE:
        resetMenu();
        handler->handleEvent(UserRequestOnboard);
        break;
    }
    oledMenu.selectedMenuItem = 0;                // clear menu item selected flag as it has been actioned
  }
}

void Menu::powerMenuActions() {
  if (oledMenu.menuId == MENU_ID_POWER) {  
    switch (oledMenu.selectedMenuItem) {
      case MENU_POWER_LOCK_SCREEN:
        resetMenu();
        handler->handleEvent(UserRequestScreenLock);
        break;
      case MENU_POWER_POWEROFF:
        resetMenu();
        handler->handleEvent(UserRequestPowerOff);
        break;
    }
    oledMenu.selectedMenuItem = 0;                // clear menu item selected flag as it has been actioned
  }
}

void Menu::setDatePartOptionsFor(uint8_t datePartId) {
  switch (datePartId) {
    case 0:
      oledMenu.menuTitle = "Month";
      oledMenu.mValueLow = 1;
      oledMenu.mValueHigh = 12;
      oledMenu.mValueStep = 1;
      oledMenu.mValueEntered = 6;
      break;
    case 1:
      oledMenu.menuTitle = "Day";
      oledMenu.mValueLow = 1;
      oledMenu.mValueHigh = 31;
      oledMenu.mValueStep = 1;
      oledMenu.mValueEntered = 15;
      break;
    case 2:
      oledMenu.menuTitle = "Year";
      oledMenu.mValueLow = 2024;
      oledMenu.mValueHigh = 2030;
      oledMenu.mValueStep = 1;
      oledMenu.mValueEntered = 2024;
      break;
    case 3:
      oledMenu.menuTitle = "Hour";
      oledMenu.mValueLow = 0;
      oledMenu.mValueHigh = 23;
      oledMenu.mValueStep = 1;
      oledMenu.mValueEntered = 12;
      break;
    case 4:
      oledMenu.menuTitle = "Minute";
      oledMenu.mValueLow = 0;
      oledMenu.mValueHigh = 59;
      oledMenu.mValueStep = 1;
      oledMenu.mValueEntered = 30;
      break;
    case 5:
      oledMenu.menuTitle = "Second";
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
  else if (oledMenu.menuId == MENU_ID_DEVICE) {
    deviceActions();
  }
  else if (oledMenu.menuId == MENU_ID_CLUSTER) {
    clusterActions();
  }
  else if (oledMenu.menuId == MENU_ID_ITERATOR) {
    iteratorActions();
  }
  else if (oledMenu.menuId == MENU_ID_POWER) {
    powerMenuActions();
  }
  else if (oledMenu.menuId == MENU_ID_MESH) {
    meshActions();
  }
}

void Menu::mainActions() {
  if (oledMenu.menuId == MENU_ID_MAIN) {  

    switch (oledMenu.selectedMenuItem) {
      case MENU_MAIN_CLUSTER:
        clusterMenu();
        mode = MenuActive;
        needsRepainted = true;
        break;        
      case MENU_MAIN_DEVICE:
        deviceMenu();
        mode = MenuActive;
        needsRepainted = true;
        break;        
      case MENU_MAIN_POWER:
        powerMenu();
        mode = MenuActive;
        needsRepainted = true;
        break;        
      case MENU_MAIN_MESH:
        meshMenu();
        mode = MenuActive;
        needsRepainted = true;
        break;        
    }
    oledMenu.selectedMenuItem = 0;                // clear menu item selected flag as it has been actioned
  }
}

void Menu::iteratorActions () {
  if (oledMenu.selectedMenuItem != ITERATOR_SELECTION_NONE) {
    //Serial.print("User selected: "); Serial.print(oledMenu.selectedMenuItem + iteratorOffset -1);
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
    mainMenu();
    mode = MenuActive;
  }

}

void Menu::notifyButtonPressed () {
    if (millis() - oledMenu.lastMenuActivity > minButtonDelay) {
      // if no menu is up, open default menu
      if (mode == MenuOff) {
        show();
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
      // ignore, main screen or other function may be using the rotary
      return;
      //  mainMenu(); // restart main menu
      //  mode = MenuActive;
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
            mainMenu();    // if button has been pressed return to default menu
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
    // touch scroll
    if (scrollUpTouched) {
      scrollUpTouched = false;
      if (display->isMenuScrollUpEnabled()) {
        movedBackward = true;
        scrollOffset--;
        oledMenu.highlightedMenuItem--;
        needsRepainted = true;
      }
    }
    else if (scrollDownTouched) {
      scrollDownTouched = false;
      if (display->isMenuScrollDownEnabled()) {
        movedForward = true;
        oledMenu.highlightedMenuItem++;
        scrollOffset++;
        needsRepainted = true;
      }
    }
    else if (rotaryChanged) {
        rotaryChanged = false;
        int newRotaryPostion = rotary->getPosition();
        if (newRotaryPostion != rotaryLastPosition) {
          int diff = newRotaryPostion - rotaryLastPosition;

          if (diff >= itemTrigger) {
              needsRepainted = true;
              rotaryLastPosition = newRotaryPostion;
              oledMenu.highlightedMenuItem++;
              scrollOffset++;
              movedForward = true;
              oledMenu.lastMenuActivity = millis();   // log time
          }
          if (diff <= -itemTrigger) {
              needsRepainted = true;
              rotaryLastPosition = newRotaryPostion;
              oledMenu.highlightedMenuItem--;
              scrollOffset--;
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

          // add logic here for scroll bars
          display->showMenuScrolls(iteratorOffset > 0, iteratorOffset + displayMaxLines < iterator->getNumItems());

          /*for (int i=1; i <= displayMaxLines; i++) {
              int item = oledMenu.highlightedMenuItem - _centreLine + i;

              if (item > 0 && item <= oledMenu.noOfmenuItems) {
                  display->showMenuItem(i, oledMenu.menuItems[item], item == oledMenu.highlightedMenuItem ? Black : White, item == oledMenu.highlightedMenuItem ? White : Black);
              }
          }*/
        }
        else {
            // add logic here for scroll bars
            display->showMenuScrolls(oledMenu.highlightedMenuItem > MENU_DEFAULT_HIGHLIGHTED_ITEM, oledMenu.highlightedMenuItem + _centreLine <= oledMenu.noOfmenuItems);
        }
        //else {
          // menu
          //for (int i=1; i <= displayMaxLines; i++) {
          for (int i=1; i <= displayMaxLines; i++) {
              int item = oledMenu.noOfmenuItems > displayMaxLines ? oledMenu.highlightedMenuItem - _centreLine + i : i;

              if (item > 0 && item <= oledMenu.noOfmenuItems) {
                if (MENU_HIGHLIGHT_CENTER) {
                  display->showMenuItem(i, oledMenu.menuItems[item], item == oledMenu.highlightedMenuItem ? Black : White, item == oledMenu.highlightedMenuItem ? White : Black);
                }
                else {
                  display->showMenuItem(i, oledMenu.menuItems[item], Beige, DarkGreen);
                }
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
        display->drawMenuBorder();
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

void Menu::resetMenu(bool repaint) {
  bool wasClosed = false;
  if (mode != MenuOff) {
    wasClosed = true;
  }

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
    scrollOffset = false;
    scrollDownTouched = false;
    scrollUpTouched = false;

  oledMenu.lastMenuActivity = millis();   // log time

  // clear oled display
  if (repaint) {
    display->blurMenuBackground();
    display->drawMenuBorder();

    display->clearMenuTitle();
    display->clearMenu();

  }
  
  if (wasClosed) {
    // trigger full repaint
    handler->handleEvent(MenuClosed);
  }
}
