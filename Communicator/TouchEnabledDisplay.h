#include "Display.h"
#include <Arduino.h>
#include "MenuEnabledDisplay.h"

#ifndef TOUCHENABLEDDISPLAY_H
#define TOUCHENABLEDDISPLAY_H

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

        virtual void setTouchListener (TouchListener* _listener) {listener = _listener;}
        virtual bool handleIfTouched () = 0;
      
    protected:
      TouchListener* listener;
      ScreenRotation keyboardOrientation = Landscape; // default set here
};

#endif