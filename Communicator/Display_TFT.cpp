#include "Display_TFT.h"

Display_TFT::Display_TFT(ThermalEncoder* _encoder) {
  encoder = _encoder;

  display.begin();
  touch.begin(); 

  display.setFont(&FreeSans9pt7b);

  clear();
}

void Display_TFT::clear () {
  display.fillScreen(BLACK);
}

void Display_TFT::clearArea (int x, int y, int width, int height) {
  display.fillRect(x, y, width, height, BLACK);
}

void Display_TFT::clearArea (int x, int y, int width, int height, DisplayColor color) {
  display.fillRect(x, y, width, height, getTFTColor(color));
}


void Display_TFT::repaint () {
  //display.display();
}

void Display_TFT::showText (String text, int x, int y, TextSize size) {
  showText(text, x, y, size, White);
}
void Display_TFT::showSymbol (int gfxChar, int x, int y) {
  showSymbol (gfxChar, x, y, White);
}


void Display_TFT::showSymbol (int gfxChar, int x, int y, DisplayColor color) {
  display.setTextColor(getTFTColor(color));   // Draw white text
  display.setTextSize(TextSmall);
  display.setCursor(x,y);
  display.write(gfxChar);
}


bool Display_TFT::handleIfTouched () {
  if (touch.touched()) {
      TS_Point p = touch.getPoint();
      // ignore edges
      if (p.y <= 0 || p.y >= DISPLAY_TFT_HEIGHT || p.x <= 0 || p.x >= DISPLAY_TFT_WIDTH) {
        return false;
      }

      // x/y will be different depending on how screen is rotated
      if (defaultKeyboard->isShowing()) {
        if (keyboardOrientation == Landscape) {
          return listener->handleScreenTouched(DISPLAY_TFT_HEIGHT - p.y, p.x);
        }
        else {
          return listener->handleScreenTouched(DISPLAY_TFT_WIDTH - p.x, DISPLAY_TFT_HEIGHT - p.y);
        }
      }
  } 

  return false;
}

int Display_TFT::getModalInput (const char* title, int maxLength, CharacterFilter charFilter, char* buffer, char* defaultValue) {
  return getModalInput(title, maxLength, charFilter, buffer, defaultValue, defaultKeyboard);
}


int Display_TFT::getModalInput (const char* title, int maxLength, CharacterFilter charFilter, char* buffer) {
  return getModalInput(title, maxLength, charFilter, buffer, "", defaultKeyboard);
}

int Display_TFT::getModalInput (const char* title, int maxLength, CharacterFilter charFilter, char* buffer, char* defaultValue, Keyboard* keyboard) {

  // rotate the screen if configured
  if (keyboardOrientation != Portrait) {
    setRotation(keyboardOrientation);
  }

  // pop up the keyboard
  keyboard->showKeyboard(charFilter, maxLength, defaultValue);

  // pop up the input modal

  // clear title area
  clearArea(getModalInputX(),getModalTitleY() - getTextUpperVerticalOffset(TextMedium), getModalInputWidth(), getModalTitleHeight());
  drawLine (getModalInputX(), getModalTitleY() - getTextUpperVerticalOffset(TextMedium), getModalInputX() + getModalInputWidth(), getModalTitleY() - getTextUpperVerticalOffset(TextMedium), White);
  showText(title, calculateModalTitleX(title), getModalTitleY(), TextMedium);
  drawLine (getModalInputX(), getModalTitleY() + getTextLowerVerticalOffset(TextMedium), getModalInputX() + getModalInputWidth(), getModalTitleY() + getTextLowerVerticalOffset(TextMedium), White);

  // clear text area
  clearArea(getModalInputX(),getModalInputY() - getTextUpperVerticalOffset(TextSmall), getModalInputWidth(), getModalInputHeight());

  // get user input until the user completes/cancels
  while (!keyboard->userTerminatedInput() && !keyboard->userCompletedInput()) {
    handleIfTouched();
    if (keyboard->wasBufferEdited()) {
      keyboard->resetBufferEditFlag();
      clearArea(getModalInputX(),getModalInputY() - getTextUpperVerticalOffset(TextSmall), getModalInputWidth(), getModalInputHeight());
    }

    if (keyboard->getUserInputLength() > 0) {
      if (keyboard->getUserInputLength() > 0 && keyboard->getUserInput()[keyboard->getUserInputLength()-1] == ' ') {
        showText(String(keyboard->getUserInput(), keyboard->getUserInputLength()-1) + "_", getModalInputX(), getModalInputY(), TextSmall, Green);
      }
      else {
        showText(String(keyboard->getUserInput(), keyboard->getUserInputLength()), getModalInputX(), getModalInputY(), TextSmall, Green);
      }
    }
  }
  keyboard->hideKeyboard();

  // clear text area
  clearArea(getModalInputX(),getModalInputY() - getTextUpperVerticalOffset(TextSmall), getModalInputWidth(), getModalInputHeight());

  // rotate the screen back if necessary
  if (keyboardOrientation != Portrait) {
    setRotation(Portrait);
  }

  int finalInputLength = keyboard->getUserInputLength();
  if (keyboard->userCompletedInput() && finalInputLength > 0) {
    memcpy(buffer, keyboard->getUserInput(), finalInputLength);
    return finalInputLength;
  }

  return 0;
}

void Display_TFT::showText (String text, int x, int y, TextSize size, DisplayColor color) {
  display.setTextSize(size);                
  display.setTextColor(getTFTColor(color));
  display.setCursor(x,y);
  display.println(text);
}

void Display_TFT::showText (const char* text, int x, int y, TextSize size, DisplayColor color) {

  display.setTextSize(size);                
  display.setTextColor(getTFTColor(color));
  display.setCursor(x,y);
  display.println(text);
}

