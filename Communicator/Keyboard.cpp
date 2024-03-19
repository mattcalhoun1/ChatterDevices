#include "Keyboard.h"

Keyboard::Keyboard(TouchEnabledDisplay* _display) {
  display = _display;

  width = display->getKeyboardAreaWidth();
  height = display->getKeyboardAreaHeight();
  x = display->getKeyboardAreaX();
  y = display->getKeyboardAreaY();

  display->setTouchListener(this);
}


bool Keyboard::init () {
  keyWidth = width / KEYBOARD_COLS;
  keyHeight = height / KEYBOARD_ROWS;
  fontSize = TextSmall;
}

void Keyboard::showKeyboard () {
  showKeyboard("");
}

void Keyboard::showKeyboard (char* defaultValue) {
  inputBufferLength = strlen(defaultValue);
  memcpy(inputBuffer, defaultValue, strlen(defaultValue));

  if (inputBufferLength > 0) {
    bufferEdited = true;
  }
  
  if (!showing) {
    // wrap keyboard in a rectangle
    display->fillRect(x, y, width, height, Gray);

    // wrap each key in a rectangle

    for (uint8_t rowCount = 0; rowCount < KEYBOARD_ROWS - 1; rowCount++) {
      // draw a line underneath this key row
      display->drawLine(x, y + (keyHeight * (rowCount+1)), x + width, y + (keyHeight * (rowCount+1)), White);
    }

    for (uint8_t colCount = 0; colCount < KEYBOARD_COLS - 1; colCount++) {
      // draw a line underneath this key row
      display->drawLine(x + (keyWidth * (colCount+1)), y, x + (keyWidth * (colCount+1)), y + height - keyHeight, White);
    }

    // draw the keys
    for (uint8_t rowCount = 0; rowCount < KEYBOARD_ROWS - 1; rowCount++) {
      for (uint8_t colCount = 0; colCount < KEYBOARD_COLS; colCount++) {
        display->showSymbol(keys[rowCount][colCount], x + (colCount * keyWidth) + .3 * keyWidth, y + (rowCount * keyHeight) + .2 * keyHeight + display->getTextUpperVerticalOffset(TextSmall), White);//, fontSize, White);
      }
    }

    // the last row gets painted a little differently
    uint8_t lastRow = KEYBOARD_ROWS-1;
    uint8_t colPosition = 0;
    for (uint8_t ctrlKey = 0; ctrlKey < 5; ctrlKey++) {
      colPosition = ctrlKey * 2;
      // draw the ctrl key
      switch (keys[lastRow][ctrlKey*2]) {
        case KEY_BACKSPACE:
          display->showText("Bksp", (int)(x + (colPosition * keyWidth) + .3 * keyWidth), (int)(y + (lastRow * keyHeight) + .2 * keyHeight + display->getTextUpperVerticalOffset(TextSmall)), TextSmall, White);
          break;
        case KEY_CLEAR:
          display->showText(" Clr", (int)(x + (colPosition * keyWidth) + .3 * keyWidth), (int)(y + (lastRow * keyHeight) + .2 * keyHeight + display->getTextUpperVerticalOffset(TextSmall)), TextSmall, White);
          break;
        case KEY_CANCEL:
          display->showText("Esc", (int)(x + (colPosition * keyWidth) + .3 * keyWidth), (int)(y + (lastRow * keyHeight) + .2 * keyHeight + display->getTextUpperVerticalOffset(TextSmall)), TextSmall, White);
          break;
        case ' ':
          display->showText(" ", (int)(x + (colPosition * keyWidth) + .3 * keyWidth), (int)(y + (lastRow * keyHeight) + .2 * keyHeight + display->getTextUpperVerticalOffset(TextSmall)), TextSmall, White);
          break;
        case KEY_SEND:
          display->showText(" Go", (int)(x + (colPosition * keyWidth) + .3 * keyWidth), (int)(y + (lastRow * keyHeight) + .2 * keyHeight + display->getTextUpperVerticalOffset(TextSmall)), TextSmall, White);
          break;
      }
    }

    // lines between bottom row keys
    for (uint8_t colCount = 1; colCount < KEYBOARD_COLS - 1; colCount+= 2) {
      // draw a line underneath this key row
      display->drawLine(x + (keyWidth * (colCount+1)), y + height - keyHeight, x + (keyWidth * (colCount+1)), y + height, White);
    }

    showing = true;
  }
}


