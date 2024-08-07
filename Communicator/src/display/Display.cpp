#include <Arduino.h>
#include "Display.h"

void Display::logConsole (String msg) {
  if (LOG_ENABLED) {
    Serial.println(msg);
  }
}

void Display::showStatus (String text, DisplayColor color) {
  clearStatus();
  changeFont(FontPico);
  showText(text, getStatusX(), getStatusY() - 2, TextSmall, color);
  changeFont(FontNormal);
}

void Display::clearStatus () {
  clearArea(getStatusX(), getStatusY() - (getTextLowerVerticalOffset(TextSmall) + 2), getStatusWidth(), getStatusHeight() - getTextLowerVerticalOffset(TextSmall));
}

void Display::showMessage (const char* message, DisplayColor color, uint8_t position) {
  if (strlen(message) > 24) {
    changeFont(FontTiny);
  }
  else {
    changeFont(FontNormal);
  }
  showText(message, getMessageAreaX(), getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())), TextSmall, color);
  changeFont(FontNormal);
}

void Display::showMessageAndTitle (const char* title, const char* text, const char* readableTS, bool received, char status, char sendMethod, DisplayColor titleColor, DisplayColor messageColor, uint8_t position) {
  // show a line above the message for separation
  if (position != 0) {
    int lineYPos = getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())) - (getTextUpperVerticalOffset(TextSmall) + 2) - getMessageTitleYOffset()*2;
    drawLine(getMessageAreaX(), lineYPos, getMessageAreaX() + getMessageAreaWidth(), lineYPos, Beige);
  }

  changeFont(FontBold);
  showText(
    title, 
    getMessageAreaX() + getMessageTitleXOffset(), 
    getMessageAreaY() - getMessageTitleYOffset() + (position * (getMessageHeight() + getMessageTitleHeight())), 
    TextSmall, 
    received ? titleColor : LightGray
  );

  changeFont(FontSystem);
  showSymbol(
    received ? 0x19 : 0x18, 
    getMessageAreaX() + 5, 
    getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())) - (getTextLowerVerticalOffset(TextSmall)+getMessageTitleIconOffset()),
    received ? titleColor : LightGray);

  // add message timestamp
  changeFont(FontTiny);
  showText(
    readableTS, 
    getMessageAreaX() + getMessageAreaWidth() - getMessageTitleTsXOffset(), 
    getMessageAreaY() - getMessageTitleYOffset() + (position * (getMessageHeight() + getMessageTitleHeight())) - (getTextUpperVerticalOffset(TextSmall) - 2), 
    TextSmall, 
    received ? titleColor : LightGray);
  
  // if it was a direct message, add an ack indicator
  if (sendMethod == 'D') {
    if (status == 'A') {
      // checkmark
      drawLine(
        getMessageAreaX() + getMessageAreaWidth() - (getMessageTitleTsXOffset()+getMessageStatusXOffset()+4),
        getMessageAreaY() - getMessageTitleIconOffset()*1.5 + (position * (getMessageHeight() + getMessageTitleHeight())) - getTextUpperVerticalOffset(TextSmall) + getMessageStatusRadius()+1,
        getMessageAreaX() + getMessageAreaWidth() - (getMessageTitleTsXOffset() + getMessageStatusXOffset()+2),
        getMessageAreaY() - getMessageTitleIconOffset()*1.5 + (position * (getMessageHeight() + getMessageTitleHeight())) - getTextUpperVerticalOffset(TextSmall) + getMessageStatusRadius()+3,
        received ? titleColor : LightGray
      );

      drawLine(
        getMessageAreaX() + getMessageAreaWidth() - (getMessageTitleTsXOffset()+getMessageStatusXOffset()+2),
        getMessageAreaY() - getMessageTitleIconOffset()*1.5 + (position * (getMessageHeight() + getMessageTitleHeight())) - getTextUpperVerticalOffset(TextSmall) + getMessageStatusRadius()+3,
        getMessageAreaX() + getMessageAreaWidth() - (getMessageTitleTsXOffset()+ getMessageStatusXOffset()),
        getMessageAreaY() - getMessageTitleIconOffset()*1.5 + (position * (getMessageHeight() + getMessageTitleHeight())) - getTextUpperVerticalOffset(TextSmall) + getMessageStatusRadius()-2,
        received ? titleColor : LightGray
      );
    }
  }
  else if (sendMethod == 'I') { // if sent via intermediary (mesh)
    // if it was accepted, draw a circle
    if (status == 'M') {
      drawCircle(
        getMessageAreaX() + getMessageAreaWidth() - (getMessageTitleTsXOffset()+getMessageStatusXOffset()),
        getMessageAreaY() - getMessageTitleIconOffset()*1.5 + (position * (getMessageHeight() + getMessageTitleHeight())) - getTextUpperVerticalOffset(TextSmall) + getMessageStatusRadius(),
        getMessageStatusRadius()-2,
        received ? LightGray : BrightGreen
      );
    }
    else if (status == 'N') {
      // not yet accepted, just a dot      
      fillCircle(
        getMessageAreaX() + getMessageAreaWidth() - (getMessageTitleTsXOffset()+getMessageStatusXOffset()),
        getMessageAreaY() - getMessageTitleIconOffset()*1.5 + (position * (getMessageHeight() + getMessageTitleHeight())) - getTextUpperVerticalOffset(TextSmall) + getMessageStatusRadius(),
        getMessageStatusRadius()-4,
        received ? LightGray : BrightGreen
      );
    }
    else if (status == 'A') { // it was acknowledged, cricle with dot
      drawCircle(
        getMessageAreaX() + getMessageAreaWidth() - (getMessageTitleTsXOffset()+getMessageStatusXOffset()),
        getMessageAreaY() - getMessageTitleIconOffset()*1.5 + (position * (getMessageHeight() + getMessageTitleHeight())) - getTextUpperVerticalOffset(TextSmall) + getMessageStatusRadius(),
        getMessageStatusRadius() - 2,
        received ? LightGray : BrightGreen
      );
      fillCircle(
        getMessageAreaX() + getMessageAreaWidth() - (getMessageTitleTsXOffset()+getMessageStatusXOffset()),
        getMessageAreaY() - getMessageTitleIconOffset()*1.5 + (position * (getMessageHeight() + getMessageTitleHeight())) - getTextUpperVerticalOffset(TextSmall) + getMessageStatusRadius(),
        getMessageStatusRadius() - 3,
        received ? LightGray : BrightGreen
      );
    }
    changeFont(FontNormal);

  }
  

  changeFont(FontMidSize);

  // max of 32 chars per line
  int msgLen = strlen(text);
  if (msgLen > 32) {
    uint8_t charsShown = 0;
    uint8_t lineNum = 0;
    uint8_t thisLineSize = 0;
    while (charsShown < msgLen) {
      memset(textBuffer, 0, 64);
      thisLineSize = min(32, msgLen - charsShown);
      memcpy(textBuffer, text+charsShown, thisLineSize);
      showText(
        textBuffer,
        getMessageAreaX(), 
        getMessageAreaY() + getMessageTitleHeight() + (position * (getMessageHeight() + getMessageTitleHeight())) + (14*lineNum) - getMessageTitleYOffset(), 
        TextSmall, 
        received ? messageColor : DarkGray);
      charsShown += thisLineSize;
      lineNum++;
    }
  }
  else {
    showText(
      text,
      getMessageAreaX(), 
      getMessageAreaY() + getMessageTitleHeight() + (position * (getMessageHeight() + getMessageTitleHeight())) - getMessageTitleYOffset(), 
      TextSmall, 
      received ? messageColor : DarkGray);
  }
  changeFont(FontNormal);
}


