#include "MessageIterator.h"

bool MessageIterator::init (const char* _clusterId, const char* _thisDeviceId, bool _sortAsc, bool _includeAllOption) {
    return init(_clusterId, _thisDeviceId, _thisDeviceId, _sortAsc);
}

bool MessageIterator::init (const char* _clusterId, const char* _thisDeviceId, const char* _filterDeviceId, bool _sortAsc) {
    memcpy(thisDeviceId, _thisDeviceId, CHATTER_DEVICE_ID_SIZE);
    thisDeviceId[CHATTER_DEVICE_ID_SIZE] = 0;

    memcpy(filterDeviceId, _filterDeviceId, CHATTER_DEVICE_ID_SIZE);
    filterDeviceId[CHATTER_DEVICE_ID_SIZE] = 0;

    numMessages = messageStore->populateMessageIndices (_clusterId, filterDeviceId, messageSlots, _sortAsc);
    //Serial.print("Iterator loaded "); Serial.print(numMessages); Serial.print(" messages for cluster ");Serial.println(_clusterId);
    return true;
}

uint8_t MessageIterator::getNumItems () {
    return numMessages;
}

bool MessageIterator::isPreviewable (uint8_t itemNum) {
    return previewable[itemNum];
}

bool MessageIterator::loadItemName (uint8_t itemNum, char* nameBuffer) {

    memset(senderIdBuffer, 0, CHATTER_DEVICE_ID_SIZE + 1);
    memset(recipientIdBuffer, 0, CHATTER_DEVICE_ID_SIZE + 1);
    memset(messageIdBuffer, 0, CHATTER_MESSAGE_ID_SIZE + 1);
    memset(timestampBuffer, 0, STORAGE_TS_SIZE + 1);
    memset(aliasBuffer, 0, STORAGE_MAX_ALIAS_LENGTH + 1);
    memset(readableTimestampBuffer, 0, 12);

    //Serial.print("loadItemName (");Serial.print(itemNum);Serial.print("), which is slot: ");Serial.println(previewable[itemNum]);
    bool isLarge = messageStore-> loadMessageDetails (messageSlots[itemNum], senderIdBuffer, recipientIdBuffer, messageIdBuffer, timestampBuffer, statusBuffer, sendMethodBuffer, messageTypeBuffer);
    bool thisDeviceSent = memcmp(thisDeviceId, senderIdBuffer, CHATTER_DEVICE_ID_SIZE) == 0;

    memcpy(readableTimestampBuffer, timestampBuffer + 2, 2);
    readableTimestampBuffer[2] = '/';
    memcpy(readableTimestampBuffer+3, timestampBuffer + 4, 2);
    readableTimestampBuffer[5] = ' ';
    memcpy(readableTimestampBuffer + 6, timestampBuffer + 6, 2);
    readableTimestampBuffer[8] = ':';
    memcpy(readableTimestampBuffer + 9, timestampBuffer + 8, 2);

    previewable[itemNum] = !isLarge;

    // send type, status, large/small indicator, timestamp, sender/recipient, " << or >> "
    // the first 3 fields are not really intended to directly display, but rather
    // to allow for color coding/etc to indicate hwo they should be shown

    // [D|B|I][R|N|][L|S][type]TS [SENDER/RECIPIENT alias]([Sender/rcp id]) [ >> or << ]
    nameBuffer[0] = (char)sendMethodBuffer;
    nameBuffer[1] = (char)statusBuffer;
    nameBuffer[2] = isLarge ? 'L' : 'S';
    nameBuffer[3] = (char)messageTypeBuffer;

    // if it was a sent broadcast, the recipient is everyone (*)
    if (thisDeviceSent && sendMethodBuffer == SentViaBroadcast && memcmp(recipientIdBuffer, chatter->getClusterBroadcastId(), CHATTER_DEVICE_ID_SIZE) == 0) {
        sprintf(aliasBuffer, "%s", "[all]");
    }
    else {
        /*Serial.print("iterator device id: ");
        for (uint8_t i = 0; i < CHATTER_DEVICE_ID_SIZE; i++) {
            Serial.print((char)(thisDeviceSent ? recipientIdBuffer[i] : senderIdBuffer[i]));
        }
        Serial.println("");*/
        // load the alias of the other device
        trustStore->loadAlias(thisDeviceSent ? recipientIdBuffer : senderIdBuffer, aliasBuffer);
    }

    sprintf(nameBuffer + 4, "%s%s:%s", thisDeviceSent ? "> " : "< ", readableTimestampBuffer, aliasBuffer);
    return true;
}

uint8_t MessageIterator::getItemVal (uint8_t itemNum) {

    return messageSlots[itemNum];
}
