#ifndef TOUCHCONTROL_H
#define TOUCHCONTROL_H

#include "../display/Display.h"

enum TouchSensitivity {
  TouchSensitivityHigh = 1,
  TouchSensitivityMedium = 2,
  TouchSensitivityLow = 3
};


class TouchControl {
    public:
        virtual bool init() = 0;
        virtual bool wasTouched(int& x, int& y, bool keyboardShowing, ScreenRotation keyboardOrientation) = 0;
        virtual bool wasTouched() = 0;
        virtual void touchInterrupt () {touchIrq++;}
        virtual void clearTouchInterrupt () {touchIrq = 0;}
        virtual void setTouchSenstivity(TouchSensitivity _sensitivity) { sensitivity = _sensitivity; }
        virtual void resetToDefaultTouchSensitivity () = 0;

    protected:
        uint8_t touchIrq = 0;
        TouchSensitivity sensitivity;

};

#endif