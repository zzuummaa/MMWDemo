//
// Created by Stephan on 21.06.2018.
//

#ifndef CALLBACKS_MMWDATA_H
#define CALLBACKS_MMWDATA_H

#include <cstdint>
#include "pibytearray.h"

int extractTLVLength(const uint8_t *tlv);
int extractNumDetectObjs(const uint8_t *tlvData);

class TLV {
public:

    /**
     * @note Constructor not parse payload.
     * @note Copies input bytes to local buffer.
     * @note Initialized as empty TLV if tlv_ length smaller then (sizeof(TLV::Header) + header.length)
     *
     * @param tlv_ tlv bytes of TLV packet (TLV::Header + TLV payload)
     */
    explicit TLV(const PIByteArray &tlv_);
    explicit TLV(const uint8_t *tlv_, int maxLength);
    explicit TLV();
    TLV(const TLV &tlv): TLV(tlv.buff) {}
    ~TLV();

    enum Type {
        DETECTED_POINTS = 1,
        CLUSTERS,
        TRACKED_OBJ,
        AZIMUT_STATIC_HEAT_MAP,
        RANGE_DOPPLER_HEAT_MAP,
        MSG_STATS,
        RANGE_AZIMUT_HEAT_MAP = 8,
        FEATURE_VECTOR,
        DECISION,
        INVALID
    };

    struct Header {
        /*! @brief   TLV type */
        union {
            uint32_t typeInt;
            TLV::Type type;
        };


        /*! @brief   Length in bytes */
        uint32_t length;
    };


    /*!
     * @brief
     * Structure holds information about detected objects.
     *
     * @details
     * This information is sent in front of the array of detected objects
     */
    struct DataObjDescr {
        /*! @brief   Number of detected objects */
        uint16_t numDetetedObj;

        /*! @brief   Q format of detected objects x/y/z coordinates */
        uint16_t xyzQFormat;

    };


    struct DetObj {
        uint16_t rangeIdx;     /*!< @brief Range index */
        int16_t dopplerIdx;   /*!< @brief Doppler index. Note that it is changed
								 to signed integer in order to handle extended maximum velocity.
								 Neagative values correspond to the object moving toward
								 sensor, and positive values correspond to the
								 object moving away from the sensor */
        uint16_t peakVal;      /*!< @brief Peak value */
        int16_t x;             /*!< @brief x - coordinate in meters. Q format depends on the range resolution */
        int16_t y;             /*!< @brief y - coordinate in meters. Q format depends on the range resolution */
        int16_t z;             /*!< @brief z - coordinate in meters. Q format depends on the range resolution */
    };


    /*!
     *  @brief   Structure for each cluster information report .
     *
     */
    struct ClusterReport
    {
        int16_t     xCenter;               /**< the clustering center on x direction */
        int16_t     yCenter;               /**< the clustering center on y direction */
        int16_t     xSize;                 /**< the clustering size on x direction */
        int16_t     ySize;                 /**< the clustering size on y direction */
    } ;


    /*!
     *  @brief   Structure for tracking report.
     *
     */
    struct TrackReport {
        int16_t x;                  /**< the tracking output -> x co-ordinate */
        int16_t y;                  /**< the tracking output -> y co-ordinate */
        int16_t xd;                 /**< velocity in the x direction */
        int16_t yd;                 /**< velocity in the y direction */
        int16_t xSize;              /**< cluster size (x direction). */
        int16_t ySize;              /**< cluster size (y direction). */
    };

    inline TLV::Type type() {
        return header != nullptr ? header->type : TLV::Type::INVALID;
    }

    /**
     * @return full TLV length (payload size) + (TLV header length) in bytes
     */
    inline int length() {
        return header != nullptr ? header->length + sizeof(Header) : 0;
    }

    /**
     * @return vector of detect objects or zero size vector if TLV type is not TLV::Type::DETECTED_POINTS
     */
    PIVector<DetObj> asDetects();

    PIVector<ClusterReport> asClusteringReport();
    PIVector<TrackReport> asTrackingReport();

private:
    PIByteArray buff;
    Header* header;
    uint8_t* data;

    template<typename T>
    inline void extractObjs(PIVector<T>& vec);
};

/**
 * Extract TLV length from TLV header and return (payload size) + (TLV header length) in bytes
 */
inline int extractTLVLength(const uint8_t *tlv) {
    return ((TLV::Header *) tlv)->length + sizeof(TLV::Header);
}

inline int extractNumDetectObjs(const uint8_t *tlvData) {
    return ((TLV::DataObjDescr*)tlvData)->numDetetedObj;
}

inline int extractTotalPacketLength(const uint8_t *mmwMessageHeader);

class MMWMessage {
public:
    explicit MMWMessage(const uint8_t *mmwMessage, int maxSize);
    explicit MMWMessage(const PIByteArray& mmwMessage);
    explicit MMWMessage();

    /*!
     * @brief
     *  Message header for reporting detection information from data path.
     *
     * @details
     *  The structure defines the message header.
     */
    struct Header {
        /*! @brief   Output buffer magic word (sync word). It is initialized to  {0x0102,0x0304,0x0506,0x0708} */
        uint16_t magicWord[4];

        /*! brief   Version: : MajorNum * 2^24 + MinorNum * 2^16 + BugfixNum * 2^8 + BuildNum   */
        uint32_t version;

        /*! @brief   Total packet length including header in Bytes */
        uint32_t totalPacketLen;

        /*! @brief   platform type */
        uint32_t platform;

        /*! @brief   Frame number */
        uint32_t frameNumber;

        /*! @brief   Time in CPU cycles when the message was created. For XWR16xx: DSP CPU cycles, for XWR14xx: R4F CPU cycles */
        uint32_t timeCpuCycles;

        /*! @brief   Number of detected objects */
        uint32_t numDetectedObj;

        /*! @brief   Number of TLVs */
        uint32_t numTLVs;

        /*! @brief   For Advanced Frame config, this is the sub-frame number in the range
         * 0 to (number of subframes - 1). For frame config (not advanced), this is always
         * set to 0. */
        uint32_t subFrameNumber;
    };

    const Header &getHeader() const;

    const PIVector<TLV> &getTlvs() const;

    bool isValidPacket() const;

private:
    MMWMessage::Header header;
    PIVector<TLV> tlvs;
    bool isValidPacket_;
};

const uint16_t requiredMagicWord[4] = {0x0102, 0x0304, 0x0506, 0x0708};

inline bool isValidMagicWorld(const uint8_t *magicWorld) {
    return memcmp(requiredMagicWord, magicWorld, sizeof(requiredMagicWord)) == 0;
}

inline int extractTotalPacketLength(const uint8_t *mmwMessageHeader) {
    return ((MMWMessage::Header*)mmwMessageHeader)->totalPacketLen;
}

#endif //CALLBACKS_MMWDATA_H
