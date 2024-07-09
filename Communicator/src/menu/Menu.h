#include "../display/MenuEnabledDisplay.h"
#include "../display/TouchEnabledDisplay.h"
#include <RotaryEncoder.h>
#include "../events/CommunicatorEventHandler.h"
#include "../events/CommunicatorEvent.h"
#include "../prefs/CommunicatorPreferenceHandler.h"
#include "Arduino.h"
#include "../iterator/ItemIterator.h"

#ifndef MENU_H
#define MENU_H

#define MENU_MAX_ITEMS 9

#define MENU_MAIN_CLUSTER 1
#define MENU_MAIN_DEVICE 2
#define MENU_MAIN_MESH 3
#define MENU_MAIN_CONNECTIONS 4
#define MENU_MAIN_BACKPACKS 5
#define MENU_MAIN_BACKPACK_TRIGGER 6
#define MENU_MAIN_REMOTE 7
//#define MENU_MAIN_POWER 6
#define MENU_MAIN_BACKUPS 99

#define MENU_CLUSTER_SECURE_CAST 1
#define MENU_CLUSTER_CHANGE_CLUSTER 2
#define MENU_CLUSTER_JOIN_CLUSTER 3
#define MENU_CLUSTER_CREATE_CLUSTER 4
#define MENU_CLUSTER_DELETE_CLUSTER 5
#define MENU_CLUSTER_ONBOARD_DEVICE 6

#define MENU_MESH_SHOW_PATH 1
#define MENU_MESH_ENABLE 2
#define MENU_MESH_CLEAR_PACKETS 3
#define MENU_MESH_CLEAR_GRAPH 4
#define MENU_MESH_ENABLE_LEARNING 5
#define MENU_MESH_ENABLE_REMOTE_CONFIG 6

#define MENU_BACKPACK_ENABLE 1
#define MENU_BACKPACK_THERMAL 2
#define MENU_BACKPACK_ULTRASONIC 3
#define MENU_BACKPACK_MOTION 4
#define MENU_BACKPACK_RELAY 5

#define MENU_BACKPACK_TRIGGER_THERMAL 1
#define MENU_BACKPACK_TRIGGER_RELAY 2

#define MENU_BACKUPS_LOCAL_BACKUP 1
#define MENU_BACKUPS_LOCAL_RESTORE 2
//#define MENU_CLUSTER_JOIN_CLUSTER 3

#define MENU_DEVICE_CLEAR_MESSAGES 1
#define MENU_DEVICE_MESSAGE_HISTORY 2
#define MENU_DEVICE_SCREEN_TIMEOUT 3
#define MENU_DEVICE_CHANGE_PASSWORD 4
#define MENU_DEVICE_ENABLE_DST 5
#define MENU_DEVICE_ALLOW_EXPIRED_MESSAGES 6
#define MENU_DEVICE_SHOW_ID 7
#define MENU_DEVICE_SET_TIME 8
#define MENU_DEVICE_SECURE_FACTORY_RESET 9

#define MENU_SCREEN_TIMEOUT_1 1
#define MENU_SCREEN_TIMEOUT_2 2
#define MENU_SCREEN_TIMEOUT_5 3
#define MENU_SCREEN_TIMEOUT_NEVER 4

#define MENU_CONNECTIONS_LORA_ENABLE 1
#define MENU_CONNECTIONS_WIFI_ENABLE 2
#define MENU_CONNECTIONS_WIRED_ENABLE 3

#define MENU_REMOTE_BATTERY 1
#define MENU_REMOTE_PATH 2
#define MENU_REMOTE_CLEAR_MESH_CACHE 3
#define MENU_REMOTE_CLEAR_PING_TABLE 4
#define MENU_REMOTE_ENABLE_LEARN 5
#define MENU_REMOTE_DISABLE_LEARN 6
#define MENU_REMOTE_CLEAR_MESH_GRAPH 7
#define MENU_REMOTE_CLEAR_MESSAGES 8

#define MENU_POWER_LOCK_SCREEN 1
#define MENU_POWER_POWEROFF 2

#define MENU_THERMAL_ENABLE 1
#define MENU_THERMAL_REMOTE_ENABLE 2
#define MENU_THERMAL_AUTO_ENABLE 3

#define MENU_RELAY_ENABLE 1
#define MENU_RELAY_REMOTE_ENABLE 2

#define MENU_ID_MAIN 0
#define MENU_ID_CLUSTER 1
#define MENU_ID_DEVICE 2
#define MENU_ID_MESH 3
#define MENU_ID_CONNECTIONS 4
#define MENU_ID_POWER 5
#define MENU_ID_REMOTE 6
#define MENU_ID_SCREEN_TIMEOUT 7
#define MENU_ID_ITERATOR 8
#define MENU_ID_BACKUPS 9
#define MENU_ID_BACKPACKS 10
#define MENU_ID_THERMAL 11
#define MENU_ID_BACKPACK_TRIGGER 12
#define MENU_ID_RELAY 13

#define MENU_HIGHLIGHT_CENTER false
#define MENU_DEFAULT_HIGHLIGHTED_ITEM 3

#define ITERATOR_MAX_NAME_SIZE 16
#define ITERATOR_SELECTION_NONE 255

enum MenuMode {
    MenuOff,                                  // display is off
    MenuActive,                                 // a menu is active
    MenuValueEntry,                                // 'enter a value' none blocking is active
    MenuMessage,                              // displaying a message
    MenuBlocking                              // a blocking procedure is in progress (see enter value)
};