uint8_t Display::getMessagePosition (int positionX, int positionY) {
  // the touch must be toward the title area (left)
  if (positionX < getScreenWidth() / 2) {
    // look mainly at y position, as that's what identifies one position versus another

    // subtract message start
    //int lineYPos = getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())) - (getTextUpperVerticalOffset(TextSmall) + 2) - getMessageTitleYOffset()*2;
    int shiftedY = positionY - getMessageAreaY();//(getMessageAreaY() - getTextUpperVerticalOffset(TextSmall) + 2);// - getMessageTitleYOffset()*2;

    #if defined(DISPLAY_TYPE_ADAFRUIT_35)
      shiftedY += 12;
    #endif

    //Serial.print("Shifted y: ");Serial.print(shiftedY);
    //Serial.print(", message area y: ");Serial.println(getMessageAreaY());

    if (shiftedY > 0) {
      // divide by message height to get the position
      uint8_t selected = shiftedY / (getMessageHeight() + getMessageTitleHeight());
      if (selected < getMaxDisplayableMessages()) {

        // must be top half of the message height (title)
        if ((shiftedY + (getMessageHeight() / 2)) / (getMessageHeight() + getMessageTitleHeight()) == selected) {
          return selected;
        }
      }
    }
  }

  return DISPLAY_MESSAGE_POSITION_NULL;
}

