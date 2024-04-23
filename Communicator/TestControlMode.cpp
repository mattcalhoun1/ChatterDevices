#include "TestControlMode.h"

StartupState TestControlMode::init () {
    GuiControlMode::init();
    logConsole("Test Mode");
    return StartupComplete;
}

void TestControlMode::loop () {
    testKeyboard();

    delay(100);
}

void TestControlMode::testKeyboard () {
    // test no filter in default orientation
    display->clearAll();
    messageBufferLength = ((FullyInteractiveDisplay*)display)->getModalInput("Test Input", 20, CharacterFilterNone, (char*)messageBuffer);

    // test numeric filter in portrait orientation
    display->clearAll();
    ((FullyInteractiveDisplay*)display)->setKeyboardOrientation(Portrait);
    messageBufferLength = ((FullyInteractiveDisplay*)display)->getModalInput("Numeric", 20, CharacterFilterNumeric, (char*)messageBuffer);

    // test alpha filter in landscape orientation
    display->clearAll();
    ((FullyInteractiveDisplay*)display)->setKeyboardOrientation(Landscape);
    messageBufferLength = ((FullyInteractiveDisplay*)display)->getModalInput("Alpha Input", 20, CharacterFilterAlpha, (char*)messageBuffer);

    // test alphanumeric filter in landscape orientation
    display->clearAll();
    ((FullyInteractiveDisplay*)display)->setKeyboardOrientation(Landscape);
    messageBufferLength = ((FullyInteractiveDisplay*)display)->getModalInput("Alpha Input", 20, CharacterFilterAlphaNumeric, (char*)messageBuffer);

}