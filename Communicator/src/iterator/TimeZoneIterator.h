#include "ItemIterator.h"
#include "ChatterAll.h"

#ifndef TIMEZONEITERATOR_H
#define TIMEZONEITERATOR_H

class TimeZoneIterator : public ItemIterator {
    public:
        TimeZoneIterator () {}
        bool init (const char* _clusterId, const char* _thisDeviceId, bool _sortAsc, bool _includeAllOption) { return true; }
        uint8_t getNumItems () { return DEVICE_STORE_NUM_TIME_ZONES; }

        bool loadItemName (uint8_t itemNum, char* nameBuffer);
        uint8_t getItemVal (uint8_t itemNum);

    protected:
        // max menu item name size is currently 16 (set in menu class)
        const char* zoneNames[DEVICE_STORE_NUM_TIME_ZONES] = {
            "London",
            "Paris/Berlin",
            "Cairo",
            "Jeddah",
            "Dubai",
            "Karachi",
            "Dhaka",
            "Bankok",
            "Beijing",
            "Tokyo",
            "Sydney",
            "Noumea",
            "Wellington",
            "Tongatapu",
            "Midway Island",
            "Honolulu",
            "Anchorage",
            "Los Angeles",
            "Denver",
            "Chicago",
            "New York",
            "Santa Domingo",
            "Rio de Janeiro",
            "Fernando de...",
            "Azores Islands"                
        };
};

#endif