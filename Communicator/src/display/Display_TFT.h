#ifndef DISPLAY_TFT_H
#define DISPLAY_TFT_H

#include "Display.h"
#include "MenuEnabledDisplay.h"
#include "TouchEnabledDisplay.h"
#include "FullyInteractiveDisplay.h"
#include "../globals/Globals.h"
#include "../keyboard/Keyboard.h"

#include <Adafruit_GFX.h>    // Core graphics library
#if defined(DISPLAY_TYPE_HOYSOND)
#include <Adafruit_ST7796S_kbv.h>
#elif defined(DISPLAY_TYPE_ADAFRUIT_28)
#include <Adafruit_ILI9341.h> // Hardware-specific library
#elif defined(DISPLAY_TYPE_ADAFRUIT_35)
#include <Adafruit_HX8357.h> // Hardware-specific library
#endif
#include "../touch/TouchControl.h"

#if defined(TOUCH_CONTROL_RAK)
#include "../touch/TouchControlRak.h"
#elif defined(TOUCH_CONTROL_ADAFRUIT_28)
#include "../touch/TouchControlAdafruit.h"
#elif defined(TOUCH_CONTROL_ADAFRUIT_35)
#include "../touch/TouchControlAdafruit35.h"
#else
#include "../touch/TouchControlNone.h"
#endif

