#include "TouchEnabledDisplay.h"

#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEYBOARD_ROWS 5
#define KEYBOARD_COLS 10
#define KEYBOARD_BUFFER_SIZE 150 // user input cannot be larger than this

#define KEY_BACKSPACE 0x11
#define KEY_CLEAR 0xAE
#define KEY_CANCEL 0x19
#define KEY_SEND 0xAF

#define KEY_HOLD_REPEAT_IGNORE_MILLIS 500

class Keyboard : public TouchListener {
  public:
    Keyboard(TouchEnabledDisplay* _display);
    bool init ();

    void showKeyboard ();
    void showKeyboard (char* defaultValue);
    void hideKeyboard ();

    const char* getUserInput () { return inputBuffer; }
    int getUserInputLength ();
    bool userTerminatedInput ();
    bool userCompletedInput ();

    bool handleScreenTouched (int touchX, int touchY);
    bool wasBufferEdited () { return bufferEdited; }
    void resetBufferEditFlag () { bufferEdited = false; }

  protected:
    TouchEnabledDisplay* display;

    uint8_t keyWidth;
    uint8_t keyHeight;
    TextSize fontSize;
    bool bufferEdited = false; // if the user edited (not appended) buffer, so know to repaint. example, backspace


    uint8_t height;
    uint8_t width;
    uint8_t x;
    uint8_t y;
    bool showing = false;

    char lastKey = 0;

    char getLetterAt (int touchX, int touchY);
    char inputBuffer[KEYBOARD_BUFFER_SIZE];
    int inputBufferLength = 0;
    unsigned long lastTouch = 0;

    char keys [KEYBOARD_ROWS][KEYBOARD_COLS] = {
      {
        '1','2','3','4','5','6','7','8','9','0'
      },
      {
        'q','w','e','r','t','y','u','i','o','p'
      },
      {
        'a','s','d','f','g','h','j','k','l',';'
      },
      {
        'z','x','c','v','b','n','m',',','.','?'
      },
      {
        KEY_BACKSPACE, KEY_BACKSPACE, KEY_CLEAR, KEY_CLEAR, KEY_CANCEL, KEY_CANCEL, ' ', ' ', KEY_SEND, KEY_SEND        
      }

    };
};

#endif KEYBOARD_H