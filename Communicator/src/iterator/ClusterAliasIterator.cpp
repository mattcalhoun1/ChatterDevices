#include "ClusterAliasIterator.h"

bool ClusterAliasIterator::init (const char* _clusterId, const char* _thisDeviceId, bool _sortAsc, bool _includeAllOption) {
    numClusters = clusterStore->populateClusterIndices (_sortAsc, clusterSlots);

    //Serial.print("Iterator loaded "); Serial.print(numClusters); Serial.print(" clusters ");
    return true;
}

uint8_t ClusterAliasIterator::getNumItems () {
    // find teh number of devices trusted in this cluster
    return numClusters;
}

bool ClusterAliasIterator::loadItemName (uint8_t itemNum, char* nameBuffer) {

    memset(clusterIdBuffer, 0, CHATTER_GLOBAL_NET_ID_SIZE + CHATTER_LOCAL_NET_ID_SIZE + 1 + 1);

    //Serial.print("loadItemName (");Serial.print(itemNum);Serial.print("), which is slot: ");Serial.println(trustSlots[itemNum]);

    if(clusterStore->loadClusterId (clusterSlots[itemNum], clusterIdBuffer)) {
        memset(clusterAliasBuffer, 0, STORAGE_MAX_ALIAS_LENGTH + 1);
        // Now load the alias
        if (clusterStore->loadAlias(clusterIdBuffer, clusterAliasBuffer)) {
            memcpy(nameBuffer, clusterAliasBuffer, STORAGE_MAX_ALIAS_LENGTH + 1);
            //Serial.print("Found alias: "); Serial.println(nameBuffer);
            return true;
        }
        else {
            Serial.print("Alias not found for cluster id: "); Serial.println(clusterIdBuffer);
        }
    }
    else {
        Serial.print("Cluster item name not found for slot: "); Serial.println(clusterSlots[itemNum]);
    }

    return false;
}

uint8_t ClusterAliasIterator::getItemVal (uint8_t itemNum) {

    return clusterSlots[itemNum];
}