// fonts
#if defined(DISPLAY_TYPE_ADAFRUIT_35)
#include <Fonts/RoboFlex7pt7b.h> // slightly smaller font
#include <Fonts/RoboFlex8pt7b.h> // slightly smaller font
#include <Fonts/RoboFlex9pt7b.h> // slightly smaller font
#include <Fonts/FreeSans10pt7b.h> // default font
#include <Fonts/FreeSans12pt7b.h> // slightly larger font
#include <Fonts/FreeSans14pt7b.h> // slightly larger font
#include <Fonts/FreeSansOblique10pt7b.h> // italic sort of
#include <Fonts/FreeSansBold10pt7b.h> // italic sort of
#include <Fonts/MaterialIconsRegular10pt7b.h>
//#include <Fonts/Picopixel.h>
#else
#include <Fonts/RoboFlex8pt7b.h> // slightly smaller font
#include <Fonts/FreeSans9pt7b.h> // default font
#include <Fonts/FreeSans10pt7b.h> // slightly larger font
#include <Fonts/FreeSansOblique9pt7b.h> // italic sort of
#include <Fonts/FreeSansBold9pt7b.h> // italic sort of
#include <Fonts/Picopixel.h>
#include <Fonts/MaterialIconsRegular10pt7b.h>
#endif

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
    void drawRect(int lineX, int lineY, int lineWidth, int lineHeight, DisplayColor color);
    void drawLine (int lineX, int lineY, int lineEndX, int lineEndY, DisplayColor color);
    void drawCircle (int circleX, int circleY, int radius, DisplayColor color);
    void fillCircle (int circleX, int circleY, int radius, DisplayColor color);
    void fillTriangle (int x1, int y1, int x2, int y2, int x3, int y3, DisplayColor color);
    void setRotation (ScreenRotation _rotation);

    void setBrightness(uint8_t brightness);

    bool handleIfTouched ();
    bool wasTouched ();
    void clearTouchInterrupts ();
    void setTouchSensitivity (TouchSensitivity sensitivity);
    void resetToDefaultTouchSensitivity ();

    int getModalInput (const char* title, int maxLength, CharacterFilter charFilter, char* buffer);
    int getModalInput (const char* title, int maxLength, CharacterFilter charFilter, char* buffer, const char* defaultValue);
    int getModalInput (const char* title, int maxLength, CharacterFilter charFilter, char* buffer, const char* defaultValue, int timeoutMillis);
    int getModalInput (const char* title, const char* subtitle, int maxLength, CharacterFilter charFilter, char* buffer, const char* defaultValue, int timeoutMillis);

    int getModalInput (const char* title, const char* subtitle, int maxLength, CharacterFilter charFilter, char* buffer, const char* defaultValue, int timeoutMillis, Keyboard* keyboard);

    int getScreenWidth () { return DISPLAY_TFT_WIDTH; }
    int getScreenHeight () { return DISPLAY_TFT_HEIGHT; }

    void changeFont (FontType fontType);

    void setTouchListening(bool _listening);
    void touchInterrupt();
    void showButtons ();
    DisplayedButton getButtonAt (int x, int y);

  protected:
    int getStatusX();
    int getStatusY();
    int getStatusWidth();
    int getStatusHeight();

    int getCacheStatusX() { return DISPLAY_TFT_CACHE_STATUS_X; }
    int getCacheStatusY() { return DISPLAY_TFT_CACHE_STATUS_Y; }
    int getCacheStatusWidth() { return DISPLAY_TFT_CACHE_STATUS_WIDTH; }
    int getCacheStatusHeight() { return DISPLAY_TFT_CACHE_STATUS_HEIGHT; }

    int getBatteryStatusX() { return DISPLAY_TFT_BATTERY_STATUS_X; }
    int getBatteryStatusY() { return DISPLAY_TFT_BATTERY_STATUS_Y; }
    int getBatteryStatusWidth() { return DISPLAY_TFT_BATTERY_STATUS_WIDTH; }
    int getBatteryStatusHeight() { return DISPLAY_TFT_BATTERY_STATUS_HEIGHT; }

    int getImageAreaX (bool isAlt);
    int getImageAreaY (bool isAlt);
    int getImageAreaWidth (bool isAlt);
    int getImageAreaHeight (bool isAlt);

    int getImageSubtitleX (bool isAlt);
    int getImageSubtitleY (bool isAlt);

    int getMessageAreaX () {return DISPLAY_TFT_MSG_X; }
    int getMessageAreaY () {return DISPLAY_TFT_MSG_Y; }
    int getMessageAreaWidth () {return DISPLAY_TFT_MSG_WIDTH; }
    int getMessageAreaHeight () {return DISPLAY_TFT_MSG_AREA_HEIGHT; }
    int getMessageHeight () {return DISPLAY_TFT_MSG_HEIGHT; }
    int getMessageTitleHeight () {return DISPLAY_TFT_MSG_TITLE_HEIGHT; }
    uint8_t getMaxDisplayableMessages() { return DISPLAY_TFT_MSG_MAX_DISPLAY; }
    int getMessageTitleTsXOffset() {return DISPLAY_TFT_MSG_TS_X_OFFSET; }
    int getMessageTitleYOffset() {return DISPLAY_TFT_MSG_TITLE_Y_OFFSET; }
    int getMessageTitleXOffset() {return DISPLAY_TFT_MSG_TITLE_X_OFFSET; }
    int getMessageStatusXOffset() {return DISPLAY_TFT_MSG_STATUS_X_OFFSET; }
    int getMessageStatusRadius() {return DISPLAY_STATUS_RADIUS;}
    int getMessageTitleIconOffset() {return DISPLAY_TFT_MSG_TITLE_ICON_Y_OFFSET;}

    int getMenuAreaX () { return DISPLAY_TFT_MENU_X; }
    int getMenuAreaY () { return DISPLAY_TFT_MENU_Y; }
    int getMenuAreaWidth () { return DISPLAY_TFT_MENU_WIDTH; }
    int getMenuAreaHeight () { return DISPLAY_TFT_MENU_HEIGHT; }
    int getMenuLineHeight () { return DISPLAY_TFT_MENU_LINE_HEIGHT; }
    int getMenuMaxItemLength () { return DISPLAY_TFT_MENU_MAX_ITEM_LENGTH; }
    int getMenuItemIndent () { return DISPLAY_TFT_MENU_INDENT; }

    int getMenuScrollUpX() { return DISPLAY_TFT_MENU_SCROLL_UP_X; }
    int getMenuScrollUpY() { return DISPLAY_TFT_MENU_SCROLL_UP_Y; }
    int getMenuScrollDownX() { return DISPLAY_TFT_MENU_SCROLL_DOWN_X; }
    int getMenuScrollDownY() { return DISPLAY_TFT_MENU_SCROLL_DOWN_Y; }
    int getMenuScrollRadius() { return DISPLAY_TFT_MENU_SCROLL_RADIUS; }
    int getMenuScrollTouchRadius() { return DISPLAY_TFT_MENU_SCROLL_TOUCH_RADIUS; }

    int getMainScrollUpX() { return DISPLAY_TFT_MAIN_SCROLL_UP_X; }
    int getMainScrollUpY() { return DISPLAY_TFT_MAIN_SCROLL_UP_Y; }
    int getMainScrollDownX() { return DISPLAY_TFT_MAIN_SCROLL_DOWN_X; }
    int getMainScrollDownY() { return DISPLAY_TFT_MAIN_SCROLL_DOWN_Y; }
    int getMainScrollRadius() { return DISPLAY_TFT_MAIN_SCROLL_RADIUS; }
    int getMainScrollTouchRadius() { return DISPLAY_TFT_MAIN_SCROLL_TOUCH_RADIUS; }

    int getTitleAreaX () { return DISPLAY_TFT_TITLE_X; }
    int getTitleAreaY () { return DISPLAY_TFT_TITLE_Y; }
    int getTitleAreaHeight () { return DISPLAY_TFT_TITLE_HEIGHT; }
    int getTitleAreaWidth () { return DISPLAY_TFT_TITLE_WIDTH; }

    int getSubtitleAreaX () { return DISPLAY_TFT_SUBTITLE_X; }
    int getSubtitleAreaY () { return DISPLAY_TFT_SUBTITLE_Y; }
    int getSubtitleAreaHeight () { return DISPLAY_TFT_SUBTITLE_HEIGHT; }
    int getSubtitleAreaWidth () { return DISPLAY_TFT_SUBTITLE_WIDTH; }

    int getDashboardAreaX () { return DISPLAY_TFT_DASHBOARD_X; }
    int getDashboardAreaY () { return DISPLAY_TFT_DASHBOARD_Y; }
    int getDashboardAreaHeight() { return DISPLAY_TFT_DASHBOARD_HEIGHT; }
    int getDashboardAreaWidth () { return DISPLAY_TFT_DASHBOARD_WIDTH; }

    int getKeyboardAreaX () { return rotation == Landscape ? DISPLAY_TFT_LS_KEYBOARD_X : DISPLAY_TFT_KEYBOARD_X; }
    int getKeyboardAreaY () { return rotation == Landscape ? DISPLAY_TFT_LS_KEYBOARD_Y : DISPLAY_TFT_KEYBOARD_Y; }

    int getKeyboardAreaHeight () { return rotation == Landscape ? DISPLAY_TFT_LS_KEYBOARD_HEIGHT : DISPLAY_TFT_KEYBOARD_HEIGHT; }
    int getKeyboardAreaWidth () { return rotation == Landscape ? DISPLAY_TFT_LS_KEYBOARD_WIDTH : DISPLAY_TFT_KEYBOARD_WIDTH; }

    TextSize getTitleTextSize () { return TextSmall; }
    DisplayColor getTitleColor() { return Beige; }

    TextSize getSubtitleTextSize () { return TextSmall; }
    DisplayColor getSubtitleColor() { return DarkRed; }

    int getSpinnerX () { return DISPLAY_TFT_SPINNER_X; }
    int getSpinnerY () { return DISPLAY_TFT_SPINNER_Y; }
    int getSpinnerRadius () { return DISPLAY_TFT_SPINNER_RADIUS; }

    int getProgressBarX () { return DISPLAY_TFT_PROGRESS_BAR_X; }
    int getProgressBarY ()  { return DISPLAY_TFT_PROGRESS_BAR_Y; }
    int getProgressBarHeight ()  { return DISPLAY_TFT_PROGRESS_BAR_HEIGHT; }
    int getProgressBarWidth ()  { return DISPLAY_TFT_PROGRESS_BAR_WIDTH; }

    int getTickerX () { return DISPLAY_TFT_TICKER_X; }
    int getTickerY () { return DISPLAY_TFT_TICKER_Y; }
    int getTickerSize () { return DISPLAY_TFT_TICKER_SIZE; }

    int getModalTitleX () { return rotation == Landscape ? DISPLAY_TFT_LS_MODAL_TITLE_X :  DISPLAY_TFT_MODAL_TITLE_X; }
    int getModalTitleY () { return rotation == Landscape ? DISPLAY_TFT_LS_MODAL_TITLE_Y : DISPLAY_TFT_MODAL_TITLE_Y; }
    int getModalTitleHeight () { return rotation == Landscape ? DISPLAY_TFT_LS_MODAL_TITLE_HEIGHT : DISPLAY_TFT_MODAL_TITLE_HEIGHT; }

    int getModalSubTitleX () { return rotation == Landscape ? DISPLAY_TFT_LS_MODAL_SUBTITLE_X :  DISPLAY_TFT_MODAL_SUBTITLE_X; }
    int getModalSubTitleY () { return rotation == Landscape ? DISPLAY_TFT_LS_MODAL_SUBTITLE_Y :  DISPLAY_TFT_MODAL_SUBTITLE_Y; }
    int getModalSubTitleHeight () { return rotation == Landscape ? DISPLAY_TFT_LS_MODAL_SUBTITLE_HEIGHT :  DISPLAY_TFT_MODAL_SUBTITLE_HEIGHT; }

    int getModalInputX () { return rotation == Landscape ? DISPLAY_TFT_LS_MODAL_INPUT_X : DISPLAY_TFT_MODAL_INPUT_X; }
    int getModalInputY () { return rotation == Landscape ? DISPLAY_TFT_LS_MODAL_INPUT_Y : DISPLAY_TFT_MODAL_INPUT_Y; }
    int getModalInputWidth () { return rotation == Landscape ? DISPLAY_TFT_LS_MODAL_INPUT_WIDTH : DISPLAY_TFT_MODAL_INPUT_WIDTH; }
    int getModalInputHeight () { return rotation == Landscape ? DISPLAY_TFT_LS_MODAL_INPUT_HEIGHT : DISPLAY_TFT_MODAL_INPUT_HEIGHT; }

    int calculateSubtitleX (const char* titleText);
    int calculateTitleX (const char* titleText);

    int getAlertAreaX() { return rotation == Landscape ? DISPLAY_TFT_LS_ALERT_X : DISPLAY_TFT_ALERT_X; }
    int getAlertAreaY() { return rotation == Landscape ? DISPLAY_TFT_LS_ALERT_Y: DISPLAY_TFT_ALERT_Y; }
    int getAlertAreaHeight() { return rotation == Landscape ? DISPLAY_TFT_LS_ALERT_HEIGHT : DISPLAY_TFT_ALERT_HEIGHT; }

    int getButtonAreaX() {return DISPLAY_BUTTON_AREA_X;}
    int getButtonAreaY() {return DISPLAY_BUTTON_AREA_Y;}
    int getButtonWidth() {return DISPLAY_BUTTON_WIDTH;}
    int getButtonHeight() {return DISPLAY_BUTTON_HEIGHT;}
    int getButtonHorizontalOffset() {return DISPLAY_BUTTON_HORIZONTAL_OFFSET;}

    int getLockButtonX() { return DISPLAY_TFT_LOCK_BUTTON_X; }
    int getLockButtonY() { return DISPLAY_TFT_LOCK_BUTTON_Y; }
    int getLockButtonSize() { return DISPLAY_TFT_LOCK_BUTTON_SIZE; }

    int getFlipButtonX() { return DISPLAY_TFT_FLIP_BUTTON_X; }
    int getFlipButtonY() { return DISPLAY_TFT_FLIP_BUTTON_Y; }
    int getFlipButtonSize() { return DISPLAY_TFT_FLIP_BUTTON_SIZE; }


    void showButton(uint8_t buttonPosition, const char* buttonText);
    const char* getButtonText (DisplayedButton btn){return buttonTexts[btn];}

    int getMainAreaX () {return DISPLAY_TFT_MAIN_AREA_X;}
    int getMainAreaY () {return DISPLAY_TFT_MAIN_AREA_Y;}
    int getMainAreaHeight () {return DISPLAY_TFT_MAIN_AREA_HEIGHT;}

    int getMainSubAreaX () {return DISPLAY_TFT_MAIN_SUB_AREA_X;}
    int getMainSubAreaY () {return DISPLAY_TFT_MAIN_SUB_AREA_Y;}
    int getMainSubAreaHeight () {return DISPLAY_TFT_MAIN_SUB_AREA_HEIGHT;}


  private:
    int calculateModalTitleX (const char* titleText, FontType fontType);
    unsigned long lastModalActivity = 0;

    #if defined(DISPLAY_TYPE_HOYSOND)
    Adafruit_ST7796S_kbv display = Adafruit_ST7796S_kbv(Display_TFT_CS, Display_TFT_DC, Display_TFT_RS);
    #elif defined(DISPLAY_TYPE_ADAFRUIT_28)
    Adafruit_ILI9341 display = Adafruit_ILI9341(Display_TFT_CS, Display_TFT_DC, Display_TFT_RS);
    #elif defined(DISPLAY_TYPE_ADAFRUIT_35)
    Adafruit_HX8357 display = Adafruit_HX8357(Display_TFT_CS, Display_TFT_DC);
    #endif
    TouchControl* touch;

    uint16_t getTFTColor(DisplayColor color);
    DisplayColor getTemperatureColor(uint8_t temperature);
    DisplayColor getTemperatureColor(float temperature);

    FontType currFontType = FontNormal;

    const char* buttonTexts[NUM_DISPLAYED_BUTTONS] = {DISPLAY_BUTTON_TEXT_SEND, DISPLAY_BUTTON_TEXT_FILTER, DISPLAY_BUTTON_TEXT_MENU};

    bool touchListening = false;
    bool touchInitialized = false;
};
#endif