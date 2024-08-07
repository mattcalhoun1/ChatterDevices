#include "../globals/Globals.h"

#ifdef TOUCH_CONTROL_RAK

#ifndef TOUCHCONTROLRAK_H
#define TOUCHCONTROLRAK_H

#include "RAK14014_FT6336U.h"
#include "TouchControl.h"
#include "../display/Display.h"

class TouchControlRak : public TouchControl {
    public:
        bool init ();
        bool wasTouched(int& x, int& y, bool keyboardShowing, ScreenRotation keyboardOrientation);
        bool wasTouched();

        void resetToDefaultTouchSensitivity () { sensitivity = TouchSensitivityMedium; }
    
    protected:
        FT6336U ft6336u;
};

#endif

#endif