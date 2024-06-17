#include "Globals.h"

#ifdef TOUCH_CONTROL_ADAFRUIT_28

#ifndef TOUCHCONTROLADAFRUIT_H
#define TOUCHCONTROLADAFRUIT_H

#include "TouchControl.h"
#include <Adafruit_FT6206.h> // capacative touch
#include "Display.h"

class TouchControlAdafruit : public TouchControl {
    public:
        bool init ();
        bool wasTouched(int& x, int& y, bool keyboardShowing, ScreenRotation keyboardOrientation);
        bool wasTouched();

        void resetToDefaultTouchSensitivity () { sensitivity = TouchSensitivityHigh; }

    protected:
        Adafruit_FT6206 touch = Adafruit_FT6206();  
};

#endif

#endif