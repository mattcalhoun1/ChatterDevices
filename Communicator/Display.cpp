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
  showText(message, getMessageAreaX(), getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())), TextSmall, color);
}

void Display::showMessageAndTitle (const char* title, const char* text, const char* readableTS, bool received, char status, char sendMethod, DisplayColor titleColor, DisplayColor messageColor, uint8_t position) {
  // show a line above the message for separation
  if (position != 0) {
    int lineYPos = getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())) - (getTextUpperVerticalOffset(TextSmall) + 2);
    drawLine(getMessageAreaX(), lineYPos, getMessageAreaX() + getMessageAreaWidth(), lineYPos, Beige);
  }

  changeFont(FontTiny);
  showText(
    received ? "<<" : ">>", 
    getMessageAreaX(), 
    getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())) - getTextLowerVerticalOffset(TextSmall)*2,
    TextSmall,
    received ? titleColor : LightGray);

  changeFont(received ? FontBold : FontNormal);
  showText(
    title, 
    getMessageAreaX() + 15, 
    getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())), 
    TextSmall, 
    received ? titleColor : LightGray
  );

  // add message timestamp
  changeFont(FontTiny);
  showText(
    readableTS, 
    getMessageAreaX() + getMessageAreaWidth() - 65, 
    getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())) - (getTextUpperVerticalOffset(TextSmall) - 2), 
    TextSmall, 
    received ? titleColor : LightGray);
  
  // if it was a direct message, add an ack indicator
  if (sendMethod == 'D') {
    if (status == 'A') {
      // checkmark
      drawLine(
        getMessageAreaX() + getMessageAreaWidth() - 72,
        getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())) - getTextUpperVerticalOffset(TextSmall) + 6,
        getMessageAreaX() + getMessageAreaWidth() - 71,
        getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())) - getTextUpperVerticalOffset(TextSmall) + 8,
        received ? titleColor : LightGray
      );

      drawLine(
        getMessageAreaX() + getMessageAreaWidth() - 71,
        getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())) - getTextUpperVerticalOffset(TextSmall) + 8,
        getMessageAreaX() + getMessageAreaWidth() - 68,
        getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())) - getTextUpperVerticalOffset(TextSmall) + 3,
        received ? titleColor : LightGray
      );
    }
  }
  else if (sendMethod == 'I') { // if sent via intermediary (mesh)
    // if it was accepted, draw a circle
    if (status == 'M') {
      drawCircle(
        getMessageAreaX() + getMessageAreaWidth() - 71,
        getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())) - getTextUpperVerticalOffset(TextSmall) + 5,
        3,
        received ? LightGray : BrightGreen
      );
    }
    else if (status == 'N') {
      // not yet accepted, just a dot      
      fillCircle(
        getMessageAreaX() + getMessageAreaWidth() - 71,
        getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())) - getTextUpperVerticalOffset(TextSmall) + 5,
        1,
        received ? LightGray : BrightGreen
      );
    }
    else if (status == 'A') { // it was acknowledged, cricle with dot
      drawCircle(
        getMessageAreaX() + getMessageAreaWidth() - 71,
        getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())) - getTextUpperVerticalOffset(TextSmall) + 5,
        3,
        received ? LightGray : BrightGreen
      );
      fillCircle(
        getMessageAreaX() + getMessageAreaWidth() - 71,
        getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())) - getTextUpperVerticalOffset(TextSmall) + 5,
        2,
        received ? LightGray : BrightGreen
      );
    }
  }
  

  changeFont(FontNormal);
  showText(
    text,
    getMessageAreaX(), 
    getMessageAreaY() + getMessageTitleHeight() + (position * (getMessageHeight() + getMessageTitleHeight())), 
    TextSmall, 
    received ? messageColor : DarkGray);
}
/*
void Display::showHasMessagesBefore () {
  for (uint8_t pxWide = 0; pxWide < 4; pxWide++) {
    showText("<", pxWide + getMessageAreaX() + getMessageAreaWidth() - 40, getMessageAreaY() + getMessageAreaHeight() - getTextUpperVerticalOffset(TextSmall), TextSmall, Cyan);
  }
}

void Display::showHasMessagesAfter () {
  for (uint8_t pxWide = 0; pxWide < 4; pxWide++) {
    showText(">", pxWide + getMessageAreaX() + getMessageAreaWidth() - 20, getMessageAreaY() + getMessageAreaHeight() - getTextUpperVerticalOffset(TextSmall), TextSmall, Cyan);
  }
}
*/
uint8_t Display::getMessagePosition (int positionX, int positionY) {
  // the touch must be toward the title area (left)
  if (positionX < getScreenWidth() / 3) {
    // look mainly at y position, as that's what identifies one position versus another

    // subtract message start
    int shiftedY = positionY - (getMessageAreaY() - getTextUpperVerticalOffset(TextSmall));

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
    showText(alertText, getAlertAreaX(), getAlertAreaY(), TextMedium, color);
}

void Display::clearMessageArea () {
  clearArea(getMessageAreaX(), getMessageAreaY() - getTextUpperVerticalOffset(TextSmall), getMessageAreaWidth(), getMessageAreaHeight() + 1);
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
  fillRect(1, 1, getScreenWidth() - 2, getTitleAreaHeight(), DarkGray);
  showText(text, calculateTitleX(text), getTitleAreaY(), getTitleTextSize(), getTitleColor());

  // line at base of title/subtitle area
  //fillRect(1, getTitleAreaHeight() + getSubtitleAreaHeight(), getScreenWidth() - 2, 2, Beige);
}

void Display::showSubtitle (const char* text) {
  // change to smaller font
  clearArea(getSubtitleAreaX(), getSubtitleAreaY() - getTextUpperVerticalOffset(TextSmall), getSubtitleAreaWidth(), getSubtitleAreaHeight() - (getTextLowerVerticalOffset(TextSmall)+1));
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

void Display::showTick () {
  // if the ticker is showing hide it
  if (tickerShowing) {
    fillCircle(getTickerX(), getTickerY(), getTickerSize(), LightBlue);
  }
  else {
    fillCircle(getTickerX(), getTickerY(), getTickerSize(), DarkGreen);
  }

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
    showText(item + (itemNum * CHANNEL_DISPLAY_SIZE), getDashboardAreaX() + nextX, getDashboardAreaY() - (getTextUpperVerticalOffset(TextSmall) - 1), TextSmall, itemColor[itemNum]);
    nextX += pixelsPerItem;
  }
  changeFont(FontNormal);

}

void Display::showLatestCacheUsed () {
  showCacheUsed(currentCacheUsed, true);
}

void Display::showCacheUsed (float percent, bool forceRepaint) {
  if (percent != currentCacheUsed || forceRepaint) {
    if (currentCacheUsed == 0 || percent < currentCacheUsed) {

      // draw outer rectangle
      drawRect(getCacheStatusX(), getCacheStatusY(), getCacheStatusWidth(), getCacheStatusHeight(), DarkBlue);

      if (percent < 20) {
        changeFont(FontPico);
        showText("Mesh Cache", getCacheStatusX() + 20, getCacheStatusY() + getTextLowerVerticalOffset(TextSmall) + 1, TextSmall, Beige);
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