/*
 * detect_obj.h
 *
 */

#ifndef DETECT_OBJ_H_
#define DETECT_OBJ_H_

/*! @brief Maximum number of detected objects by HWA. */
#define MMW_MAX_OBJ_OUT 100

#include <c++/cstdint>
#include <pibytearray.h>

/*! @brief Converts Doppler index to signed variable. Value greater than or equal
 *         half the Doppler FFT size will become negative.
 *         Needed for extended maximum velocity.
 */
#define DOPPLER_IDX_TO_SIGNED(_idx, _fftSize) ((_idx) < (_fftSize)/2 ? \
		((int16_t) (_idx)) : ((int16_t) (_idx) - (int16_t) (_fftSize)))

/*! @brief Converts signed Doppler index to unsigned variable (zero to FFT size -1).
 */
#define DOPPLER_IDX_TO_UNSIGNED(_idx, _fftSize) ((_idx) & (_fftSize - 1))

/*!
 *  @brief    Detected object estimated parameters
 *
 */
struct MmwDemo_detectedObj
{
	uint16_t   rangeIdx;     /*!< @brief Range index */
	int16_t   dopplerIdx;   /*!< @brief Doppler index. Note that it is changed
								 to signed integer in order to handle extended maximum velocity.
								 Neagative values correspond to the object moving toward
								 sensor, and positive values correspond to the
								 object moving away from the sensor */
	uint16_t  peakVal;      /*!< @brief Peak value */
	int16_t  x;             /*!< @brief x - coordinate in meters. Q format depends on the range resolution */
	int16_t  y;             /*!< @brief y - coordinate in meters. Q format depends on the range resolution */
	int16_t  z;             /*!< @brief z - coordinate in meters. Q format depends on the range resolution */
} ;

inline PIByteArray & operator >>(PIByteArray & s, MmwDemo_detectedObj & v) {
		s >> PIByteArray::RawData(&v, sizeof(v));
		return s;
}
inline PIByteArray & operator <<(PIByteArray & s, const MmwDemo_detectedObj & v) {
		s << PIByteArray::RawData(&v, sizeof(v));
		return s;
}

/*!
 *  @brief   Structure for each cluster information report .
 *
 */
struct clusteringDBscanReportForTx
{
	int16_t     xCenter;               /**< the clustering center on x direction */
	int16_t     yCenter;               /**< the clustering center on y direction */
	int16_t     xSize;                 /**< the clustering size on x direction */
	int16_t     ySize;                 /**< the clustering size on y direction */
} ;

inline PIByteArray & operator >>(PIByteArray & s, clusteringDBscanReportForTx & v) {
		s >> PIByteArray::RawData(&v, sizeof(v));
		return s;
}
inline PIByteArray & operator <<(PIByteArray & s, const clusteringDBscanReportForTx & v) {
		s << PIByteArray::RawData(&v, sizeof(v));
		return s;
}

/*!
 *  @brief   Structure for tracking report.
 *
 */
struct trackingReportForTx
{
	int16_t     x;                  /**< the tracking output -> x co-ordinate */
	int16_t     y;                  /**< the tracking output -> y co-ordinate */
	int16_t     xd;                 /**< velocity in the x direction */
	int16_t     yd;                 /**< velocity in the y direction */
	int16_t     xSize;              /**< cluster size (x direction). */
	int16_t     ySize;              /**< cluster size (y direction). */
} ;

inline PIByteArray & operator >>(PIByteArray & s, trackingReportForTx & v) {
		s >> PIByteArray::RawData(&v, sizeof(v));
		return s;
}
inline PIByteArray & operator <<(PIByteArray & s, const trackingReportForTx & v) {
		s << PIByteArray::RawData(&v, sizeof(v));
		return s;
}

#endif /* DETECT_OBJ_H_ */
