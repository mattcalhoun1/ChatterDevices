#include "ItemIterator.h"
#include "ChatterAll.h"

#ifndef TESTITERATOR_H
#define TESTITERATOR_H

class TestIterator : public ItemIterator {
    public:
        TestIterator (uint8_t numTestItems) { numItems = numTestItems; }
        bool init (const char* _clusterId, const char* _thisDeviceId, bool _sortAsc, bool _includeAllOption);
        uint8_t getNumItems ();
        bool loadItemName (uint8_t itemNum, char* nameBuffer);
        uint8_t getItemVal (uint8_t itemNum);

    protected:
        uint8_t itemSlots[STORAGE_MAX_CLUSTER_DEVICES];
        uint8_t numItems = 0;
        char deviceIdBuffer[CHATTER_DEVICE_ID_SIZE + 1];
        char deviceAliasBuffer[STORAGE_MAX_ALIAS_LENGTH + 1];
};

#endif