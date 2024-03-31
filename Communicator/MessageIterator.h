#include "ItemIterator.h"
#include "ChatterAll.h"

#ifndef MESSAGEITERATOR_H
#define MESSAGEITERATOR_H

class MessageIterator : public ItemIterator {
    public:
        MessageIterator (MessageStore* _messageStore, TrustStore* _trustStore) {messageStore = _messageStore; trustStore = _trustStore; }
        bool init (const char* _clusterId, const char* _thisDeviceId, bool _sortAsc);
        uint8_t getNumItems ();
        bool loadItemName (uint8_t itemNum, char* nameBuffer);
        uint8_t getItemVal (uint8_t itemNum);
        bool isPreviewable (uint8_t itemNum);

    protected:
        MessageStore* messageStore;
        TrustStore* trustStore;

        uint8_t messageSlots[STORAGE_MAX_STORED_MESSAGES];
        bool previewable[STORAGE_MAX_STORED_MESSAGES];
        
        uint8_t numMessages = 0;
        char messageIdBuffer[CHATTER_MESSAGE_ID_SIZE +1];
        char senderIdBuffer[CHATTER_DEVICE_ID_SIZE + 1];
        char recipientIdBuffer[CHATTER_DEVICE_ID_SIZE + 1];
        char aliasBuffer[STORAGE_MAX_ALIAS_LENGTH + 1];

        MessageSendMethod sendMethodBuffer;
        MessageStatus statusBuffer;
        char timestampBuffer[STORAGE_TS_SIZE+1];
        char thisDeviceId[CHATTER_DEVICE_ID_SIZE + 1];
};

#endif