void Display_TFT::fillRect(int lineX, int lineY, int lineWidth, int lineHeight, DisplayColor color) {
  display.fillRect(lineX, lineY, lineWidth, lineHeight, getTFTColor(color));
}

void Display_TFT::drawLine (int lineX, int lineY, int lineEndX, int lineEndY, DisplayColor color) {
  display.drawLine(lineX, lineY, lineEndX, lineEndY, getTFTColor(color));
}

void Display_TFT::drawCircle (int circleX, int circleY, int radius, DisplayColor color) {
  display.drawCircle(circleX, circleY, radius, getTFTColor(color));
}

void Display_TFT::fillCircle (int circleX, int circleY, int radius, DisplayColor color) {
  display.fillCircle(circleX, circleY, radius, getTFTColor(color));
}

void Display_TFT::setRotation (ScreenRotation _rotation) {
  rotation = _rotation;
  display.setRotation((uint8_t)_rotation);
}

void Display_TFT::showInterpolatedThermalRow (const float* interpolatedRow, int xOffset, int yOffset) {
  for (int w = 0; w < encoder->getInterpolatedResolutionWidth(); w++) {
      float t = interpolatedRow[w];
      display.drawPixel(xOffset + w, yOffset, getTFTColor(getTemperatureColor(t)));
  }
}

void Display_TFT::showThermal (const float* frame, int resHeight, int resWidth, int xOffset, int yOffset) {
  for (uint8_t h=0; h<resHeight; h++) {
    for (uint8_t w=0; w<resWidth; w++) {
      float t = frame[h*resWidth + w];
      display.drawPixel(xOffset + w, yOffset + h, getTFTColor(getTemperatureColor(t)));
    }
  }
}

void Display_TFT::showThermal (const uint8_t* frame, int resHeight, int resWidth, int xOffset, int yOffset) {
  for (uint8_t h=0; h<resHeight; h++) {
    for (uint8_t w=0; w<resWidth; w++) {
      uint8_t t = frame[h*resWidth + w];
      display.drawPixel(xOffset + w, yOffset + h, getTFTColor(getTemperatureColor(t)));
    }
  }
}

DisplayColor Display_TFT::getTemperatureColor(float temperature) {
  uint8_t temp = round(temperature);
  return getTemperatureColor(temp);
}

DisplayColor Display_TFT::getTemperatureColor(uint8_t temperature) {
  if (temperature < 20) {
    return Black;
  }
  else if (temperature < 23) {
    return Blue;
  }
  else if (temperature < 25) {
    return Green;
  }
  else if (temperature < 27) {
    return Cyan;
  }
  else if (temperature < 29) {
    return Magenta;
  }
  else if (temperature < 31) {
    return Yellow;
  }
  else if (temperature < 37) {
    return Red;
  }
  else {
    return White;
  }
}

uint16_t Display_TFT::getTFTColor(DisplayColor color) {
  switch (color) {
    case Black:
      return BLACK;
    case Blue:
      return BLUE;
    case Red:
      return RED;
    case Green:
      return GREEN;
    case Cyan:
      return CYAN;
    case Magenta:
      return MAGENTA;
    case Yellow:
      return YELLOW;
    case Gray:
      return GRAY;
    default:
      return WHITE;
  }
}

int Display_TFT::getStatusX() {
  return DISPLAY_TFT_STATUS_X;
}

int Display_TFT::getStatusY() {
  return DISPLAY_TFT_STATUS_Y;
}

int Display_TFT::getStatusWidth() {
  return DISPLAY_TFT_STATUS_WIDTH;
}

int Display_TFT::getStatusHeight() {
  return DISPLAY_TFT_STATUS_HEIGHT;
}

int Display_TFT::getImageAreaX (bool isAlt) {
  if (isAlt)
    return DISPLAY_TFT_ALT_IMAGE_X;
  return DISPLAY_TFT_MAIN_IMAGE_X;
}

int Display_TFT::getImageAreaY (bool isAlt) {
  if (isAlt)
    return DISPLAY_TFT_ALT_IMAGE_Y;
  return DISPLAY_TFT_MAIN_IMAGE_Y;
}

int Display_TFT::getImageAreaWidth (bool isAlt) {
  if (isAlt)
    return DISPLAY_TFT_ALT_IMAGE_WIDTH;
  return DISPLAY_TFT_MAIN_IMAGE_WIDTH;
}

int Display_TFT::getImageAreaHeight (bool isAlt) {
  if (isAlt)
    return DISPLAY_TFT_ALT_IMAGE_HEIGHT;
  return DISPLAY_TFT_MAIN_IMAGE_HEIGHT;
}

int Display_TFT::getImageSubtitleX (bool isAlt) {
  if (isAlt)
    return DISPLAY_TFT_ALT_IMAGE_SUBTITLE_X;
  return DISPLAY_TFT_MAIN_IMAGE_SUBTITLE_X;
}

int Display_TFT::getImageSubtitleY (bool isAlt) {
  if (isAlt)
    return DISPLAY_TFT_ALT_IMAGE_SUBTITLE_Y;
  return DISPLAY_TFT_MAIN_IMAGE_SUBTITLE_Y;
}

int Display_TFT::calculateModalTitleX (const char* titleText) {
  // bump it right until it's roughly centered
  uint8_t charsPerRow = 20;

  // roughly 20 chars wide in landscape
  if (rotation == Landscape) {
    return DISPLAY_TFT_LS_MODAL_TITLE_X + max(0,((charsPerRow - strlen(titleText)) / 2) * (getScreenWidth()/charsPerRow));
  }

  // portrait mode, keep hardcoded value
  return DISPLAY_TFT_MODAL_TITLE_X;
}
