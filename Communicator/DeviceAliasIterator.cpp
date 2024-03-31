#include "DeviceAliasIterator.h"

bool DeviceAliasIterator::init (const char* _clusterId, const char* _thisDeviceId, bool _sortAsc) {
    numTrusts = trustStore->populateDeviceIndices (_clusterId, _thisDeviceId, _sortAsc, trustSlots);

    //Serial.print("Iterator loaded "); Serial.print(numTrusts); Serial.print(" devices for cluster ");Serial.println(_clusterId);
}

uint8_t DeviceAliasIterator::getNumItems () {
    // find teh number of devices trusted in this cluster
    return numTrusts;
}

bool DeviceAliasIterator::loadItemName (uint8_t itemNum, char* nameBuffer) {

    memset(deviceIdBuffer, 0, CHATTER_DEVICE_ID_SIZE + 1);

    //Serial.print("loadItemName (");Serial.print(itemNum);Serial.print("), which is slot: ");Serial.println(trustSlots[itemNum]);

    if(trustStore->loadDeviceId (trustSlots[itemNum], deviceIdBuffer)) {
        memset(deviceAliasBuffer, 0, STORAGE_MAX_ALIAS_LENGTH + 1);
        // Now load the alias
        if (trustStore->loadAlias(deviceIdBuffer, deviceAliasBuffer)) {
            memcpy(nameBuffer, deviceAliasBuffer, STORAGE_MAX_ALIAS_LENGTH + 1);
            //Serial.print("Found alias: "); Serial.println(nameBuffer);
            return true;
        }
    }

    Serial.print("Device item name not found for slot: "); Serial.println(trustSlots[itemNum]);
    return false;
}

uint8_t DeviceAliasIterator::getItemVal (uint8_t itemNum) {

    return trustSlots[itemNum];
}
