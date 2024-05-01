#include "NearbyDeviceIterator.h"

bool NearbyDeviceIterator::init (const char* _clusterId, const char* _thisDeviceId, bool _sortAsc) {
    memcpy(thisDeviceId, _thisDeviceId, CHATTER_DEVICE_ID_SIZE);
    thisDeviceId[CHATTER_DEVICE_ID_SIZE] = 0;

    meshGraph = chatter->getMeshGraph();

    numDevices = pingTable->loadNearbyDevices (PingQualityBad, deviceSlots, CHATTER_MAX_NEARBY_DEVICES, 180);
    //Serial.print("Iterator loaded "); Serial.print(numDevices); Serial.print(" nearby devices for cluster ");Serial.println(_clusterId);
    return true;
}

uint8_t NearbyDeviceIterator::getNumItems () {
    return numDevices;
}

bool NearbyDeviceIterator::loadItemName (uint8_t itemNum, char* nameBuffer) {

    memset(deviceIdBuffer, 0, CHATTER_DEVICE_ID_SIZE + 1);
    memset(aliasBuffer, 0, STORAGE_MAX_ALIAS_LENGTH + 1);
    memset(readableTimestampBuffer, 0, 9);

    //Serial.print("loadItemName (");Serial.print(itemNum);Serial.print("), which is slot: ");Serial.println(previewable[itemNum]);
    // get the address
    chatter->loadDeviceId (deviceSlots[itemNum], deviceIdBuffer);
    isTrustedBuffer = trustStore->loadAlias(deviceIdBuffer, aliasBuffer);
    timestampBuffer = pingTable->getLastPing(deviceSlots[itemNum]);
    rssiBuffer = pingTable->getLastRssi(deviceSlots[itemNum]);

    if (!isTrustedBuffer) {
        sprintf(aliasBuffer, "unknown");
    }

    uint8_t meshDirectRatingBuffer;
    uint8_t meshIndirectRatingBuffer;
    uint32_t meshRatingTimeBuffer;

    meshGraph->readConnectionRatings (chatter->getSelfAddress(), deviceSlots[itemNum], meshDirectRatingBuffer, meshIndirectRatingBuffer, meshRatingTimeBuffer);
    
    sprintf(readableTimestampBuffer, "%02d:%02d:%02d", hour(timestampBuffer), minute(timestampBuffer), second(timestampBuffer));

    // [0|1][00-99][0-99][0 - -99]TS[dev id][alias]
    memset(nameBuffer, 0, 6+8+CHATTER_DEVICE_ID_SIZE+STORAGE_MAX_ALIAS_LENGTH+1);
    nameBuffer[0] = isTrustedBuffer;
    nameBuffer[1] = meshDirectRatingBuffer;
    nameBuffer[2] = meshIndirectRatingBuffer;
    nameBuffer[3] = pingTable->getConnectionQualityForRssi (rssiBuffer);
    memcpy(nameBuffer+4, &rssiBuffer, 2);
    memcpy(nameBuffer+6, readableTimestampBuffer, 8);
    memcpy(nameBuffer+6+8, deviceIdBuffer, CHATTER_DEVICE_ID_SIZE);
    memcpy(nameBuffer+6+8+CHATTER_DEVICE_ID_SIZE, aliasBuffer, min(strlen(aliasBuffer), STORAGE_MAX_ALIAS_LENGTH));

    return true;
}

uint8_t NearbyDeviceIterator::getItemVal (uint8_t itemNum) {

    return deviceSlots[itemNum];
}
