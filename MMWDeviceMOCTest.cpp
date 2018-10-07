//
// Created by Stephan on 06.10.2018.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <pip.h>
#include <climits>
#include "mmwave/MMWaveDevice.h"
#include "CmakeDefinitions.h"


TEST(MMWaveDevice_test, init) {
    PIString configPortName = PIString("file://") + TEST_FILES_DIR + "/empty.bin";
    PIString dataPortName = PIString("file://") + TEST_FILES_DIR + "/empty.bin";
    piCout << "";
    piCout << "configPort:" << configPortName;
    piCout << "dataPort:" << dataPortName;
    MMWaveDevice device(configPortName, dataPortName);
}

TEST(MMWaveDevice_test, smart_pointers) {
    MMWaveDevice* device = new MMWaveDevice();
    auto configPort = device->getConfigPort();
    auto dataPort = device->getDataPort();

    ASSERT_EQ(configPort.use_count(), 2);
    ASSERT_EQ(dataPort.use_count(), 2);

    delete device;

    ASSERT_EQ(configPort.use_count(), 1);
    ASSERT_EQ(dataPort.use_count(), 1);
}

TEST(MMWaveDevice_test, readData) {
    PIString configPortName = PIString("file://") + TEST_FILES_DIR + "/empty.bin";
    PIString dataPortName = PIString("file://") + TEST_FILES_DIR + "/empty.bin";
    MMWaveDevice device(configPortName, dataPortName);
}