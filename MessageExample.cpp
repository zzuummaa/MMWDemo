//
// Created by Stephan on 18.07.2018.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "mmwave2/MMWMessage.h"
#include "mmwave2/Utils.h"

TEST(mmw_message, first_test) {
    EXPECT_EQ(true, true);
}

TEST(mmw_message, demonstrate_packet) {
    PIByteArray packet = generateMMWPacket();
    MMWMessage message(packet);
    TLV tlv = message.getTlvs()[0];
    TLV::DetObj detObj = tlv.asDetects()[0];
    EXPECT_EQ(true, true);
}

TEST(mmw_message, last_test) {
    EXPECT_EQ(true, true);
}