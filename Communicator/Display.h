#include <Arduino.h>
#include "Globals.h"
#include "ThermalEncoder.h"

#ifndef DISPLAY_H
#define DISPLAY_H
enum TextSize {
  TextSmall = 1,
  TextMedium = 2,
  TextLarge = 3
};

enum FontType {
  FontNormal = 1,
  FontItalic = 2,
  FontBold = 3,
  FontTiny = 4,
  FontPico = 5
};

enum DisplayColor {
  Black = 0,
  Blue = 1,
  Red = 2,
  Green = 3,
  Cyan = 4,
  Magenta = 5,
  Yellow = 6,
  White = 7,
  Gray = 8,
  DarkBlue = 9,
  LightBlue = 10,
  Beige = 11,
  LightGreen = 12,
  DarkGreen = 13,
  LightGray = 14,
  DarkRed = 15,
  DarkGray = 16,
  BrightGreen = 17,
  BrightYellow = 18
};

enum AlertType {
    AlertSuccess = 0,
    AlertWarning = 1,
    AlertError = 2,
    AlertActivity = 3
};

enum ScreenRotation {
  Portrait = 0,
  Landscape = 1,
  PortraitFlip = 2,
  LandscapeFlip = 3
};

enum ScrollButton {
    ScrollUp = 0,
    ScrollDown = 1,
    ScrollNone = 2
};

#define DISPLAY_MESSAGE_POSITION_NULL 255

class Display {
  public:
    virtual void showThermal (const float* frame, int resHeight, int resWidth, int xOffset, int yOffset) = 0;
    virtual void showThermal (const uint8_t* frame, int resHeight, int resWidth, int xOffset, int yOffset) = 0;
    virtual void showInterpolatedThermalRow (const float* interpolatedRow, int xOffset, int yOffset) = 0;

    virtual void setBrightness(uint8_t brightness) = 0;

    virtual void showTitle (const char* text);
    virtual void showSubtitle (const char* text);

    virtual void showText (String text, int x, int y, TextSize size) = 0;
    virtual void showText (const char* text, int x, int y, TextSize size, DisplayColor color) = 0;
    virtual void showSymbol (int gfxChar, int x, int y) = 0;
    virtual void showText (String text, int x, int y, TextSize size, DisplayColor color) = 0;
    virtual void showSymbol (int gfxChar, int x, int y, DisplayColor color) = 0;
    virtual void clear () = 0;
    virtual void clearArea (int x, int y, int width, int height) = 0;
    virtual void clearArea (int x, int y, int width, int height, DisplayColor color) = 0;
    virtual void repaint () = 0;
    virtual void showStatus (String text, DisplayColor color);
    virtual void clearStatus ();

    virtual void showMessage (const char* text, DisplayColor color, uint8_t position);
    virtual void showMessageAndTitle (const char* title, const char* text, const char* readableTS, bool received, char status, char sendMethod, DisplayColor titleColor, DisplayColor messageColor, uint8_t position);
    virtual uint8_t getMessagePosition (int positionX, int positionY);
    //virtual void showHasMessagesBefore ();
    //virtual void showHasMessagesAfter ();

    virtual void clearMessageArea ();
    virtual uint8_t getMaxDisplayableMessages () = 0;

    virtual void clearDashboard ();

    virtual void showInterpolatedThermal (const uint8_t* image, bool isAlt, String subtitle);
    virtual void showInterpolatedThermal (const float* image, bool isAlt, String subtitle);


    // pass through methods
    virtual void fillRect(int lineX, int lineY, int lineWidth, int lineHeight, DisplayColor color) = 0;      
    virtual void drawRect(int lineX, int lineY, int lineWidth, int lineHeight, DisplayColor color) = 0;      
    virtual void drawLine (int lineX, int lineY, int lineEndX, int lineEndY, DisplayColor color) = 0;     
    virtual void drawCircle (int circleX, int circleY, int radius, DisplayColor color) = 0;     
    virtual void fillCircle (int circleX, int circleY, int radius, DisplayColor color) = 0;     
    virtual void fillTriangle (int x1, int y1, int x2, int y2, int x3, int y3, DisplayColor color) = 0;
    virtual void setRotation (ScreenRotation rotation) = 0;
    ScreenRotation getRotation () {return rotation;}

    virtual void clearAll ();

    void resetProgress () {currentProgress = 0;}
    void showProgress (float percent);
    void showAlert (const char* alertText, AlertType alertType);
    void showProgressBar (float percent);
    void showCacheUsed (float percent, bool forceRepaint = false);

