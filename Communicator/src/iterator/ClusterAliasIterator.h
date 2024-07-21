#ifndef CLUSTERALIASITERATOR_H
#define CLUSTERALIASITERATOR_H

#include "ItemIterator.h"
#include "ChatterAll.h"

class ClusterAliasIterator : public ItemIterator {
    public:
        ClusterAliasIterator (ClusterStore* _clusterStore) {clusterStore = _clusterStore; }
        bool init (const char* _clusterId, const char* _thisDeviceId, bool _sortAsc, bool _includeAllOption);
        uint8_t getNumItems ();
        bool loadItemName (uint8_t itemNum, char* nameBuffer);
        uint8_t getItemVal (uint8_t itemNum);

    protected:
        ClusterStore* clusterStore;
        uint8_t clusterSlots[STORAGE_MAX_CLUSTERS];
        uint8_t numClusters = 0;
        char clusterIdBuffer[CHATTER_GLOBAL_NET_ID_SIZE + CHATTER_LOCAL_NET_ID_SIZE + 1];
        char clusterAliasBuffer[STORAGE_MAX_ALIAS_LENGTH + 1];
};

#endif