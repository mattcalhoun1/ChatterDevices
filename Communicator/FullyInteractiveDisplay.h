#include "Display.h"
#include <Arduino.h>
#include "TouchEnabledDisplay.h"
#include "Keyboard.h"

#ifndef FULLYINTERACTIVEDISPLAY_H
#define FULLYINTERACTIVEDISPLAY_H

enum CharacterFilter {
  CharacterFilterNumeric = 0,
  CharacterFilterAlpha = 1,
  CharacterFilterAlphaNumeric = 2,
  CharacterFilterNone = 3
};

class FullyInteractiveDisplay : public TouchEnabledDisplay {
    public:
        virtual int getModalInput (const char* title, int maxLength, CharacterFilter charFilter, char* buffer) = 0;
        virtual int getModalInput (const char* title, int maxLength, CharacterFilter charFilter, char* buffer, char* defaultValue) = 0;
        void setDefaultKeyboard (Keyboard* _defaultKeyboard) { defaultKeyboard = _defaultKeyboard; }

    protected:
        virtual int getModalTitleX () = 0;
        virtual int getModalTitleY () = 0;
        virtual int getModalTitleHeight () = 0;
        virtual int getModalInputX () = 0;
        virtual int getModalInputY () = 0;
        virtual int getModalInputWidth () = 0;
        virtual int getModalInputHeight () = 0;
        Keyboard* defaultKeyboard;
};

#endif