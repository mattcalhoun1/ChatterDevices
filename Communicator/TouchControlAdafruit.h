#ifndef TOUCHCONTROLADAFRUIT_H
#define TOUCHCONTROLADAFRUIT_H

#include "TouchControl.h"
#include "Globals.h"
#include <Adafruit_FT6206.h> // capacative touch
#include "Display.h"

class TouchControlAdafruit : public TouchControl {
    public:
        bool init ();
        bool wasTouched(int& x, int& y, bool keyboardShowing, ScreenRotation keyboardOrientation);

    protected:
        Adafruit_FT6206 touch = Adafruit_FT6206();  
};

#endif