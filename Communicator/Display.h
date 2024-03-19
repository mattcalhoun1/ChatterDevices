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

enum DisplayColor {
  Black = 0,
  Blue = 1,
  Red = 2,
  Green = 3,
  Cyan = 4,
  Magenta = 5,
  Yellow = 6,
  White = 7,
  Gray = 8
};

enum AlertType {
    AlertSuccess = 0,
    AlertWarning = 1,
    AlertError = 2,
    AlertActivity = 3
};

class Display {
  public:
    virtual void showThermal (const float* frame, int resHeight, int resWidth, int xOffset, int yOffset) = 0;
    virtual void showThermal (const uint8_t* frame, int resHeight, int resWidth, int xOffset, int yOffset) = 0;
    virtual void showInterpolatedThermalRow (const float* interpolatedRow, int xOffset, int yOffset) = 0;


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
    virtual void showMessage (const char* text, DisplayColor color);
    virtual void clearMessage ();
    virtual void clearDashboard ();

    virtual void showInterpolatedThermal (const uint8_t* image, bool isAlt, String subtitle);
    virtual void showInterpolatedThermal (const float* image, bool isAlt, String subtitle);


    // pass through methods
    virtual void fillRect(int lineX, int lineY, int lineWidth, int lineHeight, DisplayColor color) = 0;      
    virtual void drawLine (int lineX, int lineY, int lineEndX, int lineEndY, DisplayColor color) = 0;     
    virtual void drawCircle (int circleX, int circleY, int radius, DisplayColor color) = 0;     
    virtual void fillCircle (int circleX, int circleY, int radius, DisplayColor color) = 0;     

    virtual void clearAll ();

    virtual void showProgress (float percent);
    void showAlert (const char* alertText, AlertType alertType);

    void showDashboardItems (const char* item[], DisplayColor itemColor[], uint8_t numItems);

    virtual int getTextUpperVerticalOffset (TextSize textSize) { return 14 * (uint8_t)textSize; }
    virtual int getTextLowerVerticalOffset (TextSize textSize) { return 4 * (uint8_t)textSize; }

  protected:
    float currentProgress = 0; // placeholder for progress spinner
    void logConsole (String msg);

    virtual int getScreenWidth () = 0;
    virtual int getScreenHeight () = 0;

    virtual int getStatusX() = 0;
    virtual int getStatusY() = 0;
    virtual int getStatusWidth() = 0;
    virtual int getStatusHeight() = 0;

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

    virtual int getTitleAreaX () = 0;
    virtual int getTitleAreaY () = 0;
    virtual int getTitleAreaHeight () = 0;
    virtual int getTitleAreaWidth () = 0;

    virtual int getSubtitleAreaX () = 0;
    virtual int getSubtitleAreaY () = 0;
    virtual int getSubtitleAreaHeight () = 0;
    virtual int getSubtitleAreaWidth () = 0;

    virtual int getDashboardAreaY() = 0;
    virtual int getDashboardAreaHeight() = 0;

    virtual int getSpinnerX () = 0;
    virtual int getSpinnerY () = 0;
    virtual int getSpinnerRadius () = 0;

    virtual TextSize getTitleTextSize () = 0;
    virtual DisplayColor getTitleColor() = 0;

    virtual TextSize getSubtitleTextSize () = 0;
    virtual DisplayColor getSubtitleColor() = 0;

    virtual int getAlertAreaX() = 0;
    virtual int getAlertAreaY() = 0;
    virtual int getAlertAreaHeight() = 0;

    ThermalEncoder* encoder;
};
#endif