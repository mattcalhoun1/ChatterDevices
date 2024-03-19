#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>


#include "ConnectivityMatrix.h"
#include "HeadlessControlMode.h"

#ifndef LEDHEADLESSCONTROLMODE_H
#define LEDHEADLESSCONTROLMODE_H

#define MATRIX_DOWN 0
#define MATRIX_UP 1
#define MATRIX_CHECK 2
#define MATRIX_X 3
#define MATRIX_CONNECTED 4
#define MATRIX_DISCONNECTED 5
#define MATRIX_BUSY 6
#define MATRIX_KEY 7

class LedHeadlessControlMode : public HeadlessControlMode {
    public:
        bool init ();

        virtual void updateChatDashboard ();
  
        void showBusy (); // shows indicator that activity is happening

        void showClusterOk ();
        void showClusterError ();
        void showClusterOnboard ();

    protected:
        Adafruit_IS31FL3731 matrix = Adafruit_IS31FL3731();
        void showLedStatus (ChatStatus loraStatus, ChatStatus wifiStatus);

        void showIndividualChannelStatus (char channelLetter, uint8_t matrixCode);
        void showBothChannelStatus (char channelLetter1, uint8_t matrixCode1, char channelLetter2, uint8_t matrixCode2);

        uint8_t MatrixValues[8][6] = {
            {   0b00011000,
                0b00011000,
                0b00011000,
                0b01111110,
                0b00111100,
                0b00011000 },
            {   0b00011000,
                0b00111100,
                0b01111110,
                0b00011000,
                0b00011000,
                0b00011000 },
            {   0b00000000,
                0b01000100,
                0b00101000,
                0b00010000,
                0b00101000,
                0b01000100 },
            {   0b00000000,
                0b00000010,
                0b00000100,
                0b10001000,
                0b01010000,
                0b00100000 },
            {   0b00000000,
                0b00000000,
                0b00000000,
                0b00000000,
                0b00000000,
                0b01111100 },
            {   0b00000000,
                0b00000000,
                0b00000000,
                0b00000000,
                0b00000000,
                0b01000100 },
            {   0b11111111,
                0b01111110,
                0b00011000,
                0b01111110,
                0b11111111,
                0b00000000 },
            {   0b11111111,
                0b10000001,
                0b11111111,
                0b00011000,
                0b00011000,
                0b01111000 }                            
        };

};

#endif