struct OledMenu {
    // menu
    int menuId = 0;
    String menuTitle = "";                    // the title of active mode
    int noOfmenuItems = 0;                    // number if menu items in the active menu
    int selectedMenuItem = 0;                 // when a menu item is selected it is flagged here until actioned and cleared
    int highlightedMenuItem = 0;              // which item is curently highlighted in the menu
    String menuItems[MENU_MAX_ITEMS+1];         // store for the menu item titles
    uint32_t lastMenuActivity = 0;            // time the menu last saw any activity (used for timeout)
    // 'enter a value'
    int mValueEntered = 0;                    // store for number entered by value entry menu
    int mValueLow = 0;                        // lowest allowed value
    int mValueHigh = 0;                       // highest allowed value
    int mValueStep = 0;                       // step size when encoder is turned
};

class Menu : public TouchListener {
    public:
        Menu(MenuEnabledDisplay* _display, RotaryEncoder* _rotary, CommunicatorEventHandler* _handler, bool _onboardAllowed, bool _remoteAllowed, CommunicatorPreferenceHandler* _prefHandler) { display = _display; rotary = _rotary; handler = _handler; onboardAllowed = _onboardAllowed; remoteAllowed = _remoteAllowed; prefHandler = _prefHandler;}

        bool init ();

        void mainMenu (bool fullRepaint = false);
        void deviceMenu();
        void screenTimeoutMenu();
        void connectionsMenu();
        void clusterMenu ();
        void backpackTriggerMenu ();
        void meshMenu ();
        void backupsMenu ();
        void powerMenu();
        void remoteMenu();
        void backpacksMenu();
        void thermalMenu();
        void relayMenu();
        void iteratorMenu (bool fullRepaint = false);
        void iteratorActions ();        
        void backpackTriggerActions ();

        void menuActions ();

        // leftover from demo
        //void demoMenu ();
        //void menuActions ();


        //void value1 ();
        void menuValues ();
        void resetMenu (bool repaint = false);
        void displayMessage(String _title, String _message);
        void createList(String _title, int _noOfElements, String *_list);
        int serviceValue(bool _blocking);
        void serviceMenu ();
        void menuUpdate ();
        bool isShowing () {return mode != MenuOff; }
        void show ();
        void notifyButtonPressed ();
        void notifyRotaryChanged ();

        void setItemIterator (ItemIterator* _iterator) { iterator = _iterator; }
        uint8_t getIteratorSelection () {return iteratorSelection;}

        bool handleScreenTouched (int touchX, int touchY);

    protected:
        void deviceActions ();
        void screenTimeoutActions();
        void connectionsActions ();
        void mainActions ();
        void backupsActions ();
        void clusterActions ();
        void meshActions ();
        void powerMenuActions ();
        void remoteActions();
        void backpacksActions();
        void thermalActions();
        void relayActions();

        void populateIteratorMenu ();

        MenuEnabledDisplay* display;
        MenuMode mode = MenuOff;                 // default mode at startup is off
        OledMenu oledMenu;

        bool buttonPressed = false;
        bool rotaryChanged = false;
        bool scrollUpTouched = false;
        bool scrollDownTouched = false;

        //unsigned long lastRotaryChange = 0;
        int minRotaryDelay = 100; // ignore rotary changes that come in more quickly than this
        int minButtonDelay = 200; // ignore button presses taht come in more quickly than this
        int minTouchDelay = 200; // ignore touches that come in more quickly than this

        const int menuTimeout = 10;               // menu inactivity timeout (seconds)
        const bool menuLargeText = 0;             // show larger text when possible (if struggling to read the small text)
        const int maxmenuItems = 12;              // max number of items used in any of the menus (keep as low as possible to save memory)
        const int itemTrigger = 2;                // rotary encoder - counts per tick (varies between encoders usually 1 or 2)
        const int topLine = 18;                   // y position of lower area of the display (18 with two colour displays)
        const byte lineSpace1 = 9;                // line spacing for textsize 1 (small text)
        const byte lineSpace2 = 17;               // line spacing for textsize 2 (large text)
        const int displayMaxLines = 5;            // max lines that can be displayed in lower section of display in textsize1 (5 on larger oLeds)
        const int MaxmenuTitleLength = 10;        // max characters per line when using text size 2 (usually 10)

        RotaryEncoder* rotary;
        int rotaryLastPosition = 0;

        bool serialDebug = true;
        bool needsRepainted = false; // during interaction, dont repaint unless something changes

        CommunicatorEventHandler* handler;
        CommunicatorPreferenceHandler* prefHandler;
        CommunicatorEvent eventBuffer;  

        void setDatePartOptionsFor(uint8_t datePartId);

        // array of date parts for temp storing user selected date/time
        int selectedDateParts[6];

        ItemIterator* iterator; // we may need to clean this up
        char itemNameBuffer[ITERATOR_MAX_NAME_SIZE];
        uint8_t iteratorOffset = 0; // if we go past the max number of menu items, need to slide this offset forward
        uint8_t iteratorSelection = ITERATOR_SELECTION_NONE;

        uint8_t scrollOffset = 0; // if not in iterator menu, this goes up and down as the user scrolls off the starting position

        bool onboardAllowed = false;
        bool remoteAllowed = false; // whetehr this device is allowed to issue remote config
};

#endif