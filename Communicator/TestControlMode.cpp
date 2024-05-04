#include "TestControlMode.h"

StartupState TestControlMode::init () {
    GuiControlMode::init();
    logConsole("Test Mode");

    // set up the test pool
    testPoolSize = chatter->getTrustStore()->populateDeviceIndices (chatter->getClusterId(), chatter->getDeviceId(), true, testPool, false);

    // set up any exclusions (devices not including in the test)
    testExclusions[0] = 14;
    testExclusionSize = 1;

    for (uint8_t targetId = 0; targetId < testPoolSize; targetId++) {
        // replace uint8 in the test pool with addresses , instead of the slot num
        Serial.print("target id: ");Serial.print(targetId);Serial.print(", slot:");Serial.print(testPool[targetId]);
        memset(devIdBuffer, 0, CHATTER_DEVICE_ID_SIZE+1);
        chatter->getTrustStore()->loadDeviceId (testPool[targetId], devIdBuffer);
        devIdBuffer[CHATTER_DEVICE_ID_SIZE] = 0;
        Serial.print(", device id: ");Serial.print(devIdBuffer);
        testPool[targetId] = chatter->getAddress(devIdBuffer);
        Serial.println(", addr:");Serial.println(testPool[targetId]);


        testSends[targetId] = 0;
        testDirects[targetId] = 0;
        testMeshes[targetId] = 0;
        testAcks[targetId] = 0;
    }

    return StartupComplete;
}

void TestControlMode::loop () {
    GuiControlMode::loop();

    // check the conditions that would trigger us to send a test message
    unsigned long now = millis();

    // if there's a send scheduled, do it now
    if (totalSent == 0 || (nextSendTime != 0 && now >= nextSendTime)) {
        logCurrentResults();
        
        logConsole("sending now...");
        if(sendTestMessage()) {
            nextSendTime = 0; // dequeue the send
        }
    } 
    else if (lastAckTime > lastSendTime) { // we receive an ack on the last message
        if (now - sendFrequency > lastSendTime) {
            logConsole("last test completed, schedule next test");
            nextSendTime = now + random(0, 30000);
        }

        // this means the ack came back, but we're waiting on more time to pass before we schedule another
        logConsole("waiting for next send opportunity");
    }
    else if (now - lastAckTime > maxAckWait && now - sendFrequency > lastSendTime) {
        // else if we WERE stuck waiting on an ack, but the max ack time has passed
        logConsole("stuck waiting on an ack, schedule next test");
        nextSendTime = now + random(0, 30000);
    }
    else if (now - lastAckTime <= maxAckWait) { 
        // we are still stuck wating on an ack time to pass
        logConsole("waiting on ack");
    }
    else if (nextSendTime == 0 && now - sendFrequency > lastSendTime) {
        // no test messages went out, now check if we received any response from our outstanding message
        logConsole("queuing next send");
        nextSendTime = now + random(0, 30000);
    }
}

bool TestControlMode::isExcluded (uint8_t deviceAddress) {
    // see if next target is in the exclusions
    for (uint8_t exc = 0; exc < testExclusionSize; exc++) {
        if (deviceAddress == testExclusions[exc]) {
            return true;
        }
    }

    return false;
}


void TestControlMode::logCurrentResults() {
    logConsole("== Test Results ==");
    logConsole("   Alias    | Sends |  Dir  |  Mesh |  Ack  | Success Rate");
    for (uint8_t p = 0; p < testPoolSize; p++) {
        if (!isExcluded(testPool[p])) {
            // get the alias of the target
            chatter->loadDeviceId (testPool[p], devIdBuffer);
            if(!chatter->getTrustStore()->loadAlias(devIdBuffer, aliasBuffer)) {
                sprintf(aliasBuffer, "%s", "[unknown]");
            }

            sprintf(
                logBuffer,
                "% 12s | %05d | %05d | %05d | %05d | %.2f",
                aliasBuffer,
                testSends[p],
                testDirects[p],
                testMeshes[p],
                testAcks[p],
                (float)testSends[p]/(float)testAcks[p]
            );
        }

    }
    logConsole("== End Test Results ==");
}

bool TestControlMode::sendTestMessage () {
    // randomly choose next
    uint8_t nextTarget = 254;
    uint8_t nextTargetId = 254;
    while (nextTarget == 254) {
        // randomly select a target from the pool
        nextTargetId = random(0, testPoolSize+1);
        nextTarget = testPool[nextTargetId];
        
        if (isExcluded(nextTarget)) {
            nextTarget = 254; // try again
        }
    }

    // set that device as the recipient
    memset(otherDeviceId, 0, CHATTER_DEVICE_ID_SIZE+1);
    memcpy(otherDeviceId, chatter->getClusterId(), CHATTER_GLOBAL_NET_ID_SIZE + CHATTER_LOCAL_NET_ID_SIZE);
    sprintf(otherDeviceId+CHATTER_GLOBAL_NET_ID_SIZE + CHATTER_LOCAL_NET_ID_SIZE, "%03d", nextTarget);

    memset(logBuffer, 0, TEST_LOG_BUFFER_SIZE);
    sprintf(logBuffer, "%s: %s", "Next test target", otherDeviceId);
    logConsole(logBuffer);

    // fill the outgoign message buffer
    generateMessageText();

    // send it direct first, if that fails, send via mesh
    MessageSendResult result = attemptDirectSend();
    bool wasSent = false;
    if (result != MessageNotSent) {
        // it worked
        lastSendTime = millis();
        totalSent++;
        testSends[nextTargetId] += 1;
        lastTargetId = nextTargetId;
        wasSent = true;

        if (result == MessageSentDirect) {
            testDirects[nextTargetId] += 1;
        }
        else if (result == MessageSentMesh) {
            testMeshes[nextTargetId] += 1;
        }
        else {
            // bridge not part of test
            logConsole("error, bad test send type!");
        }
    }

    return wasSent;
}

void TestControlMode::ackReceived(const char* sender, const char* messageId) {
    // if the sender is who we attempted a send to, assume this is the ack we were waiting for
    if (chatter->getAddress(sender) == testPool[lastTargetId]) {
        testAcks[lastTargetId] += 1;
        lastAckTime = millis(); // remember we got the ack
    }
}

void TestControlMode::generateMessageText () {
    // use log buffer to hold some digits
    memset(logBuffer, 0, TEST_LOG_BUFFER_SIZE);

    for (uint8_t i = 0; i < random(1, 50); i++) {
        logBuffer[i] = random(97,123); // a-z
    }

    sprintf((char*)messageBuffer, "%s@%s", "Test", logBuffer);
    messageBufferLength = strlen((const char*)messageBuffer);
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