    void showLatestCacheUsed ();


    void showDashboardItems (const char* item, DisplayColor itemColor[], uint8_t numItems);

    virtual int getTextUpperVerticalOffset (TextSize textSize) { return 14 * (uint8_t)textSize; }
    virtual int getTextLowerVerticalOffset (TextSize textSize) { return 4 * (uint8_t)textSize; }

    virtual int getScreenWidth () = 0;
    virtual int getScreenHeight () = 0;
    virtual bool isTouchEnabled () {return false;}

    virtual void changeFont (FontType fontType) {}

    // call it regularly so there is some indication on the screen
    // that the device isn't frozen
    virtual void showTick (uint8_t connectionQuality);

    void showMainScrolls (bool _scrollUpEnabled, bool _scrollDownEnabled);
    bool isScrollUpEnabled () { return scrollUpEnabled; }
    bool isScrollDownEnabled () { return scrollDownEnabled; }
    ScrollButton getScrollButtonAt (int x, int y);

  protected:
    float currentProgress = 0; // placeholder for progress spinner
    float currentCacheUsed = 0; // placeholder for cache pct
    void logConsole (String msg);

    virtual int getStatusX() = 0;
    virtual int getStatusY() = 0;
    virtual int getStatusWidth() = 0;
    virtual int getStatusHeight() = 0;

    virtual int getCacheStatusX() = 0;
    virtual int getCacheStatusY() = 0;
    virtual int getCacheStatusWidth() = 0;
    virtual int getCacheStatusHeight() = 0;

    virtual int getImageAreaX (bool isAlt) = 0;
    virtual int getImageAreaY (bool isAlt) = 0;
    virtual int getImageAreaWidth (bool isAlt) = 0;
    virtual int getImageAreaHeight (bool isAlt) = 0;
    virtual int getImageSubtitleX (bool isAlt) = 0;
    virtual int getImageSubtitleY (bool isAlt) = 0;

    virtual int getMessageAreaX () = 0;
    virtual int getMessageAreaY () = 0;
    virtual int getMessageAreaWidth () = 0;
    virtual int getMessageAreaHeight () = 0;
    virtual int getMessageHeight () = 0;
    virtual int getMessageTitleHeight () = 0;

    virtual int getTitleAreaX () = 0;
    virtual int getTitleAreaY () = 0;
    virtual int getTitleAreaHeight () = 0;
    virtual int getTitleAreaWidth () = 0;

    virtual int calculateSubtitleX (const char* titleText) = 0;
    virtual int calculateTitleX (const char* titleText) = 0;


    virtual int getSubtitleAreaX () = 0;
    virtual int getSubtitleAreaY () = 0;
    virtual int getSubtitleAreaHeight () = 0;
    virtual int getSubtitleAreaWidth () = 0;

    virtual int getDashboardAreaX() = 0;
    virtual int getDashboardAreaY() = 0;
    virtual int getDashboardAreaHeight() = 0;
    virtual int getDashboardAreaWidth() = 0;

    virtual int getSpinnerX () = 0;
    virtual int getSpinnerY () = 0;
    virtual int getSpinnerRadius () = 0;

    virtual int getProgressBarX () = 0;
    virtual int getProgressBarY () = 0;
    virtual int getProgressBarHeight () = 0;
    virtual int getProgressBarWidth () = 0;

    virtual int getTickerX () = 0;
    virtual int getTickerY () = 0;
    virtual int getTickerSize () = 0;

    virtual TextSize getTitleTextSize () = 0;
    virtual DisplayColor getTitleColor() = 0;

    virtual TextSize getSubtitleTextSize () = 0;
    virtual DisplayColor getSubtitleColor() = 0;

    virtual int getAlertAreaX() = 0;
    virtual int getAlertAreaY() = 0;
    virtual int getAlertAreaHeight() = 0;

    virtual int getMainScrollUpX() = 0;
    virtual int getMainScrollUpY() = 0;
    virtual int getMainScrollDownX() = 0;
    virtual int getMainScrollDownY() = 0;
    virtual int getMainScrollRadius() = 0;
    virtual int getMainScrollTouchRadius() = 0;

    ThermalEncoder* encoder;
    ScreenRotation rotation = Portrait;

    bool tickerShowing = false;

    bool scrollUpEnabled = false;
    bool scrollDownEnabled = false;
};
#endif