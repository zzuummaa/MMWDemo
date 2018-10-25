//
// Created by Stephan on 09.10.2018.
//

#include <gtest/gtest.h>
#include "MMWavePacketExtractor.h"
#include "MMWMessage.h"
#include "pithread.h"


class MMWPacketExtractor : public ::testing::Test
{
protected:
    void SetUp() {
        // Filling trash
        uint8_t t[] = {1, 0};
        for (int i = 0; i < 128; ++i) {
            trash.append(&t, sizeof(t));
        }

        TLV::DataObjDescr descr;
        descr.numDetetedObj = 2;

        TLV::DetObj detObj;

        TLV::Header tlvHeader;
        tlvHeader.type = TLV::Type::DETECTED_POINTS;
        tlvHeader.length = sizeof(descr) + 2 * sizeof(detObj);

        MMWMessage::Header packetHeader;
        memcpy(packetHeader.magicWord, requiredMagicWord, sizeof(requiredMagicWord));
        packetHeader.numTLVs = 1;
        packetHeader.totalPacketLen = sizeof(packetHeader) + sizeof(tlvHeader) + tlvHeader.length;

        packet.append(&packetHeader, sizeof(packetHeader));
        packet.append(&tlvHeader, sizeof(tlvHeader));
        packet.append(&descr, sizeof(descr));
        packet.append(&detObj, sizeof(detObj));
        packet.append(&detObj, sizeof(detObj));
    }
    void TearDown() {

    }

    MMWavePacketExtractor extractor;
    PIByteArray trash;
    PIByteArray packet;
};

TEST_F(MMWPacketExtractor, MMWMessage_init) {
    ASSERT_FALSE(extractor.hasNextPacketM());
    ASSERT_EQ(extractor.condition(), MMWavePacketExtractor::WAIT_MAGIC_WORLD);
}

TEST_F(MMWPacketExtractor, MMWMessage_onData_trash) {
    extractor.onData(trash.data(), trash.length());
    ASSERT_FALSE(extractor.hasNextPacketM());
    ASSERT_EQ(extractor.condition(), MMWavePacketExtractor::WAIT_MAGIC_WORLD);
}

TEST_F(MMWPacketExtractor, MMWMessage_onData_magicword) {
    extractor.onData((uchar *) requiredMagicWord, sizeof(requiredMagicWord));
    ASSERT_FALSE(extractor.hasNextPacketM());
    ASSERT_EQ(extractor.condition(), MMWavePacketExtractor::WAIT_HEADER);
}

TEST_F(MMWPacketExtractor, MMWMessage_onData_header) {
    MMWMessage::Header header;
    header.totalPacketLen = sizeof(MMWMessage::Header) + 32;
    memcpy(header.magicWord, requiredMagicWord, sizeof(requiredMagicWord));
    extractor.onData((uchar *) &header, sizeof(header));
    ASSERT_FALSE(extractor.hasNextPacketM());
    ASSERT_EQ(extractor.condition(), MMWavePacketExtractor::WAIT_PACK_END);
}

TEST_F(MMWPacketExtractor, MMWMessage_onData_packet) {
    extractor.onData(packet.data(), packet.size());
    ASSERT_TRUE(extractor.hasNextPacketM());
    MMWMessage message = extractor.nextPacketM();
    ASSERT_TRUE(message.isValidPacket());
    ASSERT_EQ(extractor.condition(), MMWavePacketExtractor::WAIT_MAGIC_WORLD);
}

TEST_F(MMWPacketExtractor, MMWMessage_onData_packets) {
    PIByteArray packets;
    packets.append(packet).append(packet);
    extractor.onData(packets.data(), packets.size());
    for (int i = 0; i < 2; ++i) {
        ASSERT_TRUE(extractor.hasNextPacketM()) << "on iteration " << i;
        MMWMessage message = extractor.nextPacketM();
        ASSERT_TRUE(message.isValidPacket()) << "on iteration " << i;
    }
    ASSERT_EQ(extractor.condition(), MMWavePacketExtractor::WAIT_MAGIC_WORLD);
}