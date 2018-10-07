//
// Created by Stephan on 01.11.2017.
//

#ifndef MMWAVEDEMO_MMWAVEDEMOPROPERTYES_H
#define MMWAVEDEMO_MMWAVEDEMOPROPERTYES_H

#include <cstdint>
#include <cstring>
#include <pistring.h>
#include <sm_base.h>

static const PIString propertiesNamesArr[] = {
        "dfeDataOutputMode",
        "channelCfg",
        "adcCfg",
        "adcbufCfg",
        "profileCfg",
        "chirpCfg0",
        "chirpCfg1",
        "bpmCfg",
        "frameCfg",
        "lowPower",
        "guiMonitor",
        "cfarCfg0",
        "cfarCfg1",
        "peakGrouping",
        "multiObjBeamForming",
        "calibDcRangeSig",
        "extendedMaxVelocity",
        "clutterRemoval",
        "compRangeBiasAndRxChanPhase",
        "measureRangeBiasAndRxChanPhase",
        "nearFieldCfg",
        "CQRxSatMonitor",
        "CQSigImgMonitor",
        "analogMonitor",
        "lvdsStreamCfg"
};

static const PIString propertiesCommandsArr[] = {
        "dfeDataOutputMode",
        "channelCfg",
        "adcCfg",
        "adcbufCfg",
        "profileCfg",
        "chirpCfg",
        "chirpCfg",
        "bpmCfg",
        "frameCfg",
        "lowPower",
        "guiMonitor",
        "cfarCfg",
        "cfarCfg",
        "peakGrouping",
        "multiObjBeamForming",
        "calibDcRangeSig",
        "extendedMaxVelocity",
        "clutterRemoval",
        "compRangeBiasAndRxChanPhase",
        "measureRangeBiasAndRxChanPhase",
        "nearFieldCfg",
        "CQRxSatMonitor",
        "CQSigImgMonitor",
        "analogMonitor",
        "lvdsStreamCfg"
};

class MMWaveProperties : public PIObject {
    PIOBJECT(MMWaveProperties)
public:
    enum Type {
        DFE_DATA_OUTPUT_MODE = 0,
        CHANNEL_CFG,
        ADC_CFG,
        ADCBUF_CFG,
        PROFILE_CFG,
        CHIRP_CFG_0,
        CHIRP_CFG_1,
        BPM_CFG,
        FRAME_CFG,
        LOW_POWER,
        GUI_MONITOR,
        CFAR_CFG_0,
        CFAR_CFG_1,
        PEAK_GROUPING,
        MULTI_OBJ_BEAM_FORMING,
        CALIB_DC_RANGE_SIG,
        EXTENDED_MAX_VEL,
        CLUTTER_REMOVAL,
        COMP_RANGE_BIAS_AND_RX_CHANE_PHASE,
        MEASURE_RANGE_BIAS_AND_CHAN_PHASE,
        NEAR_FIELD_CFG,
        CQ_RX_STAT_MONITOR,
        CQ_SIG_IMG_MONITOR,
        ANALOG_MONITOR,
        LVDS_STREAM_CFG,
        LAST_IN_TYPE
    };

    int dfeDataOutputMode = 1;
    uint8_t channelCfg[3] = {15, 3, 0};
    uint8_t adcCfg[2] = {2, 1};
    int8_t adcbufCfg[5] = {-1, 0, 0, 1, 0};
    double profileCfg[14] = {0, 77, 429, 7, 57.14, 0, 0, 70, 1, 256, 5209, 0, 0, 30};
    uint8_t chirpCfg0[8] = {0, 0, 0, 0, 0, 0, 0, 1};
    uint8_t chirpCfg1[8] = {1, 1, 0, 0, 0, 0, 0, 2};
    int8_t bpmCfg[4] = {-1, 0, 0, 1};
    int32_t frameCfg[7] = {0, 1, 16, 0, 100, 1, 0};
    int8_t lowPower[2] = {0, 0};
    int8_t guiMonitor[7] = {-1, 1, 0, 0, 0, 0, 0};
    int32_t cfarCfg0[8] = {-1, 0, 0, 8, 4, 4, 0, 5120};
    int32_t cfarCfg1[8] = {-1, 1, 0, 4, 2, 3, 0, 5120};
    int16_t peakGrouping[6] = {-1, 1, 1, 1, 1, 255};
    double multiObjBeamForming[3] = {-1, 1, 0.5};
    int32_t calibDcRangeSig[5] = {-1, 0, -5, 8, 256};
    int8_t extendedMaxVelocity[2] = {-1, 0};
    int8_t clutterRemoval[2] = {-1, 0};
    double compRangeBiasAndRxChanPhase[17] = {0.0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
    double measureRangeBiasAndRxChanPhase[3] = {0, 1.5, 0.2};
    int8_t nearFieldCfg[4] = {-1, 0, 0, 0};
    uint16_t CQRxSatMonitor[5] = {0, 3, 4, 127, 0};
    uint16_t CQSigImgMonitor[3] = {0, 63, 8};
    uint8_t analogMonitor[2] = {1, 1};
    int8_t lvdsStreamCfg[4] = {-1, 0, 0, 0};

    PIString& toCommand(PIString& str, Type t);

    PIString toCommand(Type t) {
        PIString str;
        return toCommand(str, t);
    }

    bool fromCommand(PIString& str, Type t);

    PIString toProfile();

    bool fromProfile(const PIString& str);

    bool isSpecialCommand(const PIString& str);

    PIString getPropertyName(Type t);

    PIString getPropertyName(int t);

    PIString getPropertyValues(Type t);

    PIString getPropertyValues(int t);

    PIString getStartCommand() { return "sensorStart\n"; }
    PIString getStopCommand()  { return "sensorStop\n";  }

    MMWaveProperties();

    ~MMWaveProperties();

    void initClass();
    
private:
    PIString& removeEndZeros(PIString& str) {
        int fractionEnd = str.find('.');

        if (fractionEnd < 1) {
            return str;
        }

        while (str.size() - 1 != fractionEnd) {
            if (str.back().toAscii() == '0') {
                str.cutRight(1);
            } else {
                break;
            }
        }

        if (str.size() - 1 == fractionEnd) {
            str.cutRight(1);
        }

        return str;
    }

    template <typename T>
    PIString& appendArray(PIString& str, T* arr, int arrSize) {
        for (int i = 0; i < arrSize; ++i) {
            double f = arr[i];
            if ((int32_t)f != f) {
                PIString strNum = PIString::fromNumber(f);
                str.append(" ").append( removeEndZeros(strNum) );
            } else {
                str.append(" ").append( PIString::fromNumber((int32_t)f) );
            }
        }

        return str;
    }

    template <typename T>
    void parseToArray(T* arr, int arrSize, PIString& str) {
        PIStringList values = str.split(" ");
        PIDeque<PIString>::iterator iter = values.begin();

        if (values.size() > arrSize) {
            assert(0);
        }

        while (iter != values.end()) {
            arr[0] = (*iter).toDouble();
            arr++;
            iter++;
        }
    }

    PIMap<PIString, Type> namesProperties;
    PIMap<PIString, PIVector<Type>> commandsProperties;
    PISet<PIString> specialCommands;
};

#endif //MMWAVEDEMO_MMWAVEDEMOPROPERTYES_H
