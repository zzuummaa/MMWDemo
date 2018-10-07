//
// Created by Stephan on 21.06.2018.
//

#ifndef CALLBACKS_MMWPACKETEXTRACTOR_H
#define CALLBACKS_MMWPACKETEXTRACTOR_H

#include <cstdint>
#include "pipacketextractor.h"
#include "MMWMessage.h"

static const uint16_t requiredMagicWord[4] = {0x0102, 0x0304, 0x0506, 0x0708};
static const PIByteArray requiredMagicWordBA(requiredMagicWord, sizeof(requiredMagicWordBA));

class MMWPacketExtractor : public PIPacketExtractor {
    PIIODEVICE(PIPacketExtractor)
    friend class PIConnection;
public:
    explicit MMWPacketExtractor(PIIODevice * device_ = nullptr, SplitMode mode = None) {
        MMWMessage::Header msg;
        setHeader(PIByteArray(&msg, sizeof(msg)));
    }

protected:
    bool validateHeader(uchar *src, uchar *rec, int size) override {

        PIByteArray magicWord = PIByteArray(rec, 8);
        if (magicWord == requiredMagicWordBA) {
            return false;
        }

        auto * msg_header = reinterpret_cast<MMWMessage::Header *>(rec);
        setPayloadSize(msg_header->totalPacketLen - sizeof(msg_header));

        return true;
    }

private:

};


#endif //CALLBACKS_MMWPACKETEXTRACTOR_H
