#ifndef TOUCHCONTROL_H
#define TOUCHCONTROL_H

#include "Display.h"

class TouchControl {
    public:
        virtual bool init() = 0;
        virtual bool wasTouched(int& x, int& y, bool keyboardShowing, ScreenRotation keyboardOrientation) = 0;
        virtual void touchInterrupt () {touchIrq = true;}
        virtual void clearTouchInterrupt () {touchIrq = false;}
    
    protected:
        bool touchIrq = false;
};

#endif