DisplayColor Display::getColorForConnectionQuality(uint8_t connectionQuality) {
  if (connectionQuality >= 4) {
    return Green;
  }
  else if (connectionQuality == 3) {
    return LightBlue;
  }
  else if (connectionQuality == 2) {
    return Yellow;
  }
  else if (connectionQuality == 1) {
    return Orange;
  }
  else {
    return DarkRed;
  }
}

void Display::getRgbwForConnectionQuality(uint8_t connectionQuality, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& w) {
  if (connectionQuality >= 4) {
    r = 0;
    g = 254;
    b = 0;
    w = 0;
  }
  else if (connectionQuality == 3) {
    r = 0;
    g = 0;
    b = 254;
    w = 0;
  }
  else if (connectionQuality == 2) {
    r = 254;
    g = 254;
    b = 0;
    w = 0;
  }
  else if (connectionQuality == 1) {
    r = 255;
    g = 69;
    b = 8;
    w = 0;
  }
  else {
    r = 254;
    g = 0;
    b = 0;
    w = 0;
  }
}
const char* Display::getChannelName(char pingChannelId) {
  switch(pingChannelId) {
    case 'U': // udp
      return NEARBY_CHANNEL_NAME_WIFI;
    case 'L':
      return NEARBY_CHANNEL_NAME_LORA;
    case 'W': // wire
      return NEARBY_CHANNEL_NAME_WIRE;
  }

  return NEARBY_CHANNEL_NAME_NONE;
}

