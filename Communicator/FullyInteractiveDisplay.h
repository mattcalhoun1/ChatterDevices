#include "Display.h"
#include <Arduino.h>
#include "TouchEnabledDisplay.h"
#include "Keyboard.h"

#ifndef FULLYINTERACTIVEDISPLAY_H
#define FULLYINTERACTIVEDISPLAY_H

#define NUM_DISPLAYED_BUTTONS 3
enum DisplayedButton {
    ButtonDM = 0,
    ButtonBroadcast = 1,
    ButtonMenu = 2,
    ButtonNone = 3
};

class FullyInteractiveDisplay : public TouchEnabledDisplay {
    public:
        virtual int getModalInput (const char* title, int maxLength, CharacterFilter charFilter, char* buffer) = 0;
        virtual int getModalInput (const char* title, int maxLength, CharacterFilter charFilter, char* buffer, const char* defaultValue) = 0;
        virtual int getModalInput (const char* title, int maxLength, CharacterFilter charFilter, char* buffer, const char* defaultValue, int timeoutMillis) = 0;
        virtual int getModalInput (const char* title, const char* subtitle, int maxLength, CharacterFilter charFilter, char* buffer, const char* defaultValue, int timeoutMillis) = 0;
        void setDefaultKeyboard (Keyboard* _defaultKeyboard) { defaultKeyboard = _defaultKeyboard; }
        virtual bool isKeyboardShowing () {return defaultKeyboard->isShowing();}
        virtual void touchInterrupt() = 0;
        virtual void clearTouchInterrupts () = 0;


        virtual void showButtons () = 0;
        virtual DisplayedButton getButtonAt (int x, int y) = 0;
        virtual bool handleIfTouched () = 0;
    protected:
        virtual int getModalTitleX () = 0;
        virtual int getModalTitleY () = 0;
        virtual int getModalTitleHeight () = 0;

        virtual int getModalSubTitleX () = 0;
        virtual int getModalSubTitleY () = 0;
        virtual int getModalSubTitleHeight () = 0;

        virtual int getModalInputX () = 0;
        virtual int getModalInputY () = 0;
        virtual int getModalInputWidth () = 0;
        virtual int getModalInputHeight () = 0;

        virtual int getButtonAreaX() = 0;
        virtual int getButtonAreaY() = 0;
        virtual int getButtonWidth() = 0;
        virtual int getButtonHeight() = 0;
        virtual int getButtonHorizontalOffset() = 0;
        virtual const char* getButtonText (DisplayedButton btn) = 0;

        virtual void showButton(uint8_t buttonPosition, const char* buttonText);

        Keyboard* defaultKeyboard;
};

#endif