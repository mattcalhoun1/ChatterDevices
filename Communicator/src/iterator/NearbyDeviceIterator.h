#include "ItemIterator.h"
#include "ChatterAll.h"

#ifndef NEARBYDEVICEITERATOR_H
#define NEARBYDEVICEITERATOR_H

class NearbyDeviceIterator : public ItemIterator {
    public:
        NearbyDeviceIterator (PingTable* _pingTable, TrustStore* _trustStore, Chatter* _chatter) {pingTable = _pingTable; trustStore = _trustStore; chatter = _chatter; }
        bool init (const char* _clusterId, const char* _thisDeviceId, bool _sortAsc, bool _includeAllOption);
        uint8_t getNumItems ();
        bool loadItemName (uint8_t itemNum, char* nameBuffer);
        uint8_t getItemVal (uint8_t itemNum);
        bool isPreviewable (uint8_t itemNum) { return true; }

    protected:
        PingTable* pingTable;
        TrustStore* trustStore;
        MeshGraph* meshGraph;
        Chatter* chatter;

        uint8_t deviceSlots[CHATTER_MAX_NEARBY_DEVICES];

        uint8_t numDevices = 0;
        char deviceIdBuffer[CHATTER_DEVICE_ID_SIZE +1];
        char aliasBuffer[STORAGE_MAX_ALIAS_LENGTH + 1];

        uint8_t meshDirectRatingBuffer;
        uint8_t meshIndirectRatingBuffer;
        uint32_t meshRatingTimeBuffer;
        PingChannel channelBuffer;
        PingChannel secondaryChannelBuffer;

        int16_t rssiBuffer;
        bool isTrustedBuffer;
        uint32_t timestampBuffer;
        char readableTimestampBuffer[9]; // mm/dd hh:mm
        char thisDeviceId[CHATTER_DEVICE_ID_SIZE + 1];
};

#endif