void Display::showNearbyDevice (const char* deviceAlias, const char* deviceId, uint8_t connectionQuality, uint8_t meshDirectRating, uint8_t meshIndirectRating, const char* readableTS, bool isTrusted, int16_t rssi, char channel, char secondaryChannel, DisplayColor titleColor, DisplayColor messageColor, uint8_t position) {
  // show a line above the message for separation
  if (position != 0) {
    int lineYPos = getMessageAreaY() - 3 + (position * (getMessageHeight() + getMessageTitleHeight())) - (getTextUpperVerticalOffset(TextSmall) + 2) - getMessageTitleYOffset()*2;
    drawLine(getMessageAreaX(), lineYPos, getMessageAreaX() + getMessageAreaWidth(), lineYPos, Beige);
  }

  int dotYPos = getMessageAreaY() - getMessageTitleYOffset() + (position * (getMessageHeight() + getMessageTitleHeight())) - getTextLowerVerticalOffset(TextSmall) + 1;
  fillCircle(getMessageAreaX(), dotYPos, getTickerSize()-1, getColorForConnectionQuality(connectionQuality));

  changeFont(isTrusted ? FontBold : FontNormal);
  showText(
    deviceAlias, 
    getMessageAreaX() + 15, 
    getMessageAreaY() - getMessageTitleYOffset() + (position * (getMessageHeight() + getMessageTitleHeight())), 
    TextSmall, 
    isTrusted ? titleColor : LightGray
  );

  // add channels
  changeFont(FontPico);
  showText(
    getChannelName(channel), 
    getMessageAreaX() - getMessageStatusXOffset()*2.5 + getMessageAreaWidth() - getMessageTitleTsXOffset(), 
    getMessageAreaY() - (getMessageTitleYOffset() + 4) + (position * (getMessageHeight() + getMessageTitleHeight())) - (getTextUpperVerticalOffset(TextSmall) - 2), 
    TextSmall, 
    isTrusted ? titleColor : LightGray);

  if (secondaryChannel != channel) {
    showText(
      getChannelName(secondaryChannel), 
      getMessageAreaX() - getMessageStatusXOffset()*2.5 + getMessageAreaWidth() - getMessageTitleTsXOffset(), 
      getMessageAreaY() - (9 + getMessageTitleYOffset()) + (position * (getMessageHeight() + getMessageTitleHeight())) - (getTextUpperVerticalOffset(TextSmall) - 2), 
      TextSmall, 
      isTrusted ? titleColor : LightGray);
  }
  

  // add message timestamp
  changeFont(FontTiny);
  showText(
    readableTS, 
    getMessageAreaX() + getMessageAreaWidth() - 65, 
    getMessageAreaY() - getMessageTitleYOffset() + (position * (getMessageHeight() + getMessageTitleHeight())) - (getTextUpperVerticalOffset(TextSmall) - 2), 
    TextSmall, 
    isTrusted ? titleColor : LightGray);

  memset(textBuffer, 0, 64);
  sprintf(textBuffer, "%s %s:%02d, %s:%02d", deviceId, "RSSI", rssi, "Mesh Rating", meshDirectRating);

  
  changeFont(FontTiny);
  showText(
    textBuffer,
    getMessageAreaX(), 
    getMessageAreaY() + getMessageTitleHeight() + (position * (getMessageHeight() + getMessageTitleHeight())) - getMessageTitleYOffset(), 
    //getMessageAreaY() - getMessageTitleYOffset() + getMessageTitleHeight() + (position * (getMessageHeight() + getMessageTitleHeight())), 
    TextSmall, 
    isTrusted ? messageColor : DarkGray);
  changeFont(FontNormal);
}

uint8_t Display::getNearbyDevicePosition (int positionX, int positionY) {
  return getMessagePosition(positionX, positionY);
}


void Display::showAlert (const char* alertText, const char* alertSubText, AlertType alertType) {
    DisplayColor color = BrightGreen;
    if (alertType == AlertWarning) {
        color = Beige;
    }
    else if (alertType == AlertError) {
        color = DarkRed;
    }
    else if (alertType == AlertActivity) {
      color = LightBlue;
    }
    clearAll();
    //clearArea(0, getAlertAreaY() - getTextUpperVerticalOffset(TextMedium), getScreenWidth(), getAlertAreaHeight());
    changeFont(FontNormal);
    showText(alertText, getAlertAreaX(), getAlertAreaY(), TextSmall, color);
    showText(alertSubText, getAlertSubAreaX(), getAlertSubAreaY(), TextSmall, color);
}

void Display::showAlert (const char* alertText, AlertType alertType) {
    DisplayColor color = BrightGreen;
    if (alertType == AlertWarning) {
        color = Beige;
    }
    else if (alertType == AlertError) {
        color = DarkRed;
    }
    else if (alertType == AlertActivity) {
      color = LightBlue;
    }
    clearAll();
    //clearArea(0, getAlertAreaY() - getTextUpperVerticalOffset(TextMedium), getScreenWidth(), getAlertAreaHeight());
    changeFont(FontNormal);
    showText(alertText, getAlertAreaX(), getAlertAreaY(), TextSmall, color);
}

