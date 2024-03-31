#include "MenuEnabledDisplay.h"

void MenuEnabledDisplay::clearMenuTitle () {
    clearArea(getMenuAreaX(), getMenuAreaY() - (getTextUpperVerticalOffset(TextMedium) +2), getMenuAreaWidth(), getMenuLineHeight() + getTextLowerVerticalOffset(TextMedium));

    // horizontal line under title
    clearArea(getMenuAreaX(), getMenuAreaY() - getTextUpperVerticalOffset(TextMedium) + getMenuLineHeight() + 8, getMenuAreaWidth(), 1);     
}

void MenuEnabledDisplay::clearMenu () {
    clearArea(getMenuAreaX(), getMenuAreaY() + getMenuLineHeight() - getTextUpperVerticalOffset(TextSmall), getMenuAreaWidth(), getMenuAreaHeight() - getMenuLineHeight());
}

void MenuEnabledDisplay::showMenuTitle (String& title) {
    //fillRect(getMenuAreaX(), getMenuAreaY(), getMenuAreaWidth(), getMenuLineHeight(), Black);
    showText(title, getMenuAreaX(), getMenuAreaY() /*+ getMenuLineHeight()*/, title.length() > getMenuMaxItemLength() ? TextSmall : TextMedium, White);

    // horizontal line under title
    fillRect(getMenuAreaX(), getMenuAreaY() - getTextUpperVerticalOffset(TextMedium) + getMenuLineHeight() + 8, getMenuAreaWidth(), 1, White);     
}

void MenuEnabledDisplay::showMenuItem (uint8_t itemNumber, String& text, DisplayColor textColor, DisplayColor backgroundColor) {
    int itemX = getMenuAreaX();
    int itemY = 5 + getMenuAreaY() + ((itemNumber-1) * getMenuLineHeight()) + getMenuLineHeight();
    fillRect(itemX, itemY - (getMenuLineHeight()/5) - getTextUpperVerticalOffset(TextSmall), getMenuAreaWidth(), getMenuLineHeight() + getTextLowerVerticalOffset(TextSmall), backgroundColor);
    showText(text, itemX + getMenuItemIndent(), itemY, TextSmall, textColor);
}