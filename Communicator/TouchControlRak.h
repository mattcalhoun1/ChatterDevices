#ifndef TOUCHCONTROLRAK_H
#define TOUCHCONTROLRAK_H

#include "RAK14014_FT6336U.h"
#include "TouchControl.h"
#include "Globals.h"
#include "Display.h"

class TouchControlRak : public TouchControl {
    public:
        bool init ();
        bool wasTouched(int& x, int& y, bool keyboardShowing, ScreenRotation keyboardOrientation);
    
    protected:
        FT6336U ft6336u;
};

#endif