void Display::showMainMessage (const char* messageText, const char* mainSubMessage, AlertType alertType) {
    DisplayColor color = BrightGreen;
    if (alertType == AlertWarning) {
        color = Beige;
    }
    else if (alertType == AlertError) {
        color = DarkRed;
    }
    else if (alertType == AlertActivity) {
      color = LightBlue;
    }
    
    clearArea(0, getMainAreaY() - getTextUpperVerticalOffset(TextSmall), getScreenWidth(), getMainAreaHeight());
    changeFont(FontUpSize);
    showText(messageText, getMainAreaX(), getMainAreaY(), TextSmall, color);

    clearArea(0, getMainSubAreaY() - getTextUpperVerticalOffset(TextSmall), getScreenWidth(), getMainSubAreaHeight());
    changeFont(FontMidSize);
    showText(mainSubMessage, getMainSubAreaX(), getMainSubAreaY(), TextSmall, color);
    changeFont(FontNormal);
}

void Display::clearMessageArea () {
  clearArea(max(0, getMessageAreaX() - 5), getMessageAreaY() - (getTextUpperVerticalOffset(TextSmall) + getMessageTitleYOffset()*2), getScreenWidth(), getMessageAreaHeight() + 1);
}

void Display::showInterpolatedThermal (const uint8_t* image, bool isAlt, String subtitle) {
  int xOffset = getImageAreaX(isAlt);
  int yOffset = getImageAreaY(isAlt);
  for (int r = 0; r < encoder->getInterpolatedResolutionHeight(); r++) {
    const float* interpolatedRow = encoder->getInterpolatedRow(image, r);
    showInterpolatedThermalRow(interpolatedRow, xOffset, r + yOffset);
  }

  clearArea(xOffset + getImageAreaX(isAlt), yOffset + encoder->getInterpolatedResolutionHeight() + getImageSubtitleY(isAlt), encoder->getInterpolatedResolutionWidth(), 7);
  showText(subtitle, xOffset + getImageSubtitleX(isAlt), yOffset + encoder->getInterpolatedResolutionHeight() + getImageSubtitleY(isAlt), TextSmall, White);
}

void Display::showInterpolatedThermal (const float* image, bool isAlt, String subtitle) {
  int xOffset = getImageAreaX(isAlt);
  int yOffset = getImageAreaY(isAlt);

  // temp clear
  //clearArea(xOffset, yOffset, THERMAL_INTERPOLATED_WIDTH, THERMAL_INTERPOLATED_HEIGHT);
  for (int r = 0; r < encoder->getInterpolatedResolutionHeight(); r++) {
    const float* interpolatedRow = encoder->getInterpolatedRow(image, r);
    showInterpolatedThermalRow(interpolatedRow, xOffset, r + yOffset);
  }
  clearArea(xOffset + getImageAreaX(isAlt), yOffset + encoder->getInterpolatedResolutionHeight() + getImageSubtitleY(isAlt), encoder->getInterpolatedResolutionWidth(), 7);
  showText(subtitle, xOffset + getImageSubtitleX(isAlt), yOffset + encoder->getInterpolatedResolutionHeight() + getImageSubtitleY(isAlt), TextSmall, White);
}

void Display::showTitle (const char* text) {
  clearArea(getTitleAreaX(), getTitleAreaY() - getTextUpperVerticalOffset(getTitleTextSize()), getTitleAreaWidth(), getTitleAreaHeight() - getTextLowerVerticalOffset(getTitleTextSize()));
  //changeFont(FontBold);
  changeFont(FontBold);
  fillRect(1, 1, getScreenWidth() - 2, getTitleAreaHeight(), DarkGray);
  showText(text, calculateTitleX(text), getTitleAreaY(), getTitleTextSize(), getTitleColor());

  // line at base of title/subtitle area
  //fillRect(1, getTitleAreaHeight() + getSubtitleAreaHeight(), getScreenWidth() - 2, 2, Beige);
}

