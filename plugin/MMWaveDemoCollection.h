//
// Created by Stephan on 11.09.2018.
//

#ifndef MMWAVEDEMO_MMWAVEDEMOCOLLECTION_H
#define MMWAVEDEMO_MMWAVEDEMOCOLLECTION_H

#include <sm_base.h>
#include <sm_plugins.h>
#include "MMWaveDemoInputMOC.h"

class MMWaveDemoCollection: public SMPluginCollection {
public:
    virtual PIVector<SMPluginBase *> getPluginCollection() {
        PIVector<SMPluginBase *> ret;
        ret << new MMWaveDemoInputMOC();
        return ret;
//		return PIVector<SMPluginBase *>();
    }
    virtual PIString name() {return "MMWave";}
};


#endif //MMWAVEDEMO_MMWAVEDEMOCOLLECTION_H
