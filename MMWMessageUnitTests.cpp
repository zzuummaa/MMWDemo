//
// Created by Stephan on 07.10.2018.
//

#include <gtest/gtest.h>
#include "pivector.h"
#include "mmwave2/MMWMessage.h"

class MMWMessageDetects : public ::testing::Test
{
protected:
    void SetUp()
    {
        header0.length = sizeof(TLV::DataObjDescr);
        header0.type = TLV::Type::DETECTED_POINTS;
        descr0.xyzQFormat = 1;
        descr0.numDetetedObj = 0;
        tlvPacket0.append(&header0, sizeof(header0));
        tlvPacket0.append(&descr0, sizeof(descr0));

        header2.length = sizeof(descr2) + sizeof(detObjArr);
        header2.type = TLV::Type::DETECTED_POINTS;
        descr2.xyzQFormat = 1;
        descr2.numDetetedObj = 2;

        int fieldNums = 7;
        for (int i = 0; i < 2; ++i) {
            detObjArr[i].x = 0 + i * fieldNums;
            detObjArr[i].y = 1 + i * fieldNums;
            detObjArr[i].z = 2 + i * fieldNums;
            detObjArr[i].rangeIdx = 3 + i * fieldNums;
            detObjArr[i].dopplerIdx = 4 + i * fieldNums;
            detObjArr[i].peakVal = 5 + i * fieldNums;
        }

        tlvPacket2.append(&header2, sizeof(header2));
        tlvPacket2.append(&descr2, sizeof(descr2));
        tlvPacket2.append(&detObjArr, sizeof(detObjArr));
    }
    void TearDown() {
    }

    TLV::Header header0;
    TLV::Header header2;
    TLV::DetObj detObjArr[2];
    TLV::DataObjDescr descr0;
    TLV::DataObjDescr descr2;
    PIByteArray tlvPacket0;
    PIByteArray tlvPacket2;
};

TEST_F(MMWMessageDetects, TLV_init_detObjs_empty) {
    TLV tlv(tlvPacket0);
    ASSERT_EQ(tlv.length(), sizeof(TLV::Header) + sizeof(TLV::DataObjDescr));
    ASSERT_EQ(tlv.type(), header0.type);
}

TEST_F(MMWMessageDetects, TLV_init_detObjs_non_empty) {
    TLV tlv(tlvPacket2);
    ASSERT_EQ(tlv.length(), sizeof(TLV::Header) + sizeof(TLV::DataObjDescr) + sizeof(detObjArr));
    ASSERT_EQ(tlv.type(), header2.type);
}

TEST_F(MMWMessageDetects, TLV_init_small_length) {
    TLV tlv(tlvPacket2.data(0), tlvPacket2.length() - 1);
    ASSERT_EQ(tlv.length(), 0);
    ASSERT_EQ(tlv.type(), TLV::Type::MAX);
    ASSERT_EQ(tlv.asDetects().length(), 0);
    ASSERT_EQ(tlv.asClusteringReport().length(), 0);
    ASSERT_EQ(tlv.asTrackingReport().length(), 0);
}

TEST_F(MMWMessageDetects, TLV_asDetects_empty) {
    TLV tlv(tlvPacket0);
    ASSERT_EQ(tlv.asDetects().length(), 0);
}

TEST_F(MMWMessageDetects, TLV_asDetects_non_empty) {
    TLV tlv(tlvPacket2);
    PIVector<TLV::DetObj> vec = tlv.asDetects();
    ASSERT_EQ(vec.length(), sizeof(detObjArr) / sizeof(detObjArr[0]));
    for (int i = 0; i < vec.length(); ++i) {
        ASSERT_FALSE(memcmp(&vec[i], &detObjArr[i], sizeof(detObjArr[i])));
    }
}

TEST_F(MMWMessageDetects, TLV_is_data_local_copy){
    TLV tlv(tlvPacket2);
    memset(tlvPacket2.data(), 0, tlvPacket2.length());
    ASSERT_EQ(tlv.length(), sizeof(TLV::Header) + sizeof(TLV::DataObjDescr) + sizeof(detObjArr));
    ASSERT_EQ(tlv.type(), header2.type);

    PIVector<TLV::DetObj> vec = tlv.asDetects();
    ASSERT_EQ(vec.length(), sizeof(detObjArr) / sizeof(detObjArr[0]));
    for (int i = 0; i < vec.length(); ++i) {
        ASSERT_FALSE(memcmp(&vec[i], &detObjArr[i], sizeof(detObjArr[i])));
    }
}

TEST_F(MMWMessageDetects, TLV_asDetects_invalid_type) {
    for (uint32_t i = 0; i < TLV::Type::MAX; ++i) {
        if (i == TLV::Type::DETECTED_POINTS) continue;
        ((TLV::Header*)tlvPacket2.data())->typeInt = i;
        TLV tlv(tlvPacket2);
        ASSERT_EQ(tlv.asDetects().length(), 0) << "On iteration " << i;
    }
}