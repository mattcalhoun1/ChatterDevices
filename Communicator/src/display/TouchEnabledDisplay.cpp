#include "TouchEnabledDisplay.h"

void TouchEnabledDisplay::addTouchListener (TouchListener* _listener) {
    if (numListeners >= MAX_TOUCH_LISTENERS) {
        Serial.println("ERROR! Too many touch listeners!");
    }

    listeners[(numListeners >= MAX_TOUCH_LISTENERS ? 0 : numListeners++)] = _listener;
}
