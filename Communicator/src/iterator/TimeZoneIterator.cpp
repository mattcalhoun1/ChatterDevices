#include "TimeZoneIterator.h"


bool TimeZoneIterator::loadItemName (uint8_t itemNum, char* nameBuffer) {
    if (itemNum < DEVICE_STORE_NUM_TIME_ZONES) {
        sprintf(nameBuffer, zoneNames[itemNum]);
        return true;
    }
    return false;
}

uint8_t TimeZoneIterator::getItemVal (uint8_t itemNum) {
    return itemNum;
}