void Display::showSubtitle (const char* text) {

  #if defined(DISPLAY_TYPE_ADAFRUIT_35)
  //clearArea(getMenuAreaX(), getMenuAreaY() + getMenuLineHeight() - 16, getMenuAreaWidth(), getMenuAreaHeight() - getMenuLineHeight(), DarkGreen);
  clearArea(getSubtitleAreaX(), getSubtitleAreaY() - 16, getSubtitleAreaWidth(), getSubtitleAreaHeight() - (getTextLowerVerticalOffset(TextSmall)+1));
  #else
  clearArea(getSubtitleAreaX(), getSubtitleAreaY() - getTextUpperVerticalOffset(TextSmall), getSubtitleAreaWidth(), getSubtitleAreaHeight() - (getTextLowerVerticalOffset(TextSmall)+1));
  #endif

  // change to smaller font
  changeFont(FontTiny);
  showText(text, calculateSubtitleX(text), getSubtitleAreaY() - getTextUpperVerticalOffset(TextSmall), getSubtitleTextSize(), getSubtitleColor());
  changeFont(FontNormal);
}

void Display::clearAll () {
  clearArea(0, 0, getScreenWidth(), getScreenHeight());
}

void Display::clearDashboard () {
  clearArea(getDashboardAreaX(), getDashboardAreaY() - getTextUpperVerticalOffset(TextSmall), getDashboardAreaWidth(), getDashboardAreaHeight(), DarkGray);
}

void Display::alertUnreadMessage (uint8_t numFlashes) {
  #if defined(ADAFRUIT_FEATHER_M4_EXPRESS)
  if (!statusPixelReady) {
    statusPixel.begin();
    statusPixel.setBrightness(32);
    statusPixel.show(); // Initialize all pixels to 'off'
    statusPixelReady = true;
  }

  for (uint8_t flashCount = 0; flashCount < numFlashes; flashCount++) {
    statusPixel.setPixelColor(0, 254, 254, 254, 0);
    statusPixel.show();
    delay(100);

    statusPixel.setPixelColor(0, 0, 0, 0, 0);
    statusPixel.show();

    if (flashCount+1 < numFlashes) {
      delay(100);
    }
  }

  #endif

}

void Display::showStartupScreen (float progress) {
  changeFont(FontNormal);

  clear();

  resetProgress();

  showProgressBar(progress);

  // show splash screen
  changeFont(FontUpSize);
  showText(APP_TITLE, DISPLAY_TFT_SPLASH_TITLE_X, DISPLAY_TFT_SPLASH_TITLE_Y, TextSmall, DarkBlue);
  changeFont(FontTiny);
  showText(APP_SUBTITLE, DISPLAY_TFT_SPLASH_SUBTITLE_X, DISPLAY_TFT_SPLASH_SUBTITLE_Y, TextSmall, DarkBlue);
  changeFont(FontNormal);
}


void Display::showTick (uint8_t connectionQuality) {
  // if the ticker is showing hide it
  if (tickerShowing) {
    fillCircle(getTickerX(), getTickerY(), getTickerSize(), getColorForConnectionQuality(connectionQuality));
  }
  else {
    fillCircle(getTickerX(), getTickerY(), getTickerSize(), DarkGreen);
  }

  #if defined(ADAFRUIT_FEATHER_M4_EXPRESS)
  if (!statusPixelReady) {
    statusPixel.begin();
    statusPixel.setBrightness(32);
    statusPixel.show(); // Initialize all pixels to 'off'
    statusPixelReady = true;
  }
  if (tickerShowing) {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t w;
    getRgbwForConnectionQuality(connectionQuality, r, g, b, w);
    statusPixel.setPixelColor(0, r, g, b, w);
  }
  else {
    statusPixel.setPixelColor(0, 0, 0, 0, 0);
  }
  statusPixel.show();
  #endif

  // toggle the ticker flag
  tickerShowing = !tickerShowing;
}

