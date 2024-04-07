#include "Display.h"
#include <Arduino.h>

#ifndef MENUENABLEDDISPLAY_H
#define MENUENABLEDDISPLAY_H

class MenuEnabledDisplay : public Display {

    public:
        virtual int getMenuAreaX () = 0;
        virtual int getMenuAreaY () = 0;
        virtual int getMenuAreaWidth () = 0;
        virtual int getMenuAreaHeight () = 0;

        virtual int getMenuLineHeight () = 0;
        virtual int getMenuMaxItemLength () = 0;
        virtual int getMenuItemIndent() = 0;

        virtual int getMenuScrollUpX() = 0;
        virtual int getMenuScrollUpY() = 0;
        virtual int getMenuScrollDownX() = 0;
        virtual int getMenuScrollDownY() = 0;
        virtual int getMenuScrollRadius() = 0;

        virtual int getMenuBorderSize() {return 2;}

        void clearMenuTitle ();
        void clearMenu ();
        void showMenuTitle (String& title);
        void showMenuItem (uint8_t itemNumber, String& text, DisplayColor textColor, DisplayColor backgroundColor);

        void drawMenuBorder ();
        virtual bool isTouchEnabled () = 0;
        void showScrolls (bool scrollUpEnabled, bool scrollDownEnabled);
};

#endif