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

void freeMemory(MmwDemo_message message) {
    for (int i = 0; i < message.body.detObj.header.numTLVs; ++i) {
        uint8_t* p = reinterpret_cast<uint8_t *>(message.body.detObj.tlv[i].address);
        delete[] p;
    }
}

bool parsePacket(uint8_t* buff, MmwDemo_detInfoMsg* message) {
    static int32_t packetCount = 0;
    packetCount++;

    memcpy(&message->header, buff, sizeof(MmwDemo_output_message_header_t));

    uint8_t* buffTmp = buff;

    buff += sizeof(MmwDemo_output_message_header_t);
    for (int i = 0; i < message->header.numTLVs; ++i) {
        MmwDemo_output_message_tl* message_tl = (MmwDemo_output_message_tl*)buff;
        buff += sizeof(MmwDemo_output_message_tl);

        message->tlv[i].length = message_tl->length;
        message->tlv[i].type = message_tl->type;

        if (message->tlv[i].length > message->header.totalPacketLen) {
            message->tlv[i].length = 0;
            message_tl->length = 0;
            break;
        }

        //uint8_t* address = new uint8_t[message_tl->length];
        message->tlv[i].address = reinterpret_cast<std::uintptr_t>(buff);
        //memcpy(address, buff, message_tl->length);
        buff += message_tl->length;
    }

    int countedPacketLen = (int)(buff - buffTmp);
    countedPacketLen += countedPacketLen % 32 == 0 ? 0 : 32 - countedPacketLen % 32;
    if (countedPacketLen != message->header.totalPacketLen) {
        //piCout << "error parse packet";
        return false;
    }
    return true;
}

uint32_t getDetectedObjCount(MmwDemo_msgTlv* tlv) {
    MmwDemo_output_message_dataObjDescr* dataObjDescr
            = reinterpret_cast<MmwDemo_output_message_dataObjDescr *>(tlv->address);
    return dataObjDescr->numDetetedObj;
}

DetObjFrame parseDetectedObj(MmwDemo_msgTlv* tlv) {
    DetObjFrame detObjc;
    uint8_t* p = reinterpret_cast<uint8_t *>(tlv->address);

    uint32_t detObjCount = ((MmwDemo_output_message_dataObjDescr*)p)->numDetetedObj;
    p += sizeof(MmwDemo_output_message_dataObjDescr);

    for (int i = 0; i < detObjCount; ++i) {
        MmwDemo_detectedObj* detectedObj = (MmwDemo_detectedObj*)p;
        detObjc.push_back(*detectedObj);
        p += sizeof(MmwDemo_detectedObj);
    }

    return detObjc;
}

MMWaveDevice::MMWaveDevice(): MMWaveDevice("ser://COM3:115200", "ser://COM4:921600") {
}

MMWaveDevice::MMWaveDevice(const PIString &configPortPath, const PIString &dataPortPath) {
    configPort.reset(PIIODevice::createFromFullPath(configPortPath));
    dataPort.reset(PIIODevice::createFromFullPath(dataPortPath));
    //CONNECTU(dataPort.get(), threadedReadEvent, &packetExtractor, onData);
    isOpen = false;
}

bool MMWaveDevice::execDeviceCommand(PIString inStr) {
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
    if (configPort->fullPathPrefix() == "ser") {
        outStr = ((PISerial*)configPort.get())->readData(100, 100);
    } else if (configPort->fullPathPrefix() == "file") {
        // TODO make usable logic
        outStr = configPort->read(5000);
    } else {
        piCoutObj << "Config port prefix '" << configPort->fullPathPrefix() << "' is invalid";
        return false;
    }

    //outStr = PIString("'").append(outStr).append("'");
    piCout << outStr;

    PIStringList list = outStr.split("\n");
    for (int i = 0; i < list.size_s(); ++i) {
        PIString str = list[i].trim();
        if (str == "Done") return true;
    }

    return false;
}

bool MMWaveDevice::configureDevice(PIString& config) {
    if (!configPort->isOpened()) {
        if (!configPort->open()) {
            return false;
        }
    }
    
    PIStringList lines = config.split("\n");
    PIDeque<PIString>::iterator iter = lines.begin();

    while (iter != lines.end()) {
        PIString inStr = *iter;
        iter++;

        if (!execDeviceCommand(inStr)) {
            return false;
        }
    }

    return configPort->close();

}

bool MMWaveDevice::startDev() {
    setState(START);
    return start(false);
}

std::shared_ptr<PIIODevice> MMWaveDevice::getConfigPort() {
    return configPort;
}

std::shared_ptr<PIIODevice> MMWaveDevice::getDataPort() {
    return dataPort;
}

bool MMWaveDevice::configureToStart() {
    if (!configPort->isOpened()) {
        if (!configPort->open()) {
            return false;
        }
    }

    if ( /*!execDeviceCommand("advFrameCfg")
      ||*/ !execDeviceCommand("sensorStart")) {
        return false;
    }

    return configPort->close();

}

bool MMWaveDevice::configureToStop() {
    if (!configPort->isOpened()) {
        if (!configPort->open()) {
            return false;
        }
    }

    if (!execDeviceCommand("sensorStop")) {
        return false;
    }

    return configPort->close();

}

MMWavePacketExtractor &MMWaveDevice::getPacketExtractor() {
    return packetExtractor;
}

void MMWaveDevice::run() {
    mutex.lock();
    State state = this->state;
    mutex.unlock();

    switch (state) {
    case START: onStart(); break;
    case CONFIGURE: onConfigure(); break;
    case READ_DATA: onReadData(); break;
    case STOP: onStop(); break;
    case ERROR: onStop(); break;
    default: piCoutObj << "Unknown state";
    }
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
    bool isSuccess = true;

    if (!configPort->open()) {
        setState(ERROR);
        return;
    }

    mutex.lock();
    PIStringList lines = config.split("\n");
    mutex.unlock();
    PIDeque<PIString>::iterator iter = lines.begin();

    while (iter != lines.end()) {
        PIString inStr = *iter;
        iter++;

        if (!execDeviceCommand(inStr)) {
            isSuccess = false;
        }
    }

    if (!configPort->close()) {
        setState(ERROR);
        return;
    }

    //msleep(1500);
    if (!dataPort->open()) {
        setState(ERROR);
        return;
    } else {
//        dataPort->startThreadedRead();
        state = READ_DATA;
    }
}

void MMWaveDevice::onReadData() {
    if (!dataPort->isOpened()) setState(ERROR);
    PIByteArray data = dataPort->read(4096);
    packetExtractor.onData(data.data(), data.size());
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
