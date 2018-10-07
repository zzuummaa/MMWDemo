//
// Created by Stephan on 01.11.2017.
//

#include "MMWaveProperties.h"

MMWaveProperties::MMWaveProperties() {
    initClass();
    //piCout << "MMWaveProperties initialized";
}

MMWaveProperties::~MMWaveProperties() {
}

PIString& MMWaveProperties::toCommand(PIString &str, MMWaveProperties::Type t) {
    if (t >= MMWaveProperties::Type::LAST_IN_TYPE) {
        piCoutObj << "Invalid property type:" << t;
        return str;
    }
    str.append(propertiesCommandsArr[t]);

    switch (t) {
        case DFE_DATA_OUTPUT_MODE:  appendArray(str, &dfeDataOutputMode, 1); break;
        case CHANNEL_CFG:           appendArray(str, channelCfg, sizeof(channelCfg)/sizeof(*channelCfg)); break;
        case ADC_CFG:               appendArray(str, adcCfg, sizeof(adcCfg)/sizeof(*adcCfg)); break;
        case ADCBUF_CFG:            appendArray(str, adcbufCfg, sizeof(adcbufCfg)/sizeof(*adcbufCfg)); break;
        case PROFILE_CFG:           appendArray(str, profileCfg, sizeof(profileCfg)/sizeof(*profileCfg)); break;
        case CHIRP_CFG_0:           appendArray(str, chirpCfg0, sizeof(chirpCfg0)/ sizeof(*chirpCfg0)); break;
        case CHIRP_CFG_1:           appendArray(str, chirpCfg1, sizeof(chirpCfg1)/ sizeof(*chirpCfg1)); break;
        case BPM_CFG:               appendArray(str, bpmCfg, sizeof(bpmCfg)/sizeof(*bpmCfg)); break;
        case FRAME_CFG:             appendArray(str, frameCfg, sizeof(frameCfg)/ sizeof(*frameCfg)); break;
        case LOW_POWER:             appendArray(str, lowPower, sizeof(lowPower)/sizeof(*lowPower)); break;
        case GUI_MONITOR:           appendArray(str, guiMonitor, sizeof(guiMonitor)/ sizeof(*guiMonitor)); break;
        case CFAR_CFG_0:            appendArray(str, cfarCfg0, sizeof(cfarCfg0)/ sizeof(*cfarCfg0)); break;
        case CFAR_CFG_1:            appendArray(str, cfarCfg1, sizeof(cfarCfg1)/ sizeof(*cfarCfg1)); break;
        case PEAK_GROUPING:         appendArray(str, peakGrouping, sizeof(peakGrouping)/ sizeof(*peakGrouping)); break;
        case MULTI_OBJ_BEAM_FORMING:appendArray(str, multiObjBeamForming, sizeof(multiObjBeamForming)/ sizeof(*multiObjBeamForming)); break;
        case CALIB_DC_RANGE_SIG:    appendArray(str, calibDcRangeSig, sizeof(calibDcRangeSig)/ sizeof(*calibDcRangeSig)); break;
        case EXTENDED_MAX_VEL:      appendArray(str, extendedMaxVelocity, sizeof(extendedMaxVelocity)/sizeof(*extendedMaxVelocity)); break;
        case CLUTTER_REMOVAL:       appendArray(str, clutterRemoval, sizeof(clutterRemoval)/sizeof(*clutterRemoval)); break;
        case COMP_RANGE_BIAS_AND_RX_CHANE_PHASE: appendArray(str, compRangeBiasAndRxChanPhase, sizeof(compRangeBiasAndRxChanPhase)/sizeof(*compRangeBiasAndRxChanPhase)); break;
        case MEASURE_RANGE_BIAS_AND_CHAN_PHASE:  appendArray(str, measureRangeBiasAndRxChanPhase, sizeof(measureRangeBiasAndRxChanPhase)/sizeof(*measureRangeBiasAndRxChanPhase)); break;
        case NEAR_FIELD_CFG:        appendArray(str, nearFieldCfg, sizeof(nearFieldCfg)/sizeof(*nearFieldCfg)); break;
        case CQ_RX_STAT_MONITOR:    appendArray(str, CQRxSatMonitor, sizeof(CQRxSatMonitor)/sizeof(*CQRxSatMonitor)); break;
        case CQ_SIG_IMG_MONITOR:    appendArray(str, CQSigImgMonitor, sizeof(CQSigImgMonitor)/sizeof(*CQSigImgMonitor)); break;
        case ANALOG_MONITOR:        appendArray(str, analogMonitor, sizeof(analogMonitor)/sizeof(*analogMonitor)); break;
        case LVDS_STREAM_CFG:       appendArray(str, lvdsStreamCfg, sizeof(lvdsStreamCfg)/sizeof(*lvdsStreamCfg)); break;
        default:
            piCoutObj << "Invalid property type:" << t;
            break;
    }

    return str;
}

