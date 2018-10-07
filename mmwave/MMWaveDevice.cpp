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

    thread.setThreadSafe(false);
    CONNECTU(&thread, started, this, run);
    isOpen = false;

    CONNECTU(dataPort.get(), threadedReadEvent, &packetExtractor, onData);

    piCout << "MMWaveDevice initialized";
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
    if (configPort.get()->write(data.data(), data.size_s()) == data.size_s()) {
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
    if (!deviceMutex.tryLock()) {
        return false;
    }
    
    if (!configPort->isOpened()) {
        if (!configPort->open()) {
            deviceMutex.unlock();
            return false;
        }
    }
    
    PIStringList lines = config.split("\n");
    PIDeque<PIString>::iterator iter = lines.begin();

    while (iter != lines.end()) {
        PIString inStr = *iter;
        iter++;

        if (!execDeviceCommand(inStr)) {
            deviceMutex.unlock();
            return false;
        }
    }

    if (!configPort->close()) {
        return false;
    }

    deviceMutex.unlock();
    return true;
}

bool MMWaveDevice::open() {
    if (!configPort->open() || !dataPort->open()) {
        return false;
    }

    isOpen = true;
    return true;
}

bool MMWaveDevice::close() {
    bool status = true;

    if (!dataPort->isClosed()) {
        if (!dataPort->close()) {
            piCoutObj << "data port close error";
            status = false;
        }
    }

    if (!configPort->isClosed()) {
        if (!configPort->close()) {
            piCoutObj << "config port close error";
            status = false;
        }
    }

    isOpen = false;
    return true;
}

bool MMWaveDevice::isOpened() {
    return isOpen;
}

bool MMWaveDevice::start() {
    if (!configureToStart()) {
        piCoutObj << "Can't configure to start device";
        return false;
    }

    if (!dataPort->isOpened()) {
        if (!dataPort->open()) {
            piCoutObj << "Can't open data port";
            return false;
        }
    }

    if (!thread.startOnce()) {
        piCoutObj << "Can't start thread";
        return false;
    }

    dataPort->startThreadedRead();

    return true;
}

bool MMWaveDevice::waitForFinish(int time_out) {
    return thread.waitForFinish(time_out);
}

void MMWaveDevice::lock() {
    dataMutex.lock();
}

void MMWaveDevice::unlock() {
    dataMutex.unlock();
}

void MMWaveDevice::run() {
    //piCoutObj << "run() executed";

    uint32_t prevFrameNumber = 0;

    while (!thread.isStopping()) {
        if (!packetExtractor.hasNextPacket()) {
            piMSleep(10);
            continue;
        }

        PIByteArray buffer = packetExtractor.nextPacket();

        MmwDemo_detInfoMsg message;
        if (!parsePacket(buffer.data(), &message)) {
            piCout << "can't parse packet";
            continue;
        }

        MmwDemo_output_message_header* msg_header = &message.header;

        if (prevFrameNumber != 0) {
            //piCoutObj << "Current frameNumber: " << msg_header->frameNumber;
            if (msg_header->frameNumber == prevFrameNumber+1) {
                prevFrameNumber++;
            } else {
                piCoutObj << "Warning: skipped packets from " << prevFrameNumber << "to" << msg_header->frameNumber-1;
                prevFrameNumber = msg_header->frameNumber;
            }
        } else {
            prevFrameNumber = msg_header->frameNumber;
        }

        MMWaveDataContainer container;
		container.type = (msg_header->subFrameNumber == 0 ? MMWaveDataContainer::Near : MMWaveDataContainer::Far);

        for (int i = 0; i < msg_header->numTLVs; ++i) {
            container.fromTLV(&message.tlv[i]);
//            switch (message.body.detObj.tlv[i].type) {
//                case MMWDEMO_OUTPUT_MSG_DETECTED_POINTS:
//                    detObjs = parseDetectedObj(&message.body.detObj.tlv[i]);
//
//                    //piCoutObj << "On frame";
//
//                    break;
//                default:
//                    break;
//            }
        }

        dataMutex.lock();
        dataQueue.push_back(container);
        dataMutex.unlock();
    }

    //piCoutObj << "Thread stopped";

}

void MMWaveDevice::interrupt() {
//    if (!configureToStop()) {
//        piCoutObj << "Can't stop device";
//    }
    thread.stop();
}

std::shared_ptr<PIIODevice> MMWaveDevice::getConfigPort() {
    return configPort;
}

std::shared_ptr<PIIODevice> MMWaveDevice::getDataPort() {
    return dataPort;
}

bool MMWaveDevice::configureToStart() {
    deviceMutex.lock();

    if (!configPort->isOpened()) {
        if (!configPort->open()) {
            deviceMutex.unlock();
            return false;
        }
    }

    if ( /*!execDeviceCommand("advFrameCfg")
      ||*/ !execDeviceCommand("sensorStart")) {
        deviceMutex.unlock();
        return false;
    }

    if (!configPort->close()) {
        deviceMutex.unlock();
        return false;
    }

    deviceMutex.unlock();
    return true;
}

bool MMWaveDevice::configureToStop() {
    deviceMutex.lock();

    if (!configPort->isOpened()) {
        if (!configPort->open()) {
            deviceMutex.unlock();
            return false;
        }
    }

    if (!execDeviceCommand("sensorStop")) {
        deviceMutex.unlock();
        return false;
    }

    if (!configPort->close()) {
        deviceMutex.unlock();
        return false;
    }

    deviceMutex.unlock();
    return true;
}

bool MMWaveDevice::isReadyData() {
    bool isReady = !dataQueue.isEmpty();
    return isReady;
}

MMWaveDataContainer MMWaveDevice::getData() {
    return dataQueue.take_front();
}

void MMWaveDevice::clearDataQueue() {
    dataQueue.clear();
}
