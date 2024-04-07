#include "MenuEnabledDisplay.h"

void MenuEnabledDisplay::clearMenuTitle () {
    clearArea(getMenuAreaX(), getMenuAreaY() - (getTextUpperVerticalOffset(TextMedium) +2), getMenuAreaWidth(), getMenuLineHeight() + getTextLowerVerticalOffset(TextMedium), DarkGreen);

    // horizontal line under title
    //clearArea(getMenuAreaX(), getMenuAreaY() - getTextUpperVerticalOffset(TextMedium) + getMenuLineHeight() + 8, getMenuAreaWidth(), 1);     
}

void MenuEnabledDisplay::clearMenu () {
    clearArea(getMenuAreaX(), getMenuAreaY() + getMenuLineHeight() - getTextUpperVerticalOffset(TextSmall), getMenuAreaWidth(), getMenuAreaHeight() - getMenuLineHeight(), DarkGreen);
}

void MenuEnabledDisplay::showMenuTitle (String& title) {
    //fillRect(getMenuAreaX(), getMenuAreaY(), getMenuAreaWidth(), getMenuLineHeight(), Black);
    showText(title, getMenuAreaX(), getMenuAreaY() /*+ getMenuLineHeight()*/, title.length() > getMenuMaxItemLength() ? TextSmall : TextMedium, Beige);

    // horizontal line under title
    //fillRect(getMenuAreaX(), getMenuAreaY() - getTextUpperVerticalOffset(TextMedium) + getMenuLineHeight() + 8, getMenuAreaWidth(), 1, White);     
}

void MenuEnabledDisplay::showMenuItem (uint8_t itemNumber, String& text, DisplayColor textColor, DisplayColor backgroundColor) {
    int itemX = getMenuAreaX();
    int itemY = 5 + getMenuAreaY() + ((itemNumber-1) * getMenuLineHeight()) + getMenuLineHeight();
    fillRect(itemX, itemY - (getMenuLineHeight()/5) - getTextUpperVerticalOffset(TextSmall), getMenuAreaWidth(), getMenuLineHeight() + getTextLowerVerticalOffset(TextSmall), backgroundColor);
    showText(text, itemX + getMenuItemIndent(), itemY, TextSmall, textColor);
}

void MenuEnabledDisplay::drawMenuBorder () {
    fillRect(getMenuAreaX() - getMenuBorderSize(), getMenuAreaY() - (getTextUpperVerticalOffset(TextMedium) + getMenuBorderSize() * 2), getMenuAreaWidth() + getMenuBorderSize() * 2, getMenuAreaHeight() + getMenuBorderSize() * 11, Beige);
}

void MenuEnabledDisplay::showScrolls (bool scrollUpEnabled, bool scrollDownEnabled) {
    int scrollUpX = getMenuScrollUpX();
    int scrollUpY = getMenuScrollUpY();
    int scrollDownX = getMenuScrollDownX();
    int scrollDownY = getMenuScrollDownY();

    uint8_t radius = getMenuScrollRadius();

    if (scrollUpEnabled) {
        fillTriangle (scrollUpX - (radius+2), scrollUpY + (radius+1), scrollUpX + (radius+2), scrollUpY + (radius+1), scrollUpX, scrollUpY - (radius+2), Beige);
        fillTriangle (scrollUpX - radius, scrollUpY + radius, scrollUpX + radius, scrollUpY + radius, scrollUpX, scrollUpY - radius, DarkGreen);
    }

    if (scrollDownEnabled) {
        fillTriangle (scrollDownX - (radius+2), scrollDownY - (radius+1), scrollDownX + (radius+2), scrollDownY - (radius+1), scrollDownX, scrollDownY + (radius+2), Beige);
        fillTriangle (scrollDownX - radius, scrollDownY - radius, scrollDownX + radius, scrollDownY - radius, scrollDownX, scrollDownY + radius, DarkGreen);
    }
}