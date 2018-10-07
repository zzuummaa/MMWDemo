//
// Created by Stephan on 14.02.2018.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

TEST(My, simple) {
    EXPECT_EQ(true, true);
}

TEST(My, false_simple) {
    EXPECT_EQ(10, 95);
}