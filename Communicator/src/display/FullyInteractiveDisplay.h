#include "Display.h"
#include <Arduino.h>
#include "TouchEnabledDisplay.h"
#include "../keyboard/Keyboard.h"

#ifndef FULLYINTERACTIVEDISPLAY_H
#define FULLYINTERACTIVEDISPLAY_H

#define NUM_DISPLAYED_BUTTONS 3
#define NUM_INTERACTIVE_CONTEXTS 2

enum DisplayedButton {
    ButtonDM = 0,
    ButtonFilter = 1,
    ButtonMenu = 2,
    ButtonNone = 3,
    ButtonLock = 4,
    ButtonFlip = 5,
    ButtonBroadcast = 6, // not currently displayed,
    ButtonThermalSnap = 7,
    ButtonThermalSend = 8,
    ButtonThermalExit = 9
};

enum InteractiveContext {
    InteractiveHome = 0,
    InteractiveThermal = 1
};

class FullyInteractiveDisplay : public TouchEnabledDisplay {
    public:
        virtual int getModalInput (const char* title, int maxLength, CharacterFilter charFilter, char* buffer) = 0;
        virtual int getModalInput (const char* title, int maxLength, CharacterFilter charFilter, char* buffer, const char* defaultValue) = 0;
        virtual int getModalInput (const char* title, int maxLength, CharacterFilter charFilter, char* buffer, const char* defaultValue, int timeoutMillis) = 0;
        virtual int getModalInput (const char* title, const char* subtitle, int maxLength, CharacterFilter charFilter, char* buffer, const char* defaultValue, int timeoutMillis) = 0;
        void setDefaultKeyboard (Keyboard* _defaultKeyboard) { defaultKeyboard = _defaultKeyboard; }
        virtual bool isKeyboardShowing () {return defaultKeyboard->isShowing();}
        //virtual void touchInterrupt() = 0;
        //virtual void clearTouchInterrupts () = 0;


        virtual void showButtons (InteractiveContext context) = 0;
        virtual DisplayedButton getButtonAt (InteractiveContext context, int x, int y) = 0;
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
        //virtual const char* getButtonText (DisplayedButton btn) = 0;

        virtual int getLockButtonX() = 0;
        virtual int getLockButtonY() = 0;
        virtual int getLockButtonSize() = 0;

        virtual int getFlipButtonX() = 0;
        virtual int getFlipButtonY() = 0;
        virtual int getFlipButtonSize() = 0;

        virtual void showButton(InteractiveContext context, uint8_t buttonPosition);

        Keyboard* defaultKeyboard;
};

#endif