void Display::showDashboardItems (const char* item, DisplayColor itemColor[], uint8_t numItems) {
  // space out items
  // Note: this weird logic probably only works with 1 or 2 items, and almost for sure only on a 320x240 screen
  int pixelsPerItem = getDashboardAreaWidth() / numItems;
  int nextX = (numItems-1)*20 + (pixelsPerItem/(numItems+1)) - 40;//(numItems * 40); // symbol just to left of line

  changeFont(FontTiny);
  for (int itemNum = 0; itemNum < numItems; itemNum++) {
    
    #if defined(DISPLAY_TYPE_ADAFRUIT_35)
    showText(item + (itemNum * CHANNEL_DISPLAY_SIZE), getDashboardAreaX() + nextX, getDashboardAreaY() + 12, TextSmall, itemColor[itemNum]);
    #else
    showText(item + (itemNum * CHANNEL_DISPLAY_SIZE), getDashboardAreaX() + nextX, getDashboardAreaY() - (getTextUpperVerticalOffset(TextSmall) - 1), TextSmall, itemColor[itemNum]);
    #endif
    
    nextX += pixelsPerItem;
  }
  changeFont(FontNormal);

}

void Display::showLatestCacheUsed () {
  showCacheUsed(currentCacheUsed, true);
}

void Display::showCacheUsed (float percent, bool forceRepaint) {
  if (percent != currentCacheUsed || forceRepaint) {
    // draw outer rectangle
    drawRect(getCacheStatusX(), getCacheStatusY(), getCacheStatusWidth(), getCacheStatusHeight(), DarkBlue);

    if (percent < currentCacheUsed) {
      // clear out the inner rectangle
      fillRect(getCacheStatusX() + 1, getCacheStatusY() + 1, (getCacheStatusWidth() -2), getCacheStatusHeight() - 2, Black);
    }

    if (currentCacheUsed < 20 || percent < currentCacheUsed) {
      if (percent < 20) {
        changeFont(FontPico);
        showText("Mesh Cache", getCacheStatusX() + 13, getCacheStatusY() + getTextLowerVerticalOffset(TextSmall) + 1, TextSmall, Beige);
        changeFont(FontNormal);
      }
    }

    currentCacheUsed = percent;

    // draw the inner rectangle
    if (percent > 0) {
      fillRect(getCacheStatusX() + 1, getCacheStatusY() + 1, (getCacheStatusWidth() -2) * (max(percent, 0.05)), getCacheStatusHeight() - 2, Beige);
    }
  }
}

void Display::showLatestBatteryLevel () {
  showBatteryLevel(currentBatteryLevel, true);
}

void Display::showBatteryLevel (uint8_t percent, bool forceRepaint) {
  if (percent != currentBatteryLevel || forceRepaint) {
    // draw outer rectangles
    drawRect(getBatteryStatusX()+3, getBatteryStatusY(), getBatteryStatusWidth()-3, getBatteryStatusHeight(), DarkBlue);
    drawRect(getBatteryStatusX(), getBatteryStatusY() + getBatteryStatusHeight() * .3, 3, getBatteryStatusHeight() * .4, DarkBlue);

    DisplayColor batteryColor = Red;
    if (percent >= 80) {
      batteryColor = Green;
    }
    else if (percent >= 50) {
      batteryColor = Yellow;
    }
    fillRect(getBatteryStatusX() + 4, getBatteryStatusY() + 1, getBatteryStatusWidth() - 5, getBatteryStatusHeight() - 2, batteryColor);

    //changeFont(FontPico);
    //showText("Batt", getBatteryStatusX() + 7, getBatteryStatusY() + getTextLowerVerticalOffset(TextSmall) + 1, TextSmall, Yellow);
    //changeFont(FontNormal);

    currentBatteryLevel = percent;
  }
}


