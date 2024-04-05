#include "Display.h"
#include <Arduino.h>
#include "MenuEnabledDisplay.h"

#ifndef TOUCHENABLEDDISPLAY_H
#define TOUCHENABLEDDISPLAY_H

#define MAX_TOUCH_LISTENERS 3

class TouchListener {
  public:
    virtual bool handleScreenTouched (int touchX, int touchY) = 0;
};


class TouchEnabledDisplay : public MenuEnabledDisplay {

    public:
        virtual int getKeyboardAreaX () = 0;
        virtual int getKeyboardAreaY () = 0;
        virtual int getKeyboardAreaWidth () = 0;
        virtual int getKeyboardAreaHeight () = 0;
        void setKeyboardOrientation (ScreenRotation _rotation) {keyboardOrientation = _rotation; }

        virtual void addTouchListener (TouchListener* _listener) {listeners[(numListeners >= MAX_TOUCH_LISTENERS-1 ? 0 : numListeners++)] = _listener;}
        virtual bool handleIfTouched () = 0;

        virtual void touchInterrupt () = 0;
      
    protected:
      uint8_t numListeners = 0;
      TouchListener* listeners[MAX_TOUCH_LISTENERS];
      ScreenRotation keyboardOrientation = Landscape; // default set here
};

#endif