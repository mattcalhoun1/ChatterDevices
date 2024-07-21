#include <Arduino.h>

#ifndef ITEMITERATOR_H
#define ITEMITERATOR_H

class ItemIterator {
    public:
        virtual bool init (const char* filter, const char* _thisDeviceId, bool _sortAsc, bool _includeAllOption) = 0;
        virtual uint8_t getNumItems () = 0;
        virtual uint8_t getItemVal (uint8_t itemNum) = 0;
        virtual bool loadItemName (uint8_t itemNum, char* nameBuffer) = 0;
        virtual bool isPreviewable (uint8_t itemNum) { return false; }
};

#endif