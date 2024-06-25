#include "MenuEnabledDisplay.h"

void MenuEnabledDisplay::clearMenuTitle () {
    #if defined(DISPLAY_TYPE_ADAFRUIT_35)
    clearArea(getMenuAreaX(), getMenuAreaY() - (36 +2), getMenuAreaWidth(), getMenuLineHeight() + getTextLowerVerticalOffset(TextMedium) - 3, DarkGreen);
    #else
    clearArea(getMenuAreaX(), getMenuAreaY() - (getTextUpperVerticalOffset(TextMedium) +2), getMenuAreaWidth(), getMenuLineHeight() + getTextLowerVerticalOffset(TextMedium), DarkGreen);
    #endif

    // horizontal line under title
    //clearArea(getMenuAreaX(), getMenuAreaY() - getTextUpperVerticalOffset(TextMedium) + getMenuLineHeight() + 8, getMenuAreaWidth(), 1);     
}

void MenuEnabledDisplay::clearMenu () {
    #if defined(DISPLAY_TYPE_ADAFRUIT_35)
    clearArea(getMenuAreaX(), getMenuAreaY() + getMenuLineHeight() - 20, getMenuAreaWidth(), getMenuAreaHeight() - (getMenuLineHeight() + 18), DarkGreen);
    #else
    clearArea(getMenuAreaX(), getMenuAreaY() + getMenuLineHeight() - getTextUpperVerticalOffset(TextSmall), getMenuAreaWidth(), getMenuAreaHeight() - getMenuLineHeight(), DarkGreen);
    #endif

}

void MenuEnabledDisplay::showMenuTitle (String& title) {
    changeFont(FontUpSize);
    //fillRect(getMenuAreaX(), getMenuAreaY(), getMenuAreaWidth(), getMenuLineHeight(), Black);
    showText(title, getMenuTitleX(), getMenuTitleY() /*+ getMenuLineHeight()*/, TextSmall, Beige);
    changeFont(FontNormal);

    // horizontal line under title
    //fillRect(getMenuAreaX(), getMenuAreaY() - getTextUpperVerticalOffset(TextMedium) + getMenuLineHeight() + 8, getMenuAreaWidth(), 1, White);     
}

void MenuEnabledDisplay::showMenuItem (uint8_t itemNumber, String& text, DisplayColor textColor, DisplayColor backgroundColor) {
    int itemX = getMenuAreaX();
    int itemY = DISPLAY_TFT_MENU_VERTICAL_ITEM_OFFSET + getMenuAreaY() + ((itemNumber-1) * getMenuLineHeight()) + getMenuLineHeight();
    #if defined(DISPLAY_TYPE_ADAFRUIT_35)
    fillRect(itemX, itemY - (getMenuLineHeight()/numDisplayableItems) - getTextUpperVerticalOffset(TextSmall), getMenuAreaWidth(), getMenuLineHeight() - 28, backgroundColor);
    #else
    fillRect(itemX, itemY - (getMenuLineHeight()/numDisplayableItems) - getTextUpperVerticalOffset(TextSmall), getMenuAreaWidth(), getMenuLineHeight() + getTextLowerVerticalOffset(TextSmall), backgroundColor);
    #endif
    changeFont(FontBold);
    showText(text, itemX + getMenuItemIndent(), itemY + DISPLAY_TFT_MENU_VERTICAL_TEXT_OFFSET, TextSmall, textColor);
    changeFont(FontNormal);

    // if this isn't the last item, draw a line below it
    if (itemNumber < numDisplayableItems) {
        drawLine(itemX, itemY + getTextUpperVerticalOffset(TextSmall), itemX + getMenuAreaWidth(), itemY + getTextUpperVerticalOffset(TextSmall), Beige);
    }
}

uint8_t MenuEnabledDisplay::getMenuItemAt (int x, int y) {
    int relativeY = y - (getMenuAreaY() + DISPLAY_TFT_MENU_VERTICAL_ITEM_OFFSET);
    uint8_t offsetFactor = 0;

    #ifdef DISPLAY_TYPE_ADAFRUIT_35
        relativeY += 32;
        offsetFactor = 20;
    #endif

    if (x >= getMenuAreaX() && x <= getMenuAreaX() + getMenuAreaWidth()) {
        if (relativeY >= 0 && relativeY - getTextUpperVerticalOffset(TextSmall) < numDisplayableItems * getMenuLineHeight() + offsetFactor) {
            // we want center of the entry
            relativeY += getTextUpperVerticalOffset(TextSmall);
            uint8_t selectedItem = (relativeY / getMenuLineHeight()) - 1;

            return selectedItem;
        }
    }

    // none selected
    return MENU_ITEM_NONE;
}

