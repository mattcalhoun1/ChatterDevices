#include "Display_TFT.h"

Display_TFT::Display_TFT(ThermalEncoder* _encoder) {
  encoder = _encoder;

  display.begin();

  #if defined(TOUCH_CONTROL_RAK)
  touch = new TouchControlRak();
  touch->init(); 
  #elif defined(TOUCH_CONTROL_ADAFRUIT)
  touch = new TouchControlAdafruit();
  touch->init(); 
  #else
  touch = new TouchControlNone();
  logConsole("No touch control defined");
  #endif

  display.setFont(&FreeSans9pt7b);

  clear();
}

void Display_TFT::touchInterrupt() {
  touch->touchInterrupt();
}

void Display_TFT::changeFont (FontType fontType) {
  if (fontType != currFontType) {
    switch (fontType) {
      case FontItalic:
        currFontType = FontItalic;
        display.setFont(&FreeSansOblique9pt7b);
        break;
      case FontBold:
        currFontType = FontBold;
        display.setFont(&FreeSansBold9pt7b);
        break;
      case FontTiny:
        currFontType = FontTiny;
        display.setFont(); // default fixed size
        break;
      default:
        currFontType = FontNormal;
        display.setFont(&FreeSans9pt7b);
    }
  }
}

void Display_TFT::clear () {
  display.fillScreen(ILI9341_BLACK);
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
  bool success = true;
  int transposedX;
  int transposedY;

  if (touch->wasTouched(transposedX, transposedY, defaultKeyboard->isShowing(), keyboardOrientation)) {
      // notify all listeners
      for (uint8_t l = 0; l < numListeners; l++) {
        success = listeners[l]->handleScreenTouched(transposedX, transposedY) && success;
      }
  } 

  return success;
}

int Display_TFT::getModalInput (const char* title, int maxLength, CharacterFilter charFilter, char* buffer, const char* defaultValue) {
  return getModalInput(title, maxLength, charFilter, buffer, defaultValue, 0, defaultKeyboard);
}


int Display_TFT::getModalInput (const char* title, int maxLength, CharacterFilter charFilter, char* buffer) {
  return getModalInput(title, maxLength, charFilter, buffer, "", 0, defaultKeyboard);
}

int Display_TFT::getModalInput (const char* title, int maxLength, CharacterFilter charFilter, char* buffer, const char* defaultValue, int timeoutMillis) {
  return getModalInput(title, maxLength, charFilter, buffer, "", timeoutMillis, defaultKeyboard);
}

