//
// Created by Stephan on 11.10.2017.
//

#include <pip.h>
#include <chrono>
#include <thread>
#include "mmw/detected_obj.h"
#include "MMWaveDevice.h"

/*! @brief Azimuth FFT size */
#define MMW_NUM_ANGLE_BINS 64
#define PI 3.14159265359

uint32_t getDetectedObjCount(MmwDemo_msgTlv* tlv) {
    MmwDemo_output_message_dataObjDescr* dataObjDescr
            = reinterpret_cast<MmwDemo_output_message_dataObjDescr *>(tlv->address);
    return dataObjDescr->numDetetedObj;
}

MMWaveDevice::MMWaveDevice(): MMWaveDevice("ser://COM3:115200", "ser://COM4:921600") {
}

MMWaveDevice::MMWaveDevice(PIIODevice *configPort_, PIIODevice *dataPort_) {
    configPort.reset(configPort_);
    dataPort.reset(dataPort_);
    //CONNECTU(dataPort.get(), threadedReadEvent, &packetExtractor, onData);
}

MMWaveDevice::MMWaveDevice(const PIString &configPortPath, const PIString &dataPortPath):
        MMWaveDevice(PIIODevice::createFromFullPath(configPortPath), PIIODevice::createFromFullPath(dataPortPath)) {}

bool MMWaveDevice::execDeviceCommand(PIString inStr, PIStringList* response) {
    if (inStr.back().toConsole1Byte() == '\n') {
        inStr = inStr.cutRight(1);
    }
    if (inStr.back().toConsole1Byte() == '\r') {
        inStr = inStr.cutRight(1);
    }
    inStr.append("\n");

    PIByteArray data = inStr.toByteArray();
    if (configPort->write(data.data(), data.size_s()) != data.size_s()) {
        piCoutObj << "Send command fail";
        return false;
    }

    if (inStr.back().toConsole1Byte() == '\n') {
        inStr = inStr.cutRight(1);
    }
    //piCout << "Sent to device: " << PIString("'").append(inStr).append("'");

    PIString outStr;
    do {
        outStr.append(configPort->read(200));
    } while (!outStr.endsWith(":/>"));

//    if (configPort->fullPathPrefix() == "ser") {
//        outStr = ((PISerial*)configPort.get())->readData(100, 100);
//    } else if (configPort->fullPathPrefix() == "file") {
//        // TODO make usable logic
//        outStr = configPort->read(5000);
//    } else {
//        piCoutObj << "Config port prefix '" << configPort->fullPathPrefix() << "' is invalid";
//        return false;
//    }

    //outStr = PIString("'").append(outStr).append("'");
    piCout << outStr;

    PIStringList list = outStr.split("\n");
    for (int i = 0; i < list.size_s(); ++i) {
        PIString str = list[i].trim();
        if (str == "Done") {
            if (response != nullptr) {
                *response = list;
            }
            return true;
        }
    }

    return false;
}

bool MMWaveDevice::startDev() {
    setState(START);
    onStateChange(START);
    return start(false);
}

PIIODevice* MMWaveDevice::getConfigPort() {
    return configPort.get();
}

PIIODevice* MMWaveDevice::getDataPort() {
    return dataPort.get();
}

MMWavePacketExtractor &MMWaveDevice::getPacketExtractor() {
    return packetExtractor;
}

void MMWaveDevice::run() {
    mutex.lock();
    State stateBefore = this->state;
    mutex.unlock();

    switch (stateBefore) {
    case START: onStart(); break;
    case CONFIGURE: onConfigure(); break;
    case READ_DATA: onReadData(); break;
    case STOP: onStop(); break;
    case ERROR: break;
    default: piCoutObj << "Unknown state";
    }

    mutex.lock();
    State stateAfter = this->state;
    mutex.unlock();
    if (stateBefore != stateAfter) onStateChange(stateAfter);
}

void MMWaveDevice::onStart() {
    mutex.lock();

    if (!config.isEmpty()) {
        state = CONFIGURE;
        mutex.unlock();
        return;
    }

    if (!dataPort->open()) {
        state = ERROR;
    } else {
//        dataPort->startThreadedRead();
        state = READ_DATA;
    }

    mutex.unlock();
}

void MMWaveDevice::onConfigure() {
    if (!configPort->open()) {
        setState(ERROR);
        return;
    }

    PIStringList versionResp;
    if (!execDeviceCommand("version\n", &versionResp)) {
        setState(ERROR);
        return;
    }
    PIString sdkVersionName = "mmWave SDK Version";
    for (PIDeque<PIString>::iterator iter = versionResp.begin(); iter != versionResp.end(); iter++) {
        PIString str = *iter;
        if (str.trimmed().startsWith(sdkVersionName)) {
            int versionPos = str.find(":");
            if (versionPos < 0) {
                setState(ERROR);
                return;
            }
            PIStringList version = str.mid(versionPos + 1).trim().split(".");
            if (version.size() < 4) {
                setState(ERROR);
                return;
            }
            // TODO save version
        }
    }

    mutex.lock();
    PIStringList lines = config.split("\n");
    mutex.unlock();

    for (PIDeque<PIString>::iterator iter = lines.begin(); iter != lines.end(); iter++) {
        PIString inStr = *iter;

        if (!execDeviceCommand(inStr)) {
            setState(ERROR);
            return;
        }
    }

    if (!configPort->close()) {
        setState(ERROR);
        return;
    }

    if (!dataPort->open()) {
        setState(ERROR);
        return;
    } else {
        state = READ_DATA;
    }
}

void MMWaveDevice::onReadData() {
    if (!dataPort->isOpened()) setState(ERROR);
    PIByteArray data = dataPort->read(4096);
    if (!data.isEmpty()) packetExtractor.onData(data.data(), data.size());
}

void MMWaveDevice::onStop() {
    if (!dataPort->close()) setState(ERROR);
    stop();
}

MMWaveDevice::State MMWaveDevice::getState() {
    mutex.lock();
    State state = this->state;
    mutex.unlock();
    return state;
}

void MMWaveDevice::setState(MMWaveDevice::State state) {
    mutex.lock();
    this->state = state;
    mutex.unlock();
}

bool MMWaveDevice::stopDev() {
    setState(STOP);
    return true;
}

void MMWaveDevice::setConfig(const PIString & config) {
    mutex.lock();
    this->config.clear();
    this->config.append(config);
    mutex.unlock();
}
