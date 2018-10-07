//
// Created by Stephan on 11.09.2018.
//

#ifndef MMWAVEDEMO_MMWAVEDEMOINPUTMOC_H
#define MMWAVEDEMO_MMWAVEDEMOINPUTMOC_H

#include <sm_base.h>
#include <sm_plugins.h>

class MMWaveDemoInputMOC: public SMPluginBaseInput, PIObject {
PIOBJECT(MMWaveDemoInputMOC)
public:
    MMWaveDemoInputMOC() {
        piCout << "MMWaveDemoInput initialized";
    }
    virtual PIString name() const {return "MMWaveDemoInputMOC";}
    virtual PIString description() const {return "This plugin provides communication with the MMWave device";}
    virtual int version() const {return 3;}
    virtual PIVector<SMBase::BlockPin> pinsOut(const PIPropertyStorage & props);

    void start(SMBrickBase *block);

    void stop(SMBrickBase *block);

    virtual bool isReady(const SMBrickBase *block);

    virtual bool isEnd(const SMBrickBase * block);

    virtual void get(SMBrickBase * block, SMBlockData & out);

    virtual ~MMWaveDemoInputMOC() { }
};

#endif //MMWAVEDEMO_MMWAVEDEMOINPUTMOC_H
