//
// Created by Stephan on 28.02.2018.
//

#ifndef MMWAVEDEMO_MMWAVEDATACONTAINER_H
#define MMWAVEDEMO_MMWAVEDATACONTAINER_H

#include "mmw/detected_obj.h"
#include "mmw/mmw_messages.h"
#include "mmw/sys_common.h"

enum MmwDemo_output_message_type_srr {
	MMWDEMO_UART_MSG_DETECTED_POINTS = 1,
	MMWDEMO_UART_MSG_CLUSTERS,
	MMWDEMO_UART_MSG_TRACKED_OBJ,
	MMWDEMO_UART_MSG_AZIMUT_STATIC_HEAT_MAP,
	MMWDEMO_UART_MSG_RANGE_DOPPLER_HEAT_MAP,
	MMWDEMO_UART_MSG_STATS,
	ODDEMO_UART_MSG_RANGE_AZIMUT_HEAT_MAP = 8,
	ODDEMO_UART_MSG_FEATURE_VECTOR,
	ODDEMO_UART_MSG_DECISION,
	MMWDEMO_UART_MSG_MAX
};


class MMWaveDataContainer {
public:
	enum Type {
		Invalid,
		Near,
		Far
	};

	MMWaveDataContainer() {type = Invalid;}

	bool fromTLV(MmwDemo_msgTlv* tlv);

	PIVector<MmwDemo_detectedObj> detObjs;
	PIVector<clusteringDBscanReportForTx> clusterReport;
	PIVector<trackingReportForTx> trackReport;
	PIVector<cmplx16ImRe_t_> azimuthStaticHeatMap;
	PIVector<uint16_t> rangeDopplerHeatMap;
	PIVector<float> azimuthRangeHeatMap;
	int decision;
	Type type;
};

#endif //MMWAVEDEMO_MMWAVEDATACONTAINER_H
