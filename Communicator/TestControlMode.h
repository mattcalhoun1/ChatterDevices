#include "ControlMode.h"
#include "GuiControlMode.h"

class TestControlMode : public GuiControlMode {
    public:
        TestControlMode(DeviceType _deviceType, bool _admin) : GuiControlMode (_deviceType, _admin) {}
        void loop ();
        bool init ();

    protected:
        void testKeyboard ();
};