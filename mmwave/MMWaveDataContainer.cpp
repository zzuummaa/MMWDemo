//
// Created by Stephan on 28.02.2018.
//

#include "MMWaveDataContainer.h"

/**
 * Add data from pointer to vector
 *
 * @tparam T - type of data
 * @param vec - vector to which data will be added
 * @param p - pointer to data begin
 * @param dataCount - count of data in pointer
 */
template<typename T>
void addData(PIVector<T>& vec, uint8_t* p, uint32_t dataCount) {
	for (int i = 0; i < dataCount; ++i) {
		T* data = (T*)p;
		vec.push_back(*data);
		p += sizeof(T);
	}
}

bool MMWaveDataContainer::fromTLV(MmwDemo_msgTlv* tlv) {
	uint8_t* p = reinterpret_cast<uint8_t *>(tlv->address);
	uint32_t detObjCount = ((MmwDemo_output_message_dataObjDescr*)p)->numDetetedObj;
	//piCout << tlv->type << tlv->length;
	switch (tlv->type) {
		case MMWDEMO_UART_MSG_DETECTED_POINTS:
			p += sizeof(MmwDemo_output_message_dataObjDescr);
			addData(detObjs, p, detObjCount);
			break;
		case MMWDEMO_UART_MSG_CLUSTERS:
			p += sizeof(MmwDemo_output_message_dataObjDescr);
			addData(clusterReport, p, detObjCount);
			break;
		case MMWDEMO_UART_MSG_TRACKED_OBJ:
			p += sizeof(MmwDemo_output_message_dataObjDescr);
			addData(trackReport, p, detObjCount);
			break;
		case MMWDEMO_UART_MSG_AZIMUT_STATIC_HEAT_MAP:
			addData(azimuthStaticHeatMap, p, tlv->length/sizeof(cmplx16ImRe_t_));
			break;
		case MMWDEMO_UART_MSG_RANGE_DOPPLER_HEAT_MAP:
			addData(rangeDopplerHeatMap, p, tlv->length/sizeof(uint16_t));
		case ODDEMO_UART_MSG_RANGE_AZIMUT_HEAT_MAP:
			addData(azimuthRangeHeatMap, p, tlv->length/sizeof(cmplx16ImRe_t_));
			break;
		case ODDEMO_UART_MSG_DECISION:
			decision = *((int*)p);
			break;
		default: return false;
	}
	return true;
}
