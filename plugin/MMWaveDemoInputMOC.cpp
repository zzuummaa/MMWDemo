//
// Created by Stephan on 11.09.2018.
//

#include <MMWaveDataContainer.h>
#include "MMWaveDemoInputMOC.h"

REGISTER_BUS_TYPE(int16_t)
REGISTER_BUS_TYPE(uint16_t)
REGISTER_BUS_TYPE(MmwDemo_detectedObj)
REGISTER_BUS_TYPE(clusteringDBscanReportForTx)
REGISTER_BUS_TYPE(trackingReportForTx)

void MMWaveDemoInputMOC::start(SMBrickBase *block) {
    piCoutObj << "start";

    //msleep(5000);
}

void MMWaveDemoInputMOC::stop(SMBrickBase *block) {
    //piCoutObj << "MMWaveDemoInput stop";
}

PIVector<SMBase::BlockPin> MMWaveDemoInputMOC::pinsOut(const PIPropertyStorage &props) {
    PIVector<SMBase::BlockPin> ret;
    ret << BLOCKPIN_FROMTYPE_NAME(PIVector<MmwDemo_detectedObj>, "DetObjFrame");
    ret << BLOCKPIN_FROMTYPE_NAME(PIVector<clusteringDBscanReportForTx>, "ClusterReport");
    ret << BLOCKPIN_FROMTYPE_NAME(PIVector<trackingReportForTx>, "TrackingReport");
    ret << BLOCKPIN_FROMTYPE_NAME(PIVector<PIVector<float>>, "azimuthRangeHeatMap");
    ret << BLOCKPIN_FROMTYPE_NAME(PIVector<PIVector<complexd>>, "azimuthStaticHeatMap");
    ret << BLOCKPIN_FROMTYPE_NAME(PIVector<PIVector<float>>, "rangeDopplerHeatMap");
    ret << BLOCKPIN_FROMTYPE_NAME(int, "type");
    ret << BLOCKPIN_FROMTYPE_NAME(PIString, "decision");
    return ret;
}

void MMWaveDemoInputMOC::get(SMBrickBase *block, SMBlockData &out) {
    MMWaveDataContainer container;
    container.type = container.Near;

    MmwDemo_detectedObj detectedObj1 = {1, 1, 1, 12, 12, 0};
    container.detObjs.push_back(detectedObj1);

    trackingReportForTx track1 = {8, 8, 2, 2, 4, 4};
    container.trackReport.push_back(track1);

    PIVector<PIVector<float>> convertedRangeDopplerMap;
    for (int i = 0; i < 256; ++i) {
        PIVector<float> vec;
        for (int j = 0; j < 64; ++j) {
            vec.append(i * 256 + j);
        }
        convertedRangeDopplerMap.append(vec);
    }

    out[0] = SMBusData::create(container.detObjs);
    out[1] = SMBusData::create(container.clusterReport);
    out[2] = SMBusData::create(container.trackReport);
    out[6] = SMBusData::create((int)container.type);
    out[5] = SMBusData::create(convertedRangeDopplerMap);

    block->addProperty("isEnd", true);
}

bool MMWaveDemoInputMOC::isEnd(const SMBrickBase *block) {
    return block->propertyByName("isEnd").value.toBool();
}

bool MMWaveDemoInputMOC::isReady(const SMBrickBase *block) {
    return true;
}