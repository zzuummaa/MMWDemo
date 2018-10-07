//
// Created by Stephan on 18.07.2018.
//

#ifndef CALLBACKS_UTILS_H
#define CALLBACKS_UTILS_H

#include "MMWMessage.h"

const uint16_t magicWord[] = {0x0102, 0x0304, 0x0506, 0x0708};

PIByteArray generateMMWPacket() {
    MMWMessage::Header header;
    memset(&header, 0, sizeof(header));
    memcpy(header.magicWord, &magicWord, sizeof(header.magicWord));
    header.numTLVs = 1;

    TLV::DetObj detObj;
    detObj.x = 5;
    detObj.y = 10;
    detObj.dopplerIdx = 1;
    detObj.peakVal = 3;
    detObj.rangeIdx = 2;

    TLV::Header tlvHeader;
    tlvHeader.type = TLV::Type::MMWDEMO_OUTPUT_MSG_DETECTED_POINTS;
    tlvHeader.length = sizeof(TLV::DetObj) + sizeof(TLV::DataObjDescr);

    TLV::DataObjDescr dataObjDescr;
    dataObjDescr.numDetetedObj = 1;
    dataObjDescr.xyzQFormat = 1;

    PIByteArray data;
    data.append(&header, sizeof(header));
    data.append(&tlvHeader, sizeof(tlvHeader));
    data.append(&dataObjDescr, sizeof(dataObjDescr));
    data.append(&detObj, sizeof(detObj));

    int paddingLen = static_cast<int>(data.size_s() % 32);
    paddingLen = paddingLen > 0 ? 32 - paddingLen : 0;
    uint8_t padding[paddingLen];
    data.append(padding, paddingLen);
    return data;
}

#endif //CALLBACKS_UTILS_H
