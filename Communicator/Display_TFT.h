#include "Display.h"
#include "MenuEnabledDisplay.h"
#include "TouchEnabledDisplay.h"
#include "FullyInteractiveDisplay.h"
#include "Globals.h"
#include "Keyboard.h"

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9341.h> // Hardware-specific library
#include <Adafruit_FT6206.h> // capacative touch
#include <Fonts/FreeSans9pt7b.h> // default font

#ifndef DISPLAY_TFT_H
#define DISPLAY_TFT_H

class Display_TFT : public FullyInteractiveDisplay {
  public:
    Display_TFT (ThermalEncoder* _encoder);
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
    void drawLine (int lineX, int lineY, int lineEndX, int lineEndY, DisplayColor color);
    void drawCircle (int circleX, int circleY, int radius, DisplayColor color);
    void fillCircle (int circleX, int circleY, int radius, DisplayColor color);

    bool handleIfTouched ();
    int getModalInput (const char* title, int maxLength, CharacterFilter charFilter, char* buffer);
    int getModalInput (const char* title, int maxLength, CharacterFilter charFilter, char* buffer, char* defaultValue);
    int getModalInput (const char* title, int maxLength, CharacterFilter charFilter, char* buffer, char* defaultValue, Keyboard* keyboard);

  protected:
    int getScreenWidth () { return DISPLAY_TFT_WIDTH; }
    int getScreenHeight () { return DISPLAY_TFT_HEIGHT; }

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

    int getMessageAreaX () {return DISPLAY_TFT_MSG_X; }
    int getMessageAreaY () {return DISPLAY_TFT_MSG_Y; }
    int getMessageAreaWidth () {return DISPLAY_TFT_MSG_WIDTH; }
    int getMessageAreaHeight () {return DISPLAY_TFT_MSG_HEIGHT; }

    int getMenuAreaX () { return DISPLAY_TFT_MENU_X; }
    int getMenuAreaY () { return DISPLAY_TFT_MENU_Y; }
    int getMenuAreaWidth () { return DISPLAY_TFT_MENU_WIDTH; }
    int getMenuAreaHeight () { return DISPLAY_TFT_MENU_HEIGHT; }
    int getMenuLineHeight () { return DISPLAY_TFT_MENU_LINE_HEIGHT; }
    int getMenuMaxItemLength () { return DISPLAY_TFT_MENU_MAX_ITEM_LENGTH; }
    int getMenuItemIndent () { return DISPLAY_TFT_MENU_INDENT; }

    int getTitleAreaX () { return DISPLAY_TFT_TITLE_X; }
    int getTitleAreaY () { return DISPLAY_TFT_TITLE_Y; }
    int getTitleAreaHeight () { return DISPLAY_TFT_TITLE_HEIGHT; }
    int getTitleAreaWidth () { return DISPLAY_TFT_TITLE_WIDTH; }

    int getSubtitleAreaX () { return DISPLAY_TFT_SUBTITLE_X; }
    int getSubtitleAreaY () { return DISPLAY_TFT_SUBTITLE_Y; }
    int getSubtitleAreaHeight () { return DISPLAY_TFT_SUBTITLE_HEIGHT; }
    int getSubtitleAreaWidth () { return DISPLAY_TFT_SUBTITLE_WIDTH; }

    int getDashboardAreaY () { return DISPLAY_TFT_DASHBOARD_Y; }
    int getDashboardAreaHeight() { return DISPLAY_TFT_DASHBOARD_HEIGHT; }

    int getKeyboardAreaX () { return DISPLAY_TFT_KEYBOARD_X; }
    int getKeyboardAreaY () { return DISPLAY_TFT_KEYBOARD_Y; }

    int getKeyboardAreaHeight () { return DISPLAY_TFT_KEYBOARD_HEIGHT; }
    int getKeyboardAreaWidth () { return DISPLAY_TFT_KEYBOARD_WIDTH; }

    TextSize getTitleTextSize () { return TextSmall; }
    DisplayColor getTitleColor() { return White; }

    TextSize getSubtitleTextSize () { return TextSmall; }
    DisplayColor getSubtitleColor() { return Green; }

    int getSpinnerX () { return DISPLAY_TFT_SPINNER_X; }
    int getSpinnerY () { return DISPLAY_TFT_SPINNER_Y; }
    int getSpinnerRadius () { return DISPLAY_TFT_SPINNER_RADIUS; }

    int getModalTitleX () { return DISPLAY_TFT_MODAL_TITLE_X; }
    int getModalTitleY () { return DISPLAY_TFT_MODAL_TITLE_Y; }
    int getModalTitleHeight () { return DISPLAY_TFT_MODAL_TITLE_HEIGHT; }
    int getModalInputX () { return DISPLAY_TFT_MODAL_INPUT_X; }
    int getModalInputY () { return DISPLAY_TFT_MODAL_INPUT_Y; }
    int getModalInputWidth () { return DISPLAY_TFT_MODAL_INPUT_WIDTH; }
    int getModalInputHeight () { return DISPLAY_TFT_MODAL_INPUT_HEIGHT; }

    int getAlertAreaX() { return DISPLAY_TFT_ALERT_X; }
    int getAlertAreaY() { return DISPLAY_TFT_ALERT_Y; }
    int getAlertAreaHeight() { return DISPLAY_TFT_ALERT_HEIGHT; }

  private:
  
    Adafruit_ILI9341 display = Adafruit_ILI9341(Display_TFT_CS, Display_TFT_DC, Display_TFT_RS);
    Adafruit_FT6206 touch = Adafruit_FT6206();  

    uint16_t getTFTColor(DisplayColor color);
    DisplayColor getTemperatureColor(uint8_t temperature);
    DisplayColor getTemperatureColor(float temperature);
};
#endif