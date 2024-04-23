#include "ControlMode.h"
#include "GuiControlMode.h"

class TestControlMode : public GuiControlMode {
    public:
        TestControlMode(DeviceType _deviceType) : GuiControlMode (_deviceType) {}
        void loop ();
        StartupState init ();

    protected:
        void testKeyboard ();
};