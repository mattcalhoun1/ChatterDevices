#include "Globals.h"

#ifdef TOUCH_CONTROL_ADAFRUIT_35

#ifndef TOUCHCONTROLADAFRUIT35_H
#define TOUCHCONTROLADAFRUIT35_H
#include "TouchControl.h"
#include <Adafruit_FT5336.h> // capacative touch
#include "Display.h"

class TouchControlAdafruit35 : public TouchControl {
    public:
        bool init ();
        bool wasTouched(int& x, int& y, bool keyboardShowing, ScreenRotation keyboardOrientation);
        bool wasTouched();
        void resetToDefaultTouchSensitivity () { sensitivity = TouchSensitivityHigh; }

    protected:
        Adafruit_FT5336 touch = Adafruit_FT5336();  
};

#endif

#endif