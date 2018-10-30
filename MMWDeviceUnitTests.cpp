//
// Created by Stephan on 06.10.2018.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <pip.h>
#include <climits>
#include "mmwave/MMWaveDevice.h"
#include "CmakeDefinitions.h"
#include "string"

using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::_;
using ::testing::DoAll;

class ConfigDeviceMOC: public PIIODevice {
    PIIODEVICE(ConfigDeviceMOC)
public:
    ConfigDeviceMOC(): PIIODevice() {}

    MOCK_METHOD0(openDevice, bool());
    MOCK_METHOD2(write, int(const void * data, int max_size));
    MOCK_METHOD2(read, int(const void * data, int max_size));
protected:

    PIString fullPathPrefix() const {return "ser";}
    void configureFromFullPath(const PIString & full_path) {
        // parse full_path and configure device there
    }
};

TEST(DISABLED_MMWaveDevice_test, init) {
    PIString configPortName = PIString("file://") + TEST_FILES_DIR + "/empty.bin";
    PIString dataPortName = PIString("file://") + TEST_FILES_DIR + "/empty.bin";
    piCout << "";
    piCout << "configPort:" << configPortName;
    piCout << "dataPort:" << dataPortName;
    MMWaveDevice device(configPortName, dataPortName);
}

TEST(MMWaveDevice_test, MOC) {
    ConfigDeviceMOC configPort;

    EXPECT_CALL(configPort, openDevice())
            .WillOnce(Return(true));

    EXPECT_TRUE(configPort.open());
}

TEST(MMWaveDevice_test, readData) {
    PIString configPortName = PIString("file://") + TEST_FILES_DIR + "/empty.bin";
    PIString dataPortName = PIString("file://") + TEST_FILES_DIR + "/data.bin";
    MMWaveDevice device(configPortName, dataPortName);

    ASSERT_TRUE(device.startDev());
    piSleep(0.1);

    int packetCount = 0;
    piCout << "";
    while (device.getPacketExtractor().hasNextPacketM()) {
        MMWMessage message = device.getPacketExtractor().nextPacketM();
        ASSERT_TRUE(message.isValidPacket());
        piCout << message;
        packetCount++;
    }

    EXPECT_NE(packetCount, 0);
    if (packetCount != 0) piCout << "packetCount:" << packetCount;

    ASSERT_TRUE(device.stopDev());
}