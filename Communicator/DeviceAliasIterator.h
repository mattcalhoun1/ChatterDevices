#include "ItemIterator.h"
#include "ChatterAll.h"

#ifndef DEVICEALIASITERATOR_H
#define DEVICEALIASITERATOR_H

class DeviceAliasIterator : public ItemIterator {
    public:
        DeviceAliasIterator (TrustStore* _trustStore) {trustStore = _trustStore; }
        bool init (const char* _clusterId);
        uint8_t getNumItems ();
        bool loadItemName (uint8_t itemNum, char* nameBuffer);
        uint8_t getItemVal (uint8_t itemNum);

    protected:
        TrustStore* trustStore;
        uint8_t trustSlots[STORAGE_MAX_CLUSTER_DEVICES];
        uint8_t numTrusts = 0;
        char deviceIdBuffer[CHATTER_DEVICE_ID_SIZE + 1];
        char deviceAliasBuffer[STORAGE_MAX_ALIAS_LENGTH + 1];
};

#endif