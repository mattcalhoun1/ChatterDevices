#include "TouchControlRak.h"

bool TouchControlRak::init() {
    ft6336u.begin();
    return true;
}

bool TouchControlRak::wasTouched(int& x, int& y, bool keyboardShowing, ScreenRotation keyboardOrientation) {
    return false;
}