//
// Created by Stephan on 25.09.2018.
//

#include <smbrickbase.h>
#include <smshema.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "mmwave2/MMWMessage.h"
#include "mmwave2/Utils.h"
#include "plugin/MMWaveDemoInputMOC.h"

static MMWaveDemoInputMOC plugin;
static PIPropertyStorage propertyStorage;
static PIVector<SMBase::BlockPin> pinsOut;
static SMShema shema;
static SMBrickBase* block = nullptr;

TEST(sm_block, create_block) {
    pinsOut = plugin.pinsOut(propertyStorage);
    block = shema.addBlock("MMW"+plugin.name(), plugin.version());
    ASSERT_FALSE(block == nullptr);
}

TEST(sm_block, plugin_start) {
    plugin.start(block);
}

TEST(sm_block, plugin_get) {
    SMBlockData blockData;
    for (int i = 0; i < pinsOut.size(); ++i) {
        blockData.push_back(SMBusData());
    }

    ASSERT_TRUE(plugin.isReady(block));
    plugin.get(block, blockData);
}

TEST(sm_block, plugin_end) {
    ASSERT_TRUE(plugin.isEnd(block));
    plugin.stop(block);
}