//
// Created by Stephan on 30.05.2018.
//

#ifndef MMWAVEDEMO_MMWAVECONFIGURATION_H
#define MMWAVEDEMO_MMWAVECONFIGURATION_H

#include <cstdint>
#include <cstring>
#include <pistring.h>

class MMWaveConfig {

public:

    PIString toStr();
    void fromStr(const PIString& str);

    void addCommand(PIString& str);
    PIString getCommand(int i);
    PIString getCommandName(int i);
    PIString getCommandParams(int i);

    inline int getCommandCount() { return commands.size(); }

    inline PIDeque<PIString>::iterator iterator() { return commands.begin(); }
    inline PIDeque<PIString>::iterator end() { return commands.end(); }

    inline PIString getStartCommand() { return "sensorStart\n"; }
    inline PIString getStopCommand()  { return "sensorStop\n";  }

    MMWaveConfig() {}

    virtual ~MMWaveConfig() {}

private:
    PIStringList commands;
};


#endif //MMWAVEDEMO_MMWAVECONFIGURATION_H