PIString MMWaveProperties::toProfile() {
    PIString str;

    str.append("sensorStop\n");
    str.append("flushCfg\n");

    for (int i = 0; i < MMWaveProperties::LAST_IN_TYPE; ++i) {
        str = toCommand(str, MMWaveProperties::Type(i));
        str.append("\n");
    }

    return str;
}

bool MMWaveProperties::fromCommand(PIString &str, Type t) {
    int commandNameEnd = str.find(' ');

    if (t >= LAST_IN_TYPE) {
        piCoutObj << "Invalid property type:" << t;
        return false;
    }

    PIString values = str.takeRight( str.size() - (commandNameEnd+1) );

    switch (t) {
        case DFE_DATA_OUTPUT_MODE:  parseToArray(&dfeDataOutputMode, sizeof(&dfeDataOutputMode)/sizeof(dfeDataOutputMode), values); break;
        case CHANNEL_CFG:           parseToArray(channelCfg, sizeof(channelCfg)/sizeof(*channelCfg), values); break;
        case ADC_CFG:               parseToArray(adcCfg, sizeof(adcCfg)/sizeof(*adcCfg), values); break;
        case ADCBUF_CFG:            parseToArray(adcbufCfg, sizeof(adcbufCfg)/sizeof(*adcbufCfg), values); break;
        case PROFILE_CFG:           parseToArray(profileCfg, sizeof(profileCfg)/sizeof(*profileCfg), values); break;
        case CHIRP_CFG_0:           parseToArray(chirpCfg0, sizeof(chirpCfg0)/sizeof(*chirpCfg0), values); break;
        case CHIRP_CFG_1:           parseToArray(chirpCfg1, sizeof(chirpCfg1)/sizeof(*chirpCfg1), values); break;
        case BPM_CFG:               parseToArray(bpmCfg, sizeof(bpmCfg)/sizeof(*bpmCfg), values); break;
        case FRAME_CFG:             parseToArray(frameCfg, sizeof(frameCfg)/sizeof(*frameCfg), values); break;
        case LOW_POWER:             parseToArray(lowPower, sizeof(lowPower)/sizeof(*lowPower), values); break;
        case GUI_MONITOR:           parseToArray(guiMonitor, sizeof(guiMonitor)/sizeof(*guiMonitor), values); break;
        case CFAR_CFG_0:            parseToArray(cfarCfg0, sizeof(cfarCfg0)/sizeof(*cfarCfg0), values); break;
        case CFAR_CFG_1:            parseToArray(cfarCfg1, sizeof(cfarCfg1)/sizeof(*cfarCfg1), values); break;
        case PEAK_GROUPING:         parseToArray(peakGrouping, sizeof(peakGrouping)/sizeof(*peakGrouping), values); break;
        case MULTI_OBJ_BEAM_FORMING:parseToArray(multiObjBeamForming, sizeof(multiObjBeamForming)/sizeof(*multiObjBeamForming), values); break;
        case CALIB_DC_RANGE_SIG:    parseToArray(calibDcRangeSig, sizeof(calibDcRangeSig)/sizeof(*calibDcRangeSig), values); break;
        case EXTENDED_MAX_VEL:      parseToArray(extendedMaxVelocity, sizeof(extendedMaxVelocity)/sizeof(*extendedMaxVelocity), values); break;
        case CLUTTER_REMOVAL:       parseToArray(clutterRemoval, sizeof(clutterRemoval)/sizeof(*clutterRemoval), values); break;
        case COMP_RANGE_BIAS_AND_RX_CHANE_PHASE: parseToArray(compRangeBiasAndRxChanPhase, sizeof(compRangeBiasAndRxChanPhase)/sizeof(*compRangeBiasAndRxChanPhase), values); break;
        case MEASURE_RANGE_BIAS_AND_CHAN_PHASE:  parseToArray(measureRangeBiasAndRxChanPhase, sizeof(measureRangeBiasAndRxChanPhase)/sizeof(*measureRangeBiasAndRxChanPhase), values); break;
        case NEAR_FIELD_CFG:        parseToArray(nearFieldCfg, sizeof(nearFieldCfg)/sizeof(*nearFieldCfg), values); break;
        case CQ_RX_STAT_MONITOR:    parseToArray(CQRxSatMonitor, sizeof(CQRxSatMonitor)/sizeof(*CQRxSatMonitor), values); break;
        case CQ_SIG_IMG_MONITOR:    parseToArray(CQSigImgMonitor, sizeof(CQSigImgMonitor)/sizeof(*CQSigImgMonitor), values); break;
        case ANALOG_MONITOR:        parseToArray(analogMonitor, sizeof(analogMonitor)/sizeof(*analogMonitor), values); break;
        case LVDS_STREAM_CFG:       parseToArray(lvdsStreamCfg, sizeof(lvdsStreamCfg)/sizeof(*lvdsStreamCfg), values); break;
        default: piCoutObj << "Invalid property type:" << t; return false;
    }

    return true;
}