void MenuEnabledDisplay::drawMenuBorder () {
    #if defined(DISPLAY_TYPE_ADAFRUIT_35)
    fillRect(getMenuAreaX() - getMenuBorderSize(), getMenuAreaY() - (36 + getMenuBorderSize() * 2), getMenuAreaWidth() + getMenuBorderSize() * 2, getMenuAreaHeight() + getMenuBorderSize() * 11, Beige);
    #else
    fillRect(getMenuAreaX() - getMenuBorderSize(), getMenuAreaY() - (getTextUpperVerticalOffset(TextMedium) + getMenuBorderSize() * 2), getMenuAreaWidth() + getMenuBorderSize() * 2, getMenuAreaHeight() + getMenuBorderSize() * 11, Beige);
    #endif
}

void MenuEnabledDisplay::blurMenuBackground () {
    // simply black out everything below the title
    clearArea(0, getTitleAreaHeight() + getSubtitleAreaHeight(), getScreenWidth(), getScreenHeight() - (getTitleAreaHeight() + getSubtitleAreaHeight()), Black);
}

void MenuEnabledDisplay::showMenuScrolls (bool _scrollUpEnabled, bool _scrollDownEnabled) {
    menuScrollUpEnabled = _scrollUpEnabled;
    menuScrollDownEnabled = _scrollDownEnabled;

    int scrollUpX = getMenuScrollUpX();
    int scrollUpY = getMenuScrollUpY();
    int scrollDownX = getMenuScrollDownX();
    int scrollDownY = getMenuScrollDownY();

    uint8_t radius = getMenuScrollRadius();

    if (menuScrollUpEnabled) {
        fillTriangle (scrollUpX - (radius+2), scrollUpY + (radius+1), scrollUpX + (radius+2), scrollUpY + (radius+1), scrollUpX, scrollUpY - (radius+2), Beige);
        fillTriangle (scrollUpX - radius, scrollUpY + radius, scrollUpX + radius, scrollUpY + radius, scrollUpX, scrollUpY - radius, DarkGreen);
    }
    else {
        fillTriangle (scrollUpX - (radius+2), scrollUpY + (radius+1), scrollUpX + (radius+2), scrollUpY + (radius+1), scrollUpX, scrollUpY - (radius+2), Black);
    }

    if (menuScrollDownEnabled) {
        fillTriangle (scrollDownX - (radius+2), scrollDownY - (radius+1), scrollDownX + (radius+2), scrollDownY - (radius+1), scrollDownX, scrollDownY + (radius+2), Beige);
        fillTriangle (scrollDownX - radius, scrollDownY - radius, scrollDownX + radius, scrollDownY - radius, scrollDownX, scrollDownY + radius, DarkGreen);
    }
    else {
        fillTriangle (scrollDownX - (radius+2), scrollDownY - (radius+1), scrollDownX + (radius+2), scrollDownY - (radius+1), scrollDownX, scrollDownY + (radius+2), Black);
    }
}

MenuScrollButton MenuEnabledDisplay::getMenuScrollButtonAt (int x, int y) {
    int scrollUpX = getMenuScrollUpX();
    int scrollUpY = getMenuScrollUpY();
    int scrollDownX = getMenuScrollDownX();
    int scrollDownY = getMenuScrollDownY();
    uint8_t radius = getMenuScrollTouchRadius();
    uint8_t tolerance = 10;

    if (x >= scrollUpX - (radius + tolerance) && x <= scrollUpX + (radius + tolerance) && y >= scrollUpY - (radius + tolerance) && y <= scrollUpY + (radius + tolerance)) {
        return MenuScrollUp;
    }
    else if (x >= scrollDownX - (radius + tolerance) && x <= scrollDownX + (radius + tolerance) && y >= scrollDownY - (radius + tolerance) && y <= scrollDownY + (radius + tolerance)) {
        return MenuScrollDown;
    }

    return MenuScrollNone;
}