int Display_TFT::getModalInput (const char* title, int maxLength, CharacterFilter charFilter, char* buffer, const char* defaultValue, int timeoutMillis, Keyboard* keyboard) {
  lastModalActivity = millis();
  clearAll();

  // rotate the screen if configured
  if (keyboardOrientation != Portrait) {
    setRotation(keyboardOrientation);
  }

  // pop up the keyboard
  keyboard->showKeyboard(charFilter, maxLength, defaultValue);

  // pop up the input modal

  // clear title area
  clearArea(getModalInputX(),getModalTitleY() - getTextUpperVerticalOffset(TextMedium), getModalInputWidth(), getModalTitleHeight());
  drawLine (getModalInputX(), getModalTitleY() - getTextUpperVerticalOffset(TextMedium), getModalInputX() + getModalInputWidth(), getModalTitleY() - getTextUpperVerticalOffset(TextMedium), LightGray);
  showText(title, calculateModalTitleX(title), getModalTitleY(), TextMedium, Beige);
  drawLine (getModalInputX(), getModalTitleY() + getTextLowerVerticalOffset(TextMedium), getModalInputX() + getModalInputWidth(), getModalTitleY() + getTextLowerVerticalOffset(TextMedium), LightGray);

  // clear text area
  clearArea(getModalInputX(),getModalInputY() - getTextUpperVerticalOffset(TextSmall), getModalInputWidth(), getModalInputHeight());

  // get user input until the user completes/cancels
  while (!keyboard->userTerminatedInput() && !keyboard->userCompletedInput() && (timeoutMillis == 0 || millis() - lastModalActivity < timeoutMillis)) {
    handleIfTouched();

    if (keyboard->checkAndResetActivityFlag()) {
      lastModalActivity = millis();
    }

    if (keyboard->wasBufferEdited()) {
      keyboard->resetBufferEditFlag();
      clearArea(getModalInputX(),getModalInputY() - getTextUpperVerticalOffset(TextSmall), getModalInputWidth(), getModalInputHeight());
    }

    if (keyboard->getUserInputLength() > 0) {
      //if (keyboard->getUserInputLength() > 0 && keyboard->getUserInput()[keyboard->getUserInputLength()-1] == ' ') {
      //  showText(String(keyboard->getUserInput(), keyboard->getUserInputLength()-1) + "_", getModalInputX(), getModalInputY(), TextSmall, Green);
      //}
      //else {
        // set the null indicator
        keyboard->addTermCharacter();
        showText(keyboard->getUserInput(), getModalInputX(), getModalInputY(), TextSmall, LightBlue);
      //}
    }
  }
  keyboard->hideKeyboard();

  // clear the touch interrupt
  touch->clearTouchInterrupt();

  // clear text area
  clearArea(getModalInputX(),getModalInputY() - getTextUpperVerticalOffset(TextSmall), getModalInputWidth(), getModalInputHeight());

  // rotate the screen back if necessary
  if (keyboardOrientation != Portrait) {
    setRotation(Portrait);
  }

  int finalInputLength = keyboard->getUserInputLength();
  if (keyboard->userCompletedInput() && finalInputLength > 0) {
    memcpy(buffer, keyboard->getUserInput(), finalInputLength);
    clearAll();
    return finalInputLength;
  }

  clearAll();
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

void Display_TFT::fillTriangle (int x1, int y1, int x2, int y2, int x3, int y3, DisplayColor color) {
  display.fillTriangle(x1, y1, x2, y2, x3, y3, getTFTColor(color));
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
      return ILI9341_BLACK;
    case Blue:
      return ILI9341_BLUE;
    case Red:
      return ILI9341_RED;
    case Green:
      return ILI9341_GREEN;
    case Cyan:
      return ILI9341_CYAN;
    case Magenta:
      return ILI9341_MAGENTA;
    case Yellow:
      return ILI9341_YELLOW;
    case Gray:
      return LIGHTGREY;
    case DarkBlue:
      return DARKBLUE;
    case LightBlue:
      return LIGHTBLUE;
    case Beige:
      return BEIGE;
    case LightGreen:
      return LIGHTGREEN;
    case DarkGreen:
      return DARKGREEN;
    case LightGray:
      return LIGHTGREY;
    case DarkRed:
      return DARKRED;
    case DarkGray:
      return DARKGRAY;
    case BrightGreen:
      return BRIGHTGREEN;
    case BrightYellow:
      return BRIGHTYELLOW;
    default:
      return ILI9341_WHITE;
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

int Display_TFT::calculateTitleX (const char* titleText) {
  // bump it right until it's roughly centered
  uint8_t charsPerRow = 30;

  // roughly 20 chars wide in landscape
  if (rotation == Landscape) {
    return DISPLAY_TFT_TITLE_X + max(0,((charsPerRow - strlen(titleText)) / 2) * (getScreenHeight()/charsPerRow));
  }
  else {
    return DISPLAY_TFT_TITLE_X + max(0,((charsPerRow - strlen(titleText)) / 2) * (getScreenWidth()/charsPerRow));
  }
}

int Display_TFT::calculateSubtitleX (const char* titleText) {
  // bump it right until it's roughly centered
  uint8_t charsPerRow = 38;

  // roughly 20 chars wide in landscape
  if (rotation == Landscape) {
    return DISPLAY_TFT_SUBTITLE_X + max(0,((charsPerRow - strlen(titleText)) / 2) * (getScreenHeight()/charsPerRow));
  }
  else {
    return DISPLAY_TFT_SUBTITLE_X + max(0,((charsPerRow - strlen(titleText)) / 2) * (getScreenWidth()/charsPerRow));
  }
}

int Display_TFT::calculateModalTitleX (const char* titleText) {
  // bump it right until it's roughly centered
  uint8_t charsPerRow = 20;

  // roughly 20 chars wide in landscape
  if (rotation == Landscape) { // is this right? should it be looking at screen height, not width?
    return DISPLAY_TFT_LS_MODAL_TITLE_X + max(0,((charsPerRow - strlen(titleText)) / 2) * (getScreenWidth()/charsPerRow));
  }

  // portrait mode, keep hardcoded value
  return DISPLAY_TFT_MODAL_TITLE_X;
}

void Display_TFT::showButton(uint8_t buttonPosition, const char* buttonText){
  // draw a round rectangle (filled) inside another rectangle
  int buttonX = getButtonAreaX() + (buttonPosition * getButtonHorizontalOffset());
  display.drawRoundRect(buttonX, getButtonAreaY(), getButtonWidth(), getButtonHeight(), 1, getTFTColor(LightGreen));
  display.fillRoundRect(buttonX + 1, getButtonAreaY() + 1, getButtonWidth() - 2, getButtonHeight() - 2, 1, getTFTColor(DarkGreen));

  uint8_t charsPerButton = 6;
  int textX = buttonX + (max(0, ((charsPerButton - strlen(buttonText))/2) * (getButtonWidth()/charsPerButton)));
  showText(buttonText, textX, getButtonAreaY() + getButtonHeight() - getTextLowerVerticalOffset(TextSmall), TextSmall, Beige);
}

void Display_TFT::showButtons() {
  for (uint8_t btnCount = 0; btnCount < NUM_DISPLAYED_BUTTONS; btnCount++){
    showButton(btnCount, getButtonText((DisplayedButton)btnCount));
  }
}

DisplayedButton Display_TFT::getButtonAt (int x, int y) {
  // is the button in the button area vertically
  if (y >= getButtonAreaY() && y <= getButtonAreaY() + getButtonHeight()) {
    // it's in the button row somewhere, is it on a button
    for (uint8_t btnCount = 0; btnCount < NUM_DISPLAYED_BUTTONS; btnCount++) {
      int buttonX = getButtonAreaX() + (btnCount * getButtonHorizontalOffset());
      if (x >= buttonX && x <= buttonX + getButtonWidth()) {
        return (DisplayedButton)btnCount;
      }
    }
  }

  return ButtonNone;
}