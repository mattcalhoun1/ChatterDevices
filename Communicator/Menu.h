#include "MenuEnabledDisplay.h"
#include "TouchEnabledDisplay.h"
#include <RotaryEncoder.h>
#include "CommunicatorEventHandler.h"
#include "CommunicatorEvent.h"
#include "CommunicatorPreferenceHandler.h"
#include "Arduino.h"
#include "ItemIterator.h"

#ifndef MENU_H
#define MENU_H

#define MENU_MAX_ITEMS 10

#define MENU_MAIN_DIRECT_MESSAGE 1
#define MENU_MAIN_SECURE_BROADCAST 2
#define MENU_MAIN_ANNOUNCE_PRESENCE 3
#define MENU_CHOOSE_CLUSTER 4
#define MENU_MAIN_ONBOARDING 5
#define MENU_MAIN_CLEAR_MESSAGES 6
#define MENU_MAIN_ADMIN 7

#define MENU_ADMIN_PING_LORA_BRIDGE 1
#define MENU_ADMIN_CREATE_CLUSTER 2
#define MENU_ADMIN_DELETE_CLUSTER 3
#define MENU_ADMIN_SET_TIME 4
#define MENU_ADMIN_KEYBOARD_ORIENTATION 5
#define MENU_ADMIN_WIFI_ENABLE 6
#define MENU_ADMIN_MESSAGE_HISTORY 7
#define MENU_ADMIN_SECURE_FACTORY_RESET 8

#define MENU_ONBOARDING_JOIN_CLUSTER 1
#define MENU_ONBOARDING_ONBOARD_DEVICE 2

#define MENU_POWER_LOCK_SCREEN 1
#define MENU_POWER_POWEROFF 2

#define MENU_ID_MAIN 0
#define MENU_ID_MESSAGES 1
#define MENU_ID_ONBOARDING 2
#define MENU_ID_ADMIN 3
#define MENU_ID_ITERATOR 4
#define MENU_ID_POWER 5

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
        Menu(MenuEnabledDisplay* _display, RotaryEncoder* _rotary, CommunicatorEventHandler* _handler, bool _onboardAllowed, CommunicatorPreferenceHandler* _prefHandler) { display = _display; rotary = _rotary; handler = _handler; onboardAllowed = _onboardAllowed; prefHandler = _prefHandler;}

        bool init ();

        void mainMenu (bool fullRepaint = false);
        void adminMenu();
        void onboardingMenu ();
        void powerMenu();
        void iteratorMenu (bool fullRepaint = false);
        void iteratorActions ();        

        // leftover from demo
        void demoMenu ();
        void menuActions ();
        void onboardingActions ();
        void powerMenuActions ();


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
        void adminActions ();
        void mainActions ();

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
};

#endif