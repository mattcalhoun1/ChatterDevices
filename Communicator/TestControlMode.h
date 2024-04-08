#include "ControlMode.h"
#include "GuiControlMode.h"

class TestControlMode : public GuiControlMode {
    public:
        TestControlMode(DeviceType _deviceType) : GuiControlMode (_deviceType) {}
        void loop ();
        bool init ();

    protected:
        void testKeyboard ();
};