bool MMWaveProperties::fromProfile(const PIString &str) {
    PIStringList lines = str.split("\n");

    PIDeque<PIString>::iterator iter = lines.begin();

    int commentBegin;

    int paramNum = 0;
    PIString prevCommand;

    while (iter != lines.end()) {
        PIString& line = *iter;
        iter++;

        commentBegin = line.find('%');

        if (commentBegin != -1) {
            if (commentBegin == 0) {
                continue;
            }
            line.resize(commentBegin);
        }

        if (line.right(1) == "\r") {
            line.pop_back();
        }

        if (isSpecialCommand(line)) {
            continue;
        }

        PIString curCommand = line.left( line.find(' ') );

        if (curCommand == prevCommand) {
            paramNum++;
        } else {
            paramNum = 0;
        }

        PIVector<Type> types = commandsProperties[curCommand];
        if (types.size() <= paramNum) {
            return false;
        }

        if (!fromCommand(line, (Type)types[paramNum])) {
            return false;
        }

        prevCommand = curCommand;
    }

    return true;
}

void MMWaveProperties::initClass() {
    if (!namesProperties.isEmpty()) {
        return;
    }
    //piCout << "MMWaveProperties class initializing";

    for (int i = 0; i < LAST_IN_TYPE; ++i) {
        namesProperties[propertiesNamesArr[i]] = Type(i);
    }

    for (int i = 0; i < LAST_IN_TYPE; ++i) {
        PIVector<Type> properties;
        properties.append((Type)i);

        commandsProperties[propertiesCommandsArr[i]].append(properties);
    }

    specialCommands.insert("sensorStart", 1);
    specialCommands.insert("sensorStop", 1);
    specialCommands.insert("flushCfg", 1);

    //piCout << "MMWaveProperties class initialize finish";
}

bool MMWaveProperties::isSpecialCommand(const PIString &str) {
    return specialCommands.contains(str);
}

PIString MMWaveProperties::getPropertyName(MMWaveProperties::Type t) {
    assert((int)t < LAST_IN_TYPE);
    return propertiesNamesArr[t];
}

PIString MMWaveProperties::getPropertyName(int t) {
    if ((int)t >= (int)LAST_IN_TYPE) {
        piCoutObj << "Invalid type:" << t;
    }
    return propertiesNamesArr[t];
}

PIString MMWaveProperties::getPropertyValues(MMWaveProperties::Type t) {
    if ((int)t >= (int)LAST_IN_TYPE) {
        piCoutObj << "Invalid type:" << t;
    }

    PIString str;
    str = toCommand(str, t);
    str.cutLeft(str.find(' ') + 1);

    return str;
}

PIString MMWaveProperties::getPropertyValues(int t) {
    return getPropertyValues((Type)t);
}
