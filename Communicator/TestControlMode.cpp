#include "TestControlMode.h"

bool TestControlMode::init () {
    GuiControlMode::init();
    logConsole("Test Mode");
}

void TestControlMode::loop () {

    display->clearAll();
    messageBufferLength = ((FullyInteractiveDisplay*)display)->getModalInput("Test Input Plz", 120, CharacterFilterNone, (char*)messageBuffer);

    display->showMessage((char*)messageBuffer, Gray);

    delay(100);
}
