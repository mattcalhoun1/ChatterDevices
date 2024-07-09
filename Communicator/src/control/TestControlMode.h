#include "ControlMode.h"
#include "GuiControlMode.h"

#define TEST_LOG_BUFFER_SIZE 128

class TestControlMode : public GuiControlMode {
    public:
        TestControlMode(DeviceType _deviceType) : GuiControlMode (_deviceType) {}
        void loop ();
        StartupState init ();

        void ackReceived(const char* sender, const char* messageId);

    protected:
        void testKeyboard ();

        void logCurrentResults();

        bool sendTestMessage ();
        void generateMessageText ();
        bool isExcluded (uint8_t deviceAddress);

        // all devices in the test pool, not including self
        uint8_t testPool[10];
        uint8_t testPoolSize = 0;

        // devices to exclude
        uint8_t testExclusions[2];
        uint8_t testExclusionSize = 0;

        unsigned int testSends[5]; // counter of sends
        unsigned int testAcks[5]; // counter of receives
        unsigned int testDirects[5]; // how many were directly sent
        unsigned int testMeshes[5]; // how many were meshed

        unsigned long sendFrequency = 120000; // every 2 min
        unsigned long maxAckWait = 240000; // how long (max) to wait for an ack before sending a different test message

        uint8_t lastTargetId; // index of who we sent the last message to
        unsigned long lastSendTime = 0; // millis of last message send time
        unsigned long nextSendTime = 0;
        unsigned long lastAckTime = 0;
        unsigned long totalSent = 0;

        char logBuffer[TEST_LOG_BUFFER_SIZE];

        char devIdBuffer[CHATTER_DEVICE_ID_SIZE + 1];
        char aliasBuffer[CHATTER_ALIAS_NAME_SIZE + 1];

};