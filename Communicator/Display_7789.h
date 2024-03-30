#include <Adafruit_ST7789.h>
#include <Adafruit_GFX.h>
#include "Display.h"
#include "MenuEnabledDisplay.h"
#include "Globals.h"

#ifndef DISPLAY_7789_H
#define DISPLAY_7789_H

class Display_7789 : public MenuEnabledDisplay {
  public:
    Display_7789 (ThermalEncoder* _encoder);
    void showThermal (const float* frame, int resHeight, int resWidth, int xOffset, int yOffset);
    void showThermal (const uint8_t* frame, int resHeight, int resWidth, int xOffset, int yOffset);
    void showInterpolatedThermalRow (const float* interpolatedRow, int xOffset, int yOffset);
    void showText (String text, int x, int y, TextSize size);
    void showText (String text, int x, int y, TextSize size, DisplayColor color);
    void showText (const char* text, int x, int y, TextSize size, DisplayColor color);
    void showSymbol (int gfxChar, int x, int y);
    void showSymbol (int gfxChar, int x, int y, DisplayColor color);
    void clear ();
    void clearArea (int x, int y, int width, int height);
    void clearArea (int x, int y, int width, int height, DisplayColor color);
    void repaint ();
    void fillRect(int lineX, int lineY, int lineWidth, int lineHeight, DisplayColor color);
    void setRotation (ScreenRotation rotation);

    int getScreenWidth () { return DISPLAY_7789_WIDTH; }
    int getScreenHeight () { return DISPLAY_7789_HEIGHT; }

  protected:
    int getStatusX();
    int getStatusY();
    int getStatusWidth();
    int getStatusHeight();

    int getImageAreaX (bool isAlt);
    int getImageAreaY (bool isAlt);
    int getImageAreaWidth (bool isAlt);
    int getImageAreaHeight (bool isAlt);

    int getImageSubtitleX (bool isAlt);
    int getImageSubtitleY (bool isAlt);

    int getMessageAreaX () {return DISPLAY_7789_MSG_X; }
    int getMessageAreaY () {return DISPLAY_7789_MSG_Y; }
    int getMessageAreaWidth () {return DISPLAY_7789_MSG_WIDTH; }
    int getMessageAreaHeight () {return DISPLAY_7789_MSG_HEIGHT; }

    int getMenuAreaX () { return DISPLAY_7789_MENU_X; }
    int getMenuAreaY () { return DISPLAY_7789_MENU_Y; }
    int getMenuAreaWidth () { return DISPLAY_7789_MENU_WIDTH; }
    int getMenuAreaHeight () { return DISPLAY_7789_MENU_HEIGHT; }
    int getMenuLineHeight () { return DISPLAY_7789_MENU_LINE_HEIGHT; }
    int getMenuMaxItemLength () { return DISPLAY_7789_MENU_MAX_ITEM_LENGTH; }
    int getMenuItemIndent () { return DISPLAY_7789_MENU_INDENT; }

    int getTitleAreaX () { return DISPLAY_7789_TITLE_X; }
    int getTitleAreaY () { return DISPLAY_7789_TITLE_Y; }
    int getTitleAreaHeight () { return DISPLAY_7789_TITLE_HEIGHT; }
    int getTitleAreaWidth () { return DISPLAY_7789_TITLE_WIDTH; }

    int getSubtitleAreaX () { return DISPLAY_7789_SUBTITLE_X; }
    int getSubtitleAreaY () { return DISPLAY_7789_SUBTITLE_Y; }
    int getSubtitleAreaHeight () { return DISPLAY_7789_SUBTITLE_HEIGHT; }
    int getSubtitleAreaWidth () { return DISPLAY_7789_SUBTITLE_WIDTH; }

    int getDashboardAreaY () { return DISPLAY_7789_DASHBOARD_Y; }
    int getDashboardAreaHeight() { return DISPLAY_7789_DASHBOARD_HEIGHT; }

    int getSpinnerX () { return DISPLAY_7789_SPINNER_X; }
    int getSpinnerY () { return DISPLAY_7789_SPINNER_Y; }
    int getSpinnerRadius () { return DISPLAY_7789_SPINNER_RADIUS; }

    int getAlertAreaX() { return DISPLAY_7789_ALERT_X; }
    int getAlertAreaY() { return DISPLAY_7789_ALERT_Y; }
    int getAlertAreaHeight() { return DISPLAY_7789_ALERT_HEIGHT; }

    TextSize getTitleTextSize () { return TextMedium; }
    DisplayColor getTitleColor() { return White; }

    TextSize getSubtitleTextSize () { return TextSmall; }
    DisplayColor getSubtitleColor() { return Green; }

    void drawLine (int lineX, int lineY, int lineEndX, int lineEndY, DisplayColor color); 
    void drawCircle (int circleX, int circleY, int radius, DisplayColor color);
    void fillCircle (int circleX, int circleY, int radius, DisplayColor color);

  private:
    Adafruit_ST7789 display = Adafruit_ST7789(Display_7789_CS, Display_7789_DC, Display_7789_MOSI,Display_7789_SCLK, Display_7789_RST);
    uint16_t get7789Color(DisplayColor color);
    DisplayColor getTemperatureColor(uint8_t temperature);
    DisplayColor getTemperatureColor(float temperature);
};
#endif