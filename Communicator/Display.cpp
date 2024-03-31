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

void Display::showMessageAndTitle (const char* title, const char* text, DisplayColor titleColor, DisplayColor messageColor, uint8_t position) {
  showText(title, getMessageAreaX(), getMessageAreaY() + (position * (getMessageHeight() + getMessageTitleHeight())), TextSmall, titleColor);
  showText(text, getMessageAreaX(), getMessageAreaY() + getMessageTitleHeight() + (position * (getMessageHeight() + getMessageTitleHeight())), TextSmall, messageColor);
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
  clearArea(getMessageAreaX(), getMessageAreaY() - getTextUpperVerticalOffset(TextSmall), getMessageAreaWidth(), getMessageAreaHeight() - getTextLowerVerticalOffset(TextSmall));
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
  showText(text, getTitleAreaX(), getTitleAreaY(), getTitleTextSize(), getTitleColor());
}

void Display::showSubtitle (const char* text) {
  clearArea(getSubtitleAreaX(), getSubtitleAreaY() - getTextUpperVerticalOffset(TextSmall), getSubtitleAreaWidth(), getSubtitleAreaHeight() - getTextLowerVerticalOffset(TextSmall));
  showText(text, getSubtitleAreaX(), getSubtitleAreaY(), getSubtitleTextSize(), getSubtitleColor());
}

void Display::clearAll () {
  clearArea(0, 0, getScreenWidth(), getScreenHeight());
}

void Display::clearDashboard () {
  clearArea(0, getDashboardAreaY() - getTextUpperVerticalOffset(TextSmall), getScreenWidth(), getDashboardAreaHeight(), Gray);
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