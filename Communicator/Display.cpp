#include <Arduino.h>
#include "Display.h"

void Display::logConsole (String msg) {
  if (LOG_ENABLED) {
    Serial.println(msg);
  }
}

void Display::showStatus (String text, DisplayColor color) {
  clearStatus();
  showText(text, getStatusX(), getStatusY(), TextSmall, color);
}

void Display::clearStatus () {
  clearArea(getStatusX(), getStatusY() - getTextUpperVerticalOffset(TextSmall), getStatusWidth(), getStatusHeight() - getTextLowerVerticalOffset(TextSmall));
}

void Display::showMessage (const char* message, DisplayColor color, uint8_t position) {
  showText(message, getMessageAreaX(), getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())), TextSmall, color);
}

void Display::showMessageAndTitle (const char* title, const char* text, const char* readableTS, bool received, DisplayColor titleColor, DisplayColor messageColor, uint8_t position) {
  // show a line above the message for separation
  if (position != 0) {
    int lineYPos = getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())) - (getTextUpperVerticalOffset(TextSmall) + 2);
    drawLine(getMessageAreaX(), lineYPos, getMessageAreaX() + getMessageAreaWidth(), lineYPos, Beige);
  }

  changeFont(FontTiny);
  showText(received ? ">>" : "<<", getMessageAreaX(), getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())) - getTextLowerVerticalOffset(TextSmall)*2, TextSmall, titleColor);

  changeFont(FontBold);
  showText(title, getMessageAreaX() + 15, getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())), TextSmall, titleColor);

  // add message timestamp
  changeFont(FontTiny);
  showText(readableTS, getMessageAreaX() + getMessageAreaWidth() - 65, getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())) - (getTextUpperVerticalOffset(TextSmall) - 2), TextSmall, titleColor);

  changeFont(FontNormal);
  showText(text, getMessageAreaX(), getMessageAreaY() + getMessageTitleHeight() + (position * (getMessageHeight() + getMessageTitleHeight())), TextSmall, messageColor);
}

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

uint8_t Display::getMessagePosition (int positionX, int positionY) {
  // look mainly at y position, as that's what identifies one position versus another

  // subtract message start
  int shiftedY = positionY - (getMessageAreaY() - getTextUpperVerticalOffset(TextSmall));

  if (shiftedY > 0) {
    // divide by message height to get the position
    uint8_t selected = shiftedY / (getMessageHeight() + getMessageTitleHeight());
    if (selected < getMaxDisplayableMessages()) {
      return selected;
    }
  }

  return DISPLAY_MESSAGE_POSITION_NULL;
}

void Display::showAlert (const char* alertText, AlertType alertType) {
    DisplayColor color = Green;
    if (alertType == AlertWarning) {
        color = Yellow;
    }
    else if (alertType == AlertError) {
        color = Red;
    }
    else if (alertType == AlertActivity) {
      color = Blue;
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
  clearArea(0, getDashboardAreaY() - getTextUpperVerticalOffset(TextSmall), getScreenWidth(), getDashboardAreaHeight(), DarkGray);
}

void Display::showTick () {
  // if the ticker is showing hide it
  if (tickerShowing) {
    fillCircle(getTickerX(), getTickerY(), getTickerSize(), Black);
  }
  else {
    fillCircle(getTickerX(), getTickerY(), getTickerSize(), DarkGreen);
  }

  // toggle the ticker flag
  tickerShowing = !tickerShowing;
}

void Display::showDashboardItems (const char* item[], DisplayColor itemColor[], uint8_t numItems) {
  // space out items
  int spacing = getScreenWidth() / (1 + numItems);
  int nextX = spacing - 35; // symbol just to left of line

  for (int itemNum = 0; itemNum < numItems; itemNum++) {
    showText(item[itemNum], nextX, getDashboardAreaY(), TextSmall, itemColor[itemNum]);
    nextX += spacing;
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