#include "Display.h"
#include <Arduino.h>

#ifndef MENUENABLEDDISPLAY_H
#define MENUENABLEDDISPLAY_H

enum MenuScrollButton {
    MenuScrollUp = 0,
    MenuScrollDown = 1,
    MenuScrollNone = 2
};

#define MENU_ITEM_NONE 255

class MenuEnabledDisplay : public Display {

    public:
        virtual int getMenuAreaX () = 0;
        virtual int getMenuAreaY () = 0;
        virtual int getMenuAreaWidth () = 0;
        virtual int getMenuAreaHeight () = 0;

        virtual int getMenuTitleX () = 0;
        virtual int getMenuTitleY () = 0;

        virtual int getMenuLineHeight () = 0;
        virtual int getMenuMaxItemLength () = 0;
        virtual int getMenuItemIndent() = 0;

        virtual int getMenuScrollUpX() = 0;
        virtual int getMenuScrollUpY() = 0;
        virtual int getMenuScrollDownX() = 0;
        virtual int getMenuScrollDownY() = 0;
        virtual int getMenuScrollRadius() = 0;
        virtual int getMenuScrollTouchRadius() = 0;

        virtual MenuScrollButton getMenuScrollButtonAt (int x, int y);

        virtual int getMenuBorderSize() {return 2;}

        void clearMenuTitle ();
        void clearMenu ();
        void showMenuTitle (String& title);
        void showMenuItem (uint8_t itemNumber, String& text, DisplayColor textColor, DisplayColor backgroundColor);
        uint8_t getMenuItemAt (int x, int y);

        void drawMenuBorder ();
        virtual bool isTouchEnabled () = 0;

        void blurMenuBackground ();

        void showMenuScrolls (bool _scrollUpEnabled, bool _scrollDownEnabled);
        bool isMenuScrollUpEnabled () { return menuScrollUpEnabled; }
        bool isMenuScrollDownEnabled () { return menuScrollDownEnabled; }
    protected:
        bool menuScrollUpEnabled = false;
        bool menuScrollDownEnabled = false;
        uint8_t numDisplayableItems = 5;
};

#endif