void Display::showProgress(float percent) {
  if (percent != currentProgress) {

    if (currentProgress == 0 || percent < currentProgress) {
      // draw draw outer circle and empty inner
      drawCircle(getSpinnerX(), getSpinnerY(), getSpinnerRadius(), White);
      fillCircle(getSpinnerX(), getSpinnerY(), getSpinnerRadius()-1, Black);
    }

    if (percent > currentProgress) {
      fillCircle (getSpinnerX(), getSpinnerY(), getSpinnerRadius() * percent, White);
    }

    currentProgress = percent;
  }
}

void Display::showProgressBar(float percent) {
  if (percent != currentProgress) {
    if (currentProgress == 0 || percent < currentProgress) {

      // draw outer rectangle
      fillRect(getProgressBarX(), getProgressBarY(), getProgressBarWidth(), getProgressBarHeight(), DarkBlue);
    }

    // draw the inner rectangle
    fillRect(getProgressBarX() + 2, getProgressBarY() + 2, getProgressBarWidth() * currentProgress, getProgressBarHeight() - 4, Beige);
    currentProgress = percent;
  }
}

void Display::showMainScrolls (bool _scrollUpEnabled, bool _scrollDownEnabled) {
    scrollUpEnabled = _scrollUpEnabled;
    scrollDownEnabled = _scrollDownEnabled;

    int scrollUpX = getMainScrollUpX();
    int scrollUpY = getMainScrollUpY();
    int scrollDownX = getMainScrollDownX();
    int scrollDownY = getMainScrollDownY();

    uint8_t radius = getMainScrollRadius();

    if (scrollUpEnabled) {
        fillTriangle (scrollUpX - (radius+2), scrollUpY + (radius+1), scrollUpX + (radius+2), scrollUpY + (radius+1), scrollUpX, scrollUpY - (radius+2), Beige);
        fillTriangle (scrollUpX - radius, scrollUpY + radius, scrollUpX + radius, scrollUpY + radius, scrollUpX, scrollUpY - radius, DarkGreen);
    }
    else {
        fillTriangle (scrollUpX - (radius+2), scrollUpY + (radius+1), scrollUpX + (radius+2), scrollUpY + (radius+1), scrollUpX, scrollUpY - (radius+2), Black);
    }

    if (scrollDownEnabled) {
        fillTriangle (scrollDownX - (radius+2), scrollDownY - (radius+1), scrollDownX + (radius+2), scrollDownY - (radius+1), scrollDownX, scrollDownY + (radius+2), Beige);
        fillTriangle (scrollDownX - radius, scrollDownY - radius, scrollDownX + radius, scrollDownY - radius, scrollDownX, scrollDownY + radius, DarkGreen);
    }
    else {
        fillTriangle (scrollDownX - (radius+2), scrollDownY - (radius+1), scrollDownX + (radius+2), scrollDownY - (radius+1), scrollDownX, scrollDownY + (radius+2), Black);
    }
}

ScrollButton Display::getScrollButtonAt (int x, int y) {
    int scrollUpX = getMainScrollUpX();
    int scrollUpY = getMainScrollUpY();
    int scrollDownX = getMainScrollDownX();
    int scrollDownY = getMainScrollDownY();
    uint8_t radius = getMainScrollTouchRadius();
    uint8_t tolerance = 10;

    if (x >= scrollUpX - (radius + tolerance) && x <= scrollUpX + (radius + tolerance) && y >= scrollUpY - (radius + tolerance) && y <= scrollUpY + (radius + tolerance)) {
        return ScrollUp;
    }
    else if (x >= scrollDownX - (radius + tolerance) && x <= scrollDownX + (radius + tolerance) && y >= scrollDownY - (radius + tolerance) && y <= scrollDownY + (radius + tolerance)) {
        return ScrollDown;
    }

    return ScrollNone;
}