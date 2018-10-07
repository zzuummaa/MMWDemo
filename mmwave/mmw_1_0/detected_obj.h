/*
 * detect_obj.h
 *
 */

#ifndef DETECT_OBJ_H_
#define DETECT_OBJ_H_

/*! @brief Maximum number of detected objects by HWA. */
#include <c++/cstdint>
#include <pibytearray.h>

#define MMW_MAX_OBJ_OUT 100

/*!
 *  @brief    Detected object estimated parameters
 *
 */
typedef struct MmwDemo_detectedObj_t
{
    uint16_t   rangeIdx;     /*!< @brief Range index */
    uint16_t   dopplerIdx;   /*!< @brief Dopler index */
    uint16_t  peakVal;      /*!< @brief Peak value */
    int16_t  x;             /*!< @brief x - coordinate in meters. Q format depends on the range resolution */
    int16_t  y;             /*!< @brief y - coordinate in meters. Q format depends on the range resolution */
    int16_t  z;             /*!< @brief z - coordinate in meters. Q format depends on the range resolution */
} MmwDemo_detectedObj;


inline PIByteArray & operator >>(PIByteArray & s, MmwDemo_detectedObj & v) {
    s >> PIByteArray::RawData(&v, sizeof(v));
    return s;
}
inline PIByteArray & operator <<(PIByteArray & s, const MmwDemo_detectedObj & v) {
    s << PIByteArray::RawData(&v, sizeof(v));
    return s;
}

#endif /* DETECT_OBJ_H_ */
