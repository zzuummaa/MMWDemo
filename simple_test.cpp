//
// Created by Stephan on 18.07.2018.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "pip.h"
#include "pistring.h"

TEST(My, simple) {
    EXPECT_EQ(true, true);
}

TEST(My, false_simple) {
    EXPECT_EQ(10, 95);
}

TEST(My, trim) {
    PIString str = "\rabc\r";
    str.trim();
    piCout << str;
    EXPECT_TRUE(PIString("abc") == str);
}