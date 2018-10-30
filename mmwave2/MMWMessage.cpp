//
// Created by Stephan on 21.06.2018.
//

#include <stdexcept>
#include <mmw/sys_common.h>
#include "MMWMessage.h"

TLV::TLV(const uint8_t *tlv_, int maxLength) {
    // TODO check situation when maxLength < sizeof(TLV::Header)
    int tlvLen;
    if (maxLength >= sizeof(TLV::Header) && tlv_ != nullptr) {
        tlvLen = extractTLVLength(tlv_);
    } else {
        header = nullptr;
        data = nullptr;
        return;
    }

    if (tlvLen <= maxLength) {
        buff.append(tlv_, tlvLen);
        header = (Header *) buff.data(0);
        data = buff.data(sizeof(Header));
    } else {
        header = nullptr;
        data = nullptr;
    }
}

TLV::TLV(const PIByteArray &tlv_): TLV(tlv_.data(), tlv_.length()) {}

TLV::TLV() {
    header = nullptr;
    data = nullptr;
}

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

template<typename R, typename T>
void TLV::extractDataToMat(T* data, PIVector<PIVector<R>>& dst, int rows, int cols, std::function<R(T*)> convert) {
    if (rows == 0 || cols == 0) {
        throw std::invalid_argument("vecToMat has zero args rows & cols.");
    }

    T* p = data;
    for (int i = 0; i < rows; ++i) {
        PIVector<R> row;
        for (int j = 0; j < cols; ++j) {
            row.push_back(convert(p));
            p++;
        }
        dst.push_back(row);
    }
}

PIVector<PIVector<complexd>> TLV::asAzimuthStaticHeatmap(int rangeFFTSize, int numOfVirtAntennas) {
    PIVector<PIVector<complexd>> heatMap;
    uint32_t requiredDataLen = rangeFFTSize * numOfVirtAntennas * sizeof(cmplx16ImRe_t);
    if (type() == TLV::Type::AZIMUT_STATIC_HEAT_MAP && requiredDataLen == header->length) {
        std::function<complexd(cmplx16ImRe_t*)> convFun = [](cmplx16ImRe_t* src) -> complexd { return complexd(src->real, src->imag); };
        extractDataToMat((cmplx16ImRe_t*)data, heatMap, rangeFFTSize, numOfVirtAntennas, convFun);
    }
    return heatMap;
}

PIVector<PIVector<float>> TLV::asRangeDopplerHeatmap(int rangeFFTSize, int dopplerFFTSize) {
    PIVector<PIVector<float>> heatMap;
    uint32_t requiredDataLen = rangeFFTSize * dopplerFFTSize * sizeof(uint16_t);
    if (type() == TLV::Type::RANGE_DOPPLER_HEAT_MAP && requiredDataLen == header->length) {
        std::function<float(uint16_t*)> convFun = [](uint16_t* src) -> float { return (float)*src; };
        extractDataToMat((uint16_t*)data, heatMap, rangeFFTSize, dopplerFFTSize, convFun);
    }
    return heatMap;
}

PIVector<PIVector<float>> TLV::asRangeAzimuthHeatmap(int rangeFFTSize, int numOfVirtAntennas) {
    PIVector<PIVector<float>> heatMap;
    uint32_t requiredDataLen = rangeFFTSize * numOfVirtAntennas * sizeof(cmplx16ImRe_t);
    if (type() == TLV::Type::RANGE_AZIMUT_HEAT_MAP && requiredDataLen == header->length) {
        std::function<float(cmplx16ImRe_t*)> convFun = [](cmplx16ImRe_t* src) -> float { return (float)abs(complexd(src->real, src->imag)); };
        extractDataToMat((cmplx16ImRe_t*)data, heatMap, rangeFFTSize, numOfVirtAntennas, convFun);
    }
    return heatMap;
}

TLV::~TLV() {
    header = nullptr;
    data = nullptr;
}

MMWMessage::MMWMessage(const uint8_t *mmwMessage, int maxSize) {
    isValidPacket_ = false;
    if (maxSize >= sizeof(MMWMessage::Header) && mmwMessage != nullptr) {
        if (maxSize >= extractTotalPacketLength(mmwMessage)) {
            isValidPacket_ = true;
        }
    }

    if (isValidPacket_) {
        memcpy(&header, mmwMessage, sizeof(MMWMessage::Header));
        auto p = const_cast<uint8_t *>(mmwMessage + sizeof(MMWMessage::Header));

        int parsedBytes = 0;
        for (int i = 0; i < header.numTLVs; ++i) {
            TLV tlv = TLV(p + parsedBytes, header.totalPacketLen - parsedBytes - sizeof(MMWMessage::Header));
            if (tlv.type() == TLV::Type::INVALID) {
                isValidPacket_ = false;
                tlvs.clear();
                break;
            }
            parsedBytes += tlv.length();
            tlvs.push_back(tlv);
        }
    }

    if (!isValidPacket_) memset(&header, 0, sizeof(MMWMessage::Header));

}

MMWMessage::MMWMessage(const PIByteArray &mmwMessage): MMWMessage(mmwMessage.data(), mmwMessage.length()) {}

MMWMessage::MMWMessage(): MMWMessage(nullptr, 0) {}

const MMWMessage::Header &MMWMessage::getHeader() const {
    return header;
}

const PIVector<TLV> &MMWMessage::getTlvs() const {
    return tlvs;
}

bool MMWMessage::isValidPacket() const {
    return isValidPacket_;
}
