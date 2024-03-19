#include "TestIterator.h"

bool TestIterator::init (const char* _clusterId) {
    //numTrusts = trustStore->populateDeviceIndices (_clusterId, trustSlots);
    //Serial.print("Iterator loaded "); Serial.print(numTrusts); Serial.print(" devices for cluster ");Serial.println(_clusterId);
}

uint8_t TestIterator::getNumItems () {
    // find teh number of devices trusted in this cluster
    return numItems;
}

bool TestIterator::loadItemName (uint8_t itemNum, char* nameBuffer) {

    memset(deviceIdBuffer, 0, CHATTER_DEVICE_ID_SIZE + 1);

    //Serial.print("loadItemName (");Serial.print(itemNum);Serial.print("), which is slot: ");Serial.println(trustSlots[itemNum]);

    memset(deviceAliasBuffer, 0, STORAGE_MAX_ALIAS_LENGTH + 1);
    memset(nameBuffer, 0, STORAGE_MAX_ALIAS_LENGTH + 1);
    sprintf(nameBuffer, "%s%02d", "Iterated_", itemNum + 500);
    return true;
}

uint8_t TestIterator::getItemVal (uint8_t itemNum) {
    // make them 1 referenced
    return itemNum + 1;
}
