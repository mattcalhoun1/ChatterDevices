#include "TouchControlAdafruit.h"
#include "Keyboard.h"

bool TouchControlAdafruit::init() {
    touch.begin(); 
    return true;
}

bool TouchControlAdafruit::wasTouched(int& x, int& y, bool keyboardShowing, ScreenRotation keyboardOrientation) {
    if (touchIrq > sensitivity || touch.touched()) {
        touchIrq = 0;

        TS_Point p = touch.getPoint();
        // ignore edges
        if (p.y <= 0 || p.y >= DISPLAY_TFT_HEIGHT || p.x <= 0 || p.x >= DISPLAY_TFT_WIDTH) {
            return false;
        }

        x = DISPLAY_TFT_WIDTH - p.x;
        y = DISPLAY_TFT_HEIGHT - p.y;

        // x/y will be different depending on how screen is rotated
        if (keyboardShowing) {
            if (keyboardOrientation == Landscape) {
                x = DISPLAY_TFT_HEIGHT - p.y;
                y = p.x;
            }
        }    

        return true;        
    }

    return false;
}