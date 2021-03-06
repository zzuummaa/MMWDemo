//
// Created by Stephan on 07.10.2018.
//

#include <gtest/gtest.h>
#include <mmw/sys_common.h>
#include "pivector.h"
#include "mmwave2/MMWMessage.h"

class MMWMessageTLV : public ::testing::Test
{
protected:
    void SetUp() {
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

TEST_F(MMWMessageTLV, TLV_init_detObjs_zero) {
    TLV tlv(tlvPacket0);
    ASSERT_EQ(tlv.length(), sizeof(TLV::Header) + sizeof(TLV::DataObjDescr));
    ASSERT_EQ(tlv.type(), header0.type);
}

TEST_F(MMWMessageTLV, TLV_init_detObjs_excess_malloc) {
    TLV tlv(tlvPacket0.data(), INT32_MAX);
    piCout << sizeof(tlv);
}

TEST_F(MMWMessageTLV, TLV_init_detObjs_non_zero) {
    TLV tlv(tlvPacket2);
    ASSERT_EQ(tlv.length(), sizeof(TLV::Header) + sizeof(TLV::DataObjDescr) + sizeof(detObjArr));
    ASSERT_EQ(tlv.type(), header2.type);
}

TEST_F(MMWMessageTLV, TLV_init_not_enoght_input_data_length) {
    TLV tlv(tlvPacket2.data(0), tlvPacket2.length() - 1);
    ASSERT_EQ(tlv.length(), 0);
    ASSERT_EQ(tlv.type(), TLV::Type::INVALID);
    ASSERT_EQ(tlv.asDetects().length(), 0);
    ASSERT_EQ(tlv.asClusteringReport().length(), 0);
    ASSERT_EQ(tlv.asTrackingReport().length(), 0);
}

TEST_F(MMWMessageTLV, TLV_init_input_zero_pointer_zero_len) {
    EXPECT_EXIT(TLV tlv(nullptr, 0); exit(0),::testing::ExitedWithCode(0),".*");
}

TEST_F(MMWMessageTLV, TLV_asDetects_zero) {
    TLV tlv(tlvPacket0);
    ASSERT_EQ(tlv.type(), TLV::Type::DETECTED_POINTS);
    ASSERT_EQ(tlv.asDetects().length(), 0);
}

TEST_F(MMWMessageTLV, TLV_asDetects_empty) {
    TLV tlv;
    ASSERT_EQ(tlv.asDetects().length(), 0);
}

TEST_F(MMWMessageTLV, TLV_asDetects_non_empty) {
    TLV tlv(tlvPacket2);
    PIVector<TLV::DetObj> vec = tlv.asDetects();
    ASSERT_EQ(vec.length(), sizeof(detObjArr) / sizeof(detObjArr[0]));
    for (int i = 0; i < vec.length(); ++i) {
        ASSERT_FALSE(memcmp(&vec[i], &detObjArr[i], sizeof(detObjArr[i])));
    }
}

TEST_F(MMWMessageTLV, TLV_asDetects_non_detect_type) {
    for (uint32_t i = 0; i < TLV::Type::INVALID; ++i) {
        if (i == TLV::Type::DETECTED_POINTS) continue;
        ((TLV::Header*)tlvPacket2.data())->typeInt = i;
        TLV tlv(tlvPacket2);
        ASSERT_EQ(tlv.asDetects().length(), 0) << "On iteration " << i;
    }
}

TEST_F(MMWMessageTLV, TLV_asAzimuthStaticHeatmap_zero) {
    header0.length = 0;
    header0.type = TLV::Type::AZIMUT_STATIC_HEAT_MAP;
    tlvPacket0.clear();
    tlvPacket0.append(&header0, sizeof(header0));
    TLV tlv(tlvPacket0);
    ASSERT_EQ(tlv.type(), TLV::Type::AZIMUT_STATIC_HEAT_MAP);
    ASSERT_EQ(tlv.asAzimuthStaticHeatmap().size(), 0);
}

TEST_F(MMWMessageTLV, TLV_asAzimuthStaticHeatmap_empty) {
    TLV tlv;
    ASSERT_EQ(tlv.asAzimuthStaticHeatmap().size(), 0);
    // TODO
}

TEST_F(MMWMessageTLV, TLV_asAzimuthStaticHeatmap_non_empty) {
    PIVector<PIVector<complexd>> reqVec;
    PIByteArray data;
    int rangeFFTSize = 256;
    int numOfVirtAnt = 8;
    for (int i = 0; i < rangeFFTSize; ++i) {
        PIVector<complexd> row;
        for (int j = 0; j < numOfVirtAnt; ++j) {
            cmplx16ImRe_t val = {(int16_t)i, (int16_t)j};
            data.append(&val, sizeof(val));
            row.push_back(complexd(val.real, val.imag));
        }
        reqVec.push_back(row);
    }
    header2.type = TLV::Type::AZIMUT_STATIC_HEAT_MAP;
    header2.length = rangeFFTSize * numOfVirtAnt * sizeof(cmplx16ImRe_t);

    tlvPacket2.clear();
    tlvPacket2.append(&header2, sizeof(header2));
    tlvPacket2.append(data);

    TLV tlv(tlvPacket2);
    PIVector<PIVector<complexd>> vec = tlv.asAzimuthStaticHeatmap(rangeFFTSize, numOfVirtAnt);
    ASSERT_EQ(vec.size(), reqVec.size());
    ASSERT_TRUE(vec == reqVec);
}

TEST_F(MMWMessageTLV, TLV_asRangeDopplerHeatmap_non_empty) {
    PIVector<PIVector<float>> reqVec;
    PIByteArray data;
    int rangeFFTSize = 256;
    int dopplerFFTSize = 32;
    for (int i = 0; i < rangeFFTSize; ++i) {
        PIVector<float> row;
        for (int j = 0; j < dopplerFFTSize; ++j) {
            uint16_t val = i + j;
            data.append(&val, sizeof(val));
            row.push_back(val);
        }
        reqVec.push_back(row);
    }
    header2.type = TLV::Type::RANGE_DOPPLER_HEAT_MAP;
    header2.length = rangeFFTSize * dopplerFFTSize * sizeof(uint16_t);

    tlvPacket2.clear();
    tlvPacket2.append(&header2, sizeof(header2));
    tlvPacket2.append(data);

    TLV tlv(tlvPacket2);
    PIVector<PIVector<float>> vec = tlv.asRangeDopplerHeatmap(rangeFFTSize, dopplerFFTSize);
    ASSERT_EQ(vec.size(), reqVec.size());
    ASSERT_TRUE(vec == reqVec);
}

TEST_F(MMWMessageTLV, TLV_asAzimuthRangeHeatmap_non_empty) {
    PIVector<PIVector<float>> reqVec;
    PIByteArray data;
    int rangeFFTSize = 256;
    int numOfVirtAnt = 8;
    for (int i = 0; i < rangeFFTSize; ++i) {
        PIVector<float> row;
        for (int j = 0; j < numOfVirtAnt; ++j) {
            cmplx16ImRe_t val = {(int16_t)i, (int16_t)j};
            data.append(&val, sizeof(val));
            row.push_back((float)abs(complexd(val.real, val.imag)));
        }
        reqVec.push_back(row);
    }
    header2.type = TLV::Type::RANGE_AZIMUT_HEAT_MAP;
    header2.length = rangeFFTSize * numOfVirtAnt * sizeof(cmplx16ImRe_t);

    tlvPacket2.clear();
    tlvPacket2.append(&header2, sizeof(header2));
    tlvPacket2.append(data);

    TLV tlv(tlvPacket2);
    PIVector<PIVector<float>> vec = tlv.asRangeAzimuthHeatmap(rangeFFTSize, numOfVirtAnt);
    ASSERT_EQ(vec.size(), reqVec.size());
    ASSERT_TRUE(vec == reqVec);
}

TEST_F(MMWMessageTLV, TLV_is_data_local_copy){
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

class MMWMessageMessage : public ::testing::Test
{
protected:
    void SetUp()
    {
        memset(&padding, 0, sizeof(padding));

        memset(&headerEmpty, 0, sizeof(headerEmpty));

        memset(&headerZero, 0, sizeof(headerZero));
        memcpy(headerZero.magicWord, requiredMagicWord, sizeof(requiredMagicWord));
        headerZero.numTLVs = 0;
        uint32_t packetLen = sizeof(headerZero);
        packetLen += packetLen % 32 == 0 ? 0 : 32 - packetLen % 32;
        headerZero.totalPacketLen = packetLen;
        messageArrZero.append(&headerZero, sizeof(headerZero));
        messageArrZero.append(&padding, 32 - messageArrZero.length() % 32);

        TLV::DataObjDescr descr;
        descr.numDetetedObj = 2;
        descr.xyzQFormat = 1;
        memset(detObj, 0, sizeof(detObj[0]));
        memset(detObj, 1, sizeof(detObj[1]));
        TLV::Header detObjTLVHeader;
        detObjTLVHeader.type = TLV::Type::DETECTED_POINTS;
        detObjTLVHeader.length = sizeof(descr) + sizeof(detObj);
        memset(&headerTLV, 0, sizeof(headerTLV));
        memcpy(headerTLV.magicWord, requiredMagicWord, sizeof(requiredMagicWord));
        headerTLV.numTLVs = 1;
        packetLen = sizeof(headerTLV) + sizeof(detObjTLVHeader) + detObjTLVHeader.length;
        packetLen += packetLen % 32 == 0 ? 0 : 32 - packetLen % 32;
        headerTLV.totalPacketLen = packetLen;
        messageArrTLV.append(&headerTLV, sizeof(headerTLV));
        messageArrTLV.append(&detObjTLVHeader, sizeof(detObjTLVHeader));
        messageArrTLV.append(&descr, sizeof(descr));
        messageArrTLV.append(&detObj, sizeof(detObj));
        messageArrTLV.append(&padding, 32 - messageArrTLV.length() % 32);

        headerTLVs.numTLVs = TLV::Type::INVALID - 1;
        packetLen = sizeof(headerTLV) + sizeof(TLV::Header) * headerTLVs.numTLVs;
        packetLen += packetLen % 32 == 0 ? 0 : 32 - packetLen % 32;
        headerTLVs.totalPacketLen = packetLen;
        messageArrTLVs.append(&headerTLVs, sizeof(headerTLVs));
        for (uint32_t i = 1; i <= headerTLVs.numTLVs; ++i) {
            TLV::Header tlvHeader;
            tlvHeader.typeInt = i;
            messageArrTLVs.append(&tlvHeader, sizeof(tlvHeader));
        }
        messageArrTLVs.append(&padding, 32 - messageArrTLVs.length() % 32);
    }
    void TearDown() {
    }

    PIByteArray messageArrZero;
    PIByteArray messageArrTLV;
    PIByteArray messageArrTLVs;
    MMWMessage::Header headerZero;
    MMWMessage::Header headerEmpty;
    MMWMessage::Header headerTLV;
    MMWMessage::Header headerTLVs;
    TLV::DetObj detObj[2];
    uint8_t padding[32];
};

TEST_F(MMWMessageMessage, MMWMessage_init_empty) {
    MMWMessage message;
    ASSERT_FALSE(message.isValidPacket());
    ASSERT_TRUE(memcmp(&message.getHeader(), &headerEmpty, sizeof(headerEmpty)) == 0);
}

TEST_F(MMWMessageMessage, MMWMessage_init_zero_tlvs) {
    MMWMessage message(messageArrZero.data(), messageArrZero.length());
    ASSERT_TRUE(message.isValidPacket());
    ASSERT_EQ(message.getTlvs().length(), 0);
    ASSERT_TRUE(memcmp(&headerZero, &message.getHeader(), sizeof(headerZero)) == 0);
}

TEST_F(MMWMessageMessage, MMWMessage_init_excess_malloc) {
    MMWMessage message(messageArrZero.data(), INT32_MAX);
    ASSERT_TRUE(message.isValidPacket());
    ASSERT_EQ(message.getTlvs().length(), 0);
    ASSERT_TRUE(memcmp(&headerZero, &message.getHeader(), sizeof(headerZero)) == 0);
}

TEST_F(MMWMessageMessage, MMWMessage_init_not_enoght_input_data_length) {
    MMWMessage message(messageArrZero.data(), messageArrZero.length() - 1);
    ASSERT_EQ(message.getTlvs().length(), 0);
    ASSERT_TRUE(memcmp(&headerEmpty, &message.getHeader(), sizeof(headerEmpty)) == 0);
}

TEST_F(MMWMessageMessage, MMWMessage_init_input_data_length_smaller_then_magic_length) {
    MMWMessage message(messageArrZero.data(), sizeof(requiredMagicWord) - 1);
    ASSERT_EQ(message.getTlvs().length(), 0);
    ASSERT_TRUE(memcmp(&headerEmpty, &message.getHeader(), sizeof(headerEmpty)) == 0);
}

TEST_F(MMWMessageMessage, MMWMessage_tlv_detObj) {
    MMWMessage message(messageArrTLV.data(), messageArrTLV.length());
    ASSERT_TRUE(message.isValidPacket());
    PIVector<TLV> tlvs = message.getTlvs();
    ASSERT_EQ(tlvs.length(), 1);
    PIVector<TLV::DetObj> actualDetObjs = tlvs[0].asDetects();
    for (int i = 0; i < actualDetObjs.length(); i++) {
        ASSERT_TRUE(memcmp(&actualDetObjs[i], &detObj[i], sizeof(detObj[i])) == 0) << "On iteration " << i;
    }
}

TEST_F(MMWMessageMessage, MMWMessage_several_tlvs) {
    MMWMessage message(messageArrTLVs.data(), messageArrTLVs.length());
    ASSERT_TRUE(message.isValidPacket());
    PIVector<TLV> tlvs = message.getTlvs();
    ASSERT_EQ(tlvs.length(), headerTLVs.numTLVs);
    for (int i = 0; i < tlvs.length(); ++i) {
        ASSERT_NE(tlvs[i].type(), TLV::Type::INVALID);
        ASSERT_EQ(tlvs[i].length(), sizeof(TLV::Header));
    }
}