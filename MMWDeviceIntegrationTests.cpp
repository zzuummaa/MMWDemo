//
// Created by Stephan on 16.10.2018.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <pip.h>
#include <climits>
#include "mmwave/MMWaveDevice.h"
#include "MMWaveProperties.h"

TEST(MMWaveDevice_test, mmw) {
    MMWaveDevice device("ser://COM3:115200", "ser://COM4:921600");

    MMWaveProperties properties;
    device.setConfig(properties.toProfile().append("sensorStart\n"));
    ASSERT_TRUE(device.startDev());

    int packetCount = 0;
    while (packetCount < 10) {
        if (!device.getPacketExtractor().hasNextPacketM()) {
            piMSleep(20);
            continue;
        }
        MMWMessage message = device.getPacketExtractor().nextPacketM();
        ASSERT_TRUE(message.isValidPacket());
        piCout << message;
        packetCount++;
    }
}

class ConfigPortEventHandler : public PIObject {
PIOBJECT(ConfigPortEventHandler)
public:
    PIString response;

    ConfigPortEventHandler(PIIODevice& dev): isFailed_(false) {
        CONNECTU(&dev, threadedWriteEvent, this, onWrite);
        CONNECTU(&dev, threadedReadEvent, this, onRead);
    }

    EVENT_HANDLER2(void, onWrite, ullong, id, int, written_size) {
        piCout << "id:" << id << "written_size:" << written_size;
        piMSleep(30);
        m.lock();
        if (!isReceivedSuccessResp) {
            isFailed_ = true;
        }
        isReceivedSuccessResp = false;
        m.unlock();
    }

    EVENT_HANDLER2(void, onRead, uchar * , data, int, size) {
        response.append(PIByteArray(data, size));
        if (response.endsWith(":/>")) {
            onCommandResponseRead(response);
            response.clear();
            m.lock();
            isReceivedSuccessResp = true;
            m.unlock();
        }
//        piCout << PIString(PIByteArray(data, size));
    }

    void onCommandResponseRead(PIString& data) {
        piCout << data;
    }

    bool isFailed() const {
        return isFailed_;
    }

private:
    PIVector<ullong> ids;
    bool isReceivedSuccessResp;
    bool isFailed_;
    PIMutex m;
};

TEST(DISABLED_MMWaveDevice_test, upload_profile) {
    MMWaveProperties properties;
    PISerial serial("COM3");
    ConfigPortEventHandler handler(serial);

    ASSERT_TRUE(serial.open());
    serial.startThreadedRead();
    PIStringList commandList = properties.toProfile().split("\n");
    for (int i = 0; i < commandList.size(); ++i) {
        PIString str = commandList[i] + "\n";
        serial.writeThreaded(str.toByteArray());
    }
    serial.startThreadedWrite();
    serial.waitForFinish(100 + commandList.size() * 60);
    piCout << "Is failed transmission: " << handler.isFailed();
    EXPECT_FALSE(handler.isFailed());
}