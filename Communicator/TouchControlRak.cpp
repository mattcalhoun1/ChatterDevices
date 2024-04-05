#include "TouchControlRak.h"

bool TouchControlRak::init() {
    // reset the touch
    digitalWrite(PIN_TOUCH_RS, LOW);
    delay(1000);
    digitalWrite(PIN_TOUCH_RS, HIGH);
    delay(1000);
    /*analogWrite(PIN_TOUCH_RS, 1);
    delay(1000);
    analogWrite(PIN_TOUCH_RS, 0);
    delay(1000);*/


    Serial.println("FT6336U touch initializing"); 
    int maxRetries = 5;
    int retryCount = 0;
    while(ft6336u.begin() == false && ++retryCount <= maxRetries) 
    {
            Serial.println("ft6336u is not connected.");
            delay(1000);
    }

    Serial.print("FT6336U Firmware Version: "); 
    Serial.println(ft6336u.read_firmware_id());  
    Serial.print("FT6336U Device Mode: "); 
    Serial.println(ft6336u.read_device_mode());      
    return true;
}

bool TouchControlRak::wasTouched(int& x, int& y, bool keyboardShowing, ScreenRotation keyboardOrientation) {
    if (touchIrq || ft6336u.read_td_status() > 0) {
        touchIrq = false;
        int freshX = ft6336u.read_touch1_x();
        int freshY = ft6336u.read_touch1_y();

        // ignore edges
        if (freshY <= 0 || freshY >= DISPLAY_TFT_HEIGHT || freshX <= 0 || freshX >= DISPLAY_TFT_WIDTH) {
            return false;
        }

        x = freshX;
        y = freshY;

        //x = DISPLAY_TFT_WIDTH - p.x;
        //y = DISPLAY_TFT_HEIGHT - p.y;

        // x/y will be different depending on how screen is rotated
        if (keyboardShowing) {
            if (keyboardOrientation == Landscape) {
                x = freshY;
                y = DISPLAY_TFT_WIDTH - freshX;
            }
        }    

        return true;
    }
    return false;
}