#ifndef TOUCHCONTROLNONE_H
#define TOUCHCONTROLNONE_H

#include "TouchControl.h"
#include "Globals.h"
#include "Display.h"

// just a placeholder for when touch is not available or is otherwise disabled
class TouchControlNone : public TouchControl {
    public:
        bool init () { return true; }
        bool wasTouched(int& x, int& y, bool keyboardShowing, ScreenRotation keyboardOrientation) { return false; }
};

#endif