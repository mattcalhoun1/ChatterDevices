#include "ItemIterator.h"
#include "ChatterAll.h"

#ifndef DEVICEALIASITERATOR_H
#define DEVICEALIASITERATOR_H

#define DEVICE_SELECTION_ALL 254

class DeviceAliasIterator : public ItemIterator {
    public:
        DeviceAliasIterator (TrustStore* _trustStore) {trustStore = _trustStore; }
        bool init (const char* _clusterId, const char* _thisDeviceId, bool _sortAsc, bool _includeAllOption);
        uint8_t getNumItems ();
        bool loadItemName (uint8_t itemNum, char* nameBuffer);
        uint8_t getItemVal (uint8_t itemNum);

    protected:
        TrustStore* trustStore;
        uint8_t trustSlots[STORAGE_MAX_CLUSTER_DEVICES + 1];
        uint8_t numTrusts = 0;
        char deviceIdBuffer[CHATTER_DEVICE_ID_SIZE + 1];
        char deviceAliasBuffer[STORAGE_MAX_ALIAS_LENGTH + 1];
        bool includeAllOption = false;
};

#endif