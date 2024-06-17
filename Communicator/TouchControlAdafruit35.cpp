#include "TouchControlAdafruit35.h"
#include "Keyboard.h"

#ifdef TOUCH_CONTROL_ADAFRUIT_35

bool TouchControlAdafruit35::init() {
    touch.begin(); 
    return true;
}

bool TouchControlAdafruit35::wasTouched () {
    if(touchIrq > sensitivity || touch.touched()) {
        TS_Point p = touch.getPoint();

        // ignore edges
        if (p.y > 0 && p.y < DISPLAY_TFT_HEIGHT && p.x > 0 && p.x < DISPLAY_TFT_WIDTH) {
            return true;
        }
    }

    return false;
}

bool TouchControlAdafruit35::wasTouched(int& x, int& y, bool keyboardShowing, ScreenRotation keyboardOrientation) {
    if (touchIrq > sensitivity || touch.touched()) {
        touchIrq = 0;

        TS_Point p = touch.getPoint();

        // ignore edges
        if (p.y <= 0 || p.y >= DISPLAY_TFT_HEIGHT || p.x <= 0 || p.x >= DISPLAY_TFT_WIDTH) {
            return false;
        }

        // 3.5 driver doesn't seem to flip x/y like the 2.8 did
        //x = DISPLAY_TFT_WIDTH - p.x;
        //y = DISPLAY_TFT_HEIGHT - p.y;
        x = p.x;
        y = p.y;

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

#endif