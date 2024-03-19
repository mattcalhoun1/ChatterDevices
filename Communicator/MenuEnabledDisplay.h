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

        void clearMenuTitle ();
        void clearMenu ();
        void showMenuTitle (String& title);
        void showMenuItem (uint8_t itemNumber, String& text, DisplayColor textColor, DisplayColor backgroundColor);
};

#endif