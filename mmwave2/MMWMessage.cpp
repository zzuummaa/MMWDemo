//
// Created by Stephan on 21.06.2018.
//

#include "MMWMessage.h"

TLV::TLV(const uint8_t *tlv_, int maxLength) {
    int tlvLen = extractTLVLength(tlv_);
    if (tlvLen <= maxLength) {
        buff.append(tlv_, maxLength);
        header = (Header *) buff.data(0);
        data = buff.data(sizeof(Header));
    } else {
        header = nullptr;
        data = nullptr;
    }
}

TLV::TLV(const PIByteArray &tlv_): TLV(tlv_.data(), tlv_.length()) {}

/**
 * Add data from pointer to vector
 *
 * @param T - type of data
 * @param vec - vector to which data will be added
 * @param p - pointer to data begin
 * @param dataCount - count of data in pointer
 */
template<typename T>
inline void addData(PIVector<T>& vec, uint8_t* p, int dataCount) {
    for (int i = 0; i < dataCount; ++i) {
        T* data = (T*)p;
        vec.push_back(*data);
        p += sizeof(T);
    }
}

template<typename T>
inline void TLV::extractObjs(PIVector<T> &vec) {
    uint8_t* p = data;
    p += sizeof(DataObjDescr);
    int num = extractNumDetectObjs(data);
    if (num * sizeof(T) + sizeof(DataObjDescr) == header->length) {
        addData(vec, p, num);
    }
}

PIVector<TLV::DetObj> TLV::asDetects() {
    PIVector<TLV::DetObj> detObjs;
    if (type() == TLV::Type::DETECTED_POINTS) {
        extractObjs(detObjs);
    }
    return detObjs;
}

PIVector<TLV::ClusterReport> TLV::asClusteringReport() {
    PIVector<TLV::ClusterReport> clusteringReport;
    if (type() == TLV::Type::CLUSTERS) {
        extractObjs(clusteringReport);
    }
    return clusteringReport;
}

PIVector<TLV::TrackReport> TLV::asTrackingReport() {
    PIVector<TLV::TrackReport> trackingReport;
    if (type() == TLV::Type::TRACKED_OBJ) {
        extractObjs(trackingReport);
    }
    return trackingReport;
}

TLV::~TLV() {
    header = nullptr;
    data = nullptr;
}

MMWMessage::MMWMessage(const uint8_t *mmwMessage) {
    init(mmwMessage);
}

MMWMessage::MMWMessage(const PIByteArray &mmwMessage) {
    init(mmwMessage.data(0));
}

inline void MMWMessage::init(const uint8_t *mmwMessage) {
    memcpy(&header, mmwMessage, sizeof(MMWMessage::Header));
    auto p = const_cast<uint8_t *>(mmwMessage + sizeof(MMWMessage::Header));

    for (int i = 0; i < header.numTLVs; ++i) {
        TLV tlv = TLV(p, header.totalPacketLen);
        p += tlv.length();
        tlvs.push_back(tlv);
    }
}

const MMWMessage::Header &MMWMessage::getHeader() const {
    return header;
}

const PIVector<TLV> &MMWMessage::getTlvs() const {
    return tlvs;
}
