#if defined(ARDUINO_UNOR4_WIFI)

#include "ConnectivityMatrix.h"
#include "Arduino_LED_Matrix.h"
#include "HeadlessControlMode.h"

#ifndef R4HEADLESSCONTROLMODE_H
#define R4HEADLESSCONTROLMODE_H

class R4HeadlessControlMode : public HeadlessControlMode {
    public:
        bool init ();

        virtual void updateChatDashboard ();
  
        void showBusy (); // shows indicator that activity is happening

        void showClusterOk ();
        void showClusterError ();
        void showClusterOnboard ();

    protected:
        ArduinoLEDMatrix ledMatrix;
        uint8_t getFrameFor (ChatStatus loraStatus, ChatStatus wifiStatus);
};

#endif

#endif