int Keyboard::getUserInputLength() {
  if (inputBufferLength == 0) {
    return 0;
  }
  else if (userTerminatedInput() || userCompletedInput()) {
    return inputBufferLength - 1;
  }
  return inputBufferLength;
}

void Keyboard::hideKeyboard () {
  if (showing) {

    display->clearArea(x, y, width + 1, height + 1, Black);

    showing = false;
  }
}

bool Keyboard::handleScreenTouched (int touchX, int touchY) {
  if (touchX == 0 || touchY == 0 || touchX == 270 || touchY == 320) {
    // ignore, it's at an edge and likely misreading
    return false;
  }

  char typedKey = getLetterAt(touchX, touchY);
  if (typedKey == lastKey && millis() - lastTouch < KEY_HOLD_REPEAT_IGNORE_MILLIS) {
    // ignore
  }
  else if (typedKey != 0) {
    lastKey = typedKey;
    //Serial.println(typedKey);
    if (typedKey == KEY_BACKSPACE) {
      bufferEdited = true;
      if (inputBufferLength > 0) {
        inputBufferLength--;
      }
    }
    else if (typedKey == KEY_CLEAR) {
      bufferEdited = true;
      inputBufferLength = 0;
    }
    else {
      // if previous key was a space, display should be repainted
      if (inputBufferLength > 0 && inputBuffer[inputBufferLength-1] == ' ') {
        bufferEdited = true;
      }
      else {
        bufferEdited = false;
      }
      inputBuffer[inputBufferLength++] = typedKey;
    }

    lastTouch = millis();
    return true;
  }
}

bool Keyboard::userTerminatedInput() {
  if (inputBufferLength != 0) {
    if (inputBuffer[inputBufferLength-1] == KEY_CANCEL) {
      return true;
    }
  }

  return false;
}

bool Keyboard::userCompletedInput() {
  if (inputBufferLength != 0) {
    if (inputBuffer[inputBufferLength-1] == KEY_SEND) {
      return true;
    }
  }

  return false;
}

char Keyboard::getLetterAt (int touchX, int touchY) {

  // the touch should be toward the center of the key. otherwise we dont count it

  float frow = ((touchY - (.2*keyHeight) - y) / (float)keyHeight);
  float fcol = ((touchX - (.4*keyWidth) - x) / (float)keyWidth) - 1;

  fcol = max(.1, fcol);
  fcol = min(KEYBOARD_COLS + .1, fcol);
  //frow = max(.5, frow);

  // get the row
  uint8_t row = floor(frow);
  uint8_t col = floor(fcol);

  //Serial.print("Touched: ");Serial.print(touchX); Serial.print(", ");Serial.println(touchY);

  int xDistFromCenter = abs((int)(round(fcol*10)) - (col*10));
  int yDistFromCenter = abs((int)(round(frow*10)) - (row*10));

  //Serial.print("x dist: ");Serial.print(xDistFromCenter); Serial.print(", y dist: ");Serial.println(yDistFromCenter);

  if (xDistFromCenter < (.5*keyWidth) && yDistFromCenter < (.5*keyHeight)) {
    if (row >= 0 && row < KEYBOARD_ROWS) {
      if (col >= 0 && col < KEYBOARD_COLS) {

        return keys[row][col];
      }
    }
    //else {
    //  Serial.print("No key at: ");Serial.print(touchX); Serial.print(", ");Serial.println(touchY);
    //}
  }


  return 0;
}
