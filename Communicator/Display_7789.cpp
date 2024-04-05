#include "Display_7789.h"

Display_7789::Display_7789(ThermalEncoder* _encoder) {
  encoder = _encoder;
  // Option 1 using SPI
  //Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

  // OPTION 2 lets you interface the display using ANY TWO or THREE PINS,
  // tradeoff being that performance is not as fast as hardware SPI above.
  //display = new Adafruit_ST7789(Display_7789_CS, Display_7789_DC, Display_7789_MOSI,Display_7789_SCLK, Display_7789_RST);

  // OR use this initializer (uncomment) if using a 1.47" 172x320 TFT:
  display.init(170, 320); // Init ST7789 172x320

  // initialize and clear display
  //display.begin();
  clear();
}

void Display_7789::clear () {
  display.fillScreen(ST77XX_BLACK);
}

void Display_7789::clearArea (int x, int y, int width, int height) {
  display.fillRect(x, y, width, height, ST77XX_BLACK);
}

void Display_7789::clearArea (int x, int y, int width, int height, DisplayColor color) {
  display.fillRect(x, y, width, height, get7789Color(color));
}


void Display_7789::repaint () {
  //display.display();
}

void Display_7789::showText (String text, int x, int y, TextSize size) {
  showText(text, x, y, size, White);
}
void Display_7789::showSymbol (int gfxChar, int x, int y) {
  showSymbol (gfxChar, x, y, White);
}


void Display_7789::showSymbol (int gfxChar, int x, int y, DisplayColor color) {
  display.setTextColor(get7789Color(color));   // Draw white text
  display.setTextSize(TextMedium);
  display.setCursor(x,y);
  display.write(gfxChar);
}


void Display_7789::showText (String text, int x, int y, TextSize size, DisplayColor color) {
  display.setTextSize(size);                
  display.setTextColor(get7789Color(color));
  display.setCursor(x,y);
  display.println(text);
}

void Display_7789::showText (const char* text, int x, int y, TextSize size, DisplayColor color) {
  display.setTextSize(size);                
  display.setTextColor(get7789Color(color));
  display.setCursor(x,y);
  display.println(text);
}

void Display_7789::fillRect(int lineX, int lineY, int lineWidth, int lineHeight, DisplayColor color) {
  display.fillRect(lineX, lineY, lineWidth, lineHeight, get7789Color(color));
}

void Display_7789::drawLine (int lineX, int lineY, int lineEndX, int lineEndY, DisplayColor color) {
  display.drawLine(lineX, lineY, lineEndX, lineEndY, get7789Color(color));
}

void Display_7789::drawCircle (int circleX, int circleY, int radius, DisplayColor color) {
  display.drawCircle(circleX, circleY, radius, get7789Color(color));
}

void Display_7789::fillCircle (int circleX, int circleY, int radius, DisplayColor color) {
  display.fillCircle(circleX, circleY, radius, get7789Color(color));
}

void Display_7789::setRotation (ScreenRotation rotation) {
  display.setRotation((uint8_t)rotation);
}

void Display_7789::showInterpolatedThermalRow (const float* interpolatedRow, int xOffset, int yOffset) {
  for (int w = 0; w < encoder->getInterpolatedResolutionWidth(); w++) {
      float t = interpolatedRow[w];
      display.drawPixel(xOffset + w, yOffset, get7789Color(getTemperatureColor(t)));
  }
}

void Display_7789::showThermal (const float* frame, int resHeight, int resWidth, int xOffset, int yOffset) {
  for (uint8_t h=0; h<resHeight; h++) {
    for (uint8_t w=0; w<resWidth; w++) {
      float t = frame[h*resWidth + w];
      display.drawPixel(xOffset + w, yOffset + h, get7789Color(getTemperatureColor(t)));
    }
  }
}

void Display_7789::showThermal (const uint8_t* frame, int resHeight, int resWidth, int xOffset, int yOffset) {
  for (uint8_t h=0; h<resHeight; h++) {
    for (uint8_t w=0; w<resWidth; w++) {
      uint8_t t = frame[h*resWidth + w];
      display.drawPixel(xOffset + w, yOffset + h, get7789Color(getTemperatureColor(t)));
    }
  }
}

DisplayColor Display_7789::getTemperatureColor(float temperature) {
  uint8_t temp = round(temperature);
  return getTemperatureColor(temp);
}

DisplayColor Display_7789::getTemperatureColor(uint8_t temperature) {
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

uint16_t Display_7789::get7789Color(DisplayColor color) {
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
    case BrightGreen:
      return BRIGHTGREEN;
    case LightGray:
      return LIGHTGREY;
    case DarkRed:
      return DARKRED;
    case DarkGray:
      return DARKGRAY;
    default:
      return WHITE;
  }
}

int Display_7789::getStatusX() {
  return DISPLAY_7789_STATUS_X;
}

int Display_7789::getStatusY() {
  return DISPLAY_7789_STATUS_Y;
}

int Display_7789::getStatusWidth() {
  return DISPLAY_7789_STATUS_WIDTH;
}

int Display_7789::getStatusHeight() {
  return DISPLAY_7789_STATUS_HEIGHT;
}

int Display_7789::getImageAreaX (bool isAlt) {
  if (isAlt)
    return DISPLAY_7789_ALT_IMAGE_X;
  return DISPLAY_7789_MAIN_IMAGE_X;
}

int Display_7789::getImageAreaY (bool isAlt) {
  if (isAlt)
    return DISPLAY_7789_ALT_IMAGE_Y;
  return DISPLAY_7789_MAIN_IMAGE_Y;
}

int Display_7789::getImageAreaWidth (bool isAlt) {
  if (isAlt)
    return DISPLAY_7789_ALT_IMAGE_WIDTH;
  return DISPLAY_7789_MAIN_IMAGE_WIDTH;
}

int Display_7789::getImageAreaHeight (bool isAlt) {
  if (isAlt)
    return DISPLAY_7789_ALT_IMAGE_HEIGHT;
  return DISPLAY_7789_MAIN_IMAGE_HEIGHT;
}

int Display_7789::getImageSubtitleX (bool isAlt) {
  if (isAlt)
    return DISPLAY_7789_ALT_IMAGE_SUBTITLE_X;
  return DISPLAY_7789_MAIN_IMAGE_SUBTITLE_X;
}

int Display_7789::getImageSubtitleY (bool isAlt) {
  if (isAlt)
    return DISPLAY_7789_ALT_IMAGE_SUBTITLE_Y;
  return DISPLAY_7789_MAIN_IMAGE_SUBTITLE_Y;
}

