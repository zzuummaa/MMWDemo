//
// Created by Stephan on 11.10.2017.
//

#ifndef SMSDK_MMWAVEDEVICE_H
#define SMSDK_MMWAVEDEVICE_H

#include <memory>
#include <piserial.h>

#include "mmw/detected_obj.h"
#include "mmw/mmw_messages.h"
#include "mmw/sys_common.h"
#include "MMWaveProperties.h"
#include "MMWavePacketExtractor.h"
#include "MMWaveDataContainer.h"


class MMWaveDevice : public PIThread {
PIOBJECT_SUBCLASS(MMWaveDevice, PIThread)
public:

	enum State {
		START,
		CONFIGURE,
		READ_DATA,
		STOP,
		ERROR
	};

	MMWaveDevice();

    MMWaveDevice(PIIODevice* configPort_, PIIODevice* dataPort_);

	MMWaveDevice(const PIString &configPortPath, const PIString &dataPortPath);

	void setConfig(const PIString& config);

	bool startDev();
	bool stopDev();

	MMWavePacketExtractor &getPacketExtractor();

    PIIODevice* getConfigPort();
	PIIODevice* getDataPort();

    State getState();
	EVENT1(onStateChange, MMWaveDevice::State, state);
protected:
	void run() override;

private:
	State state;
	PIMutex mutex;
	PIString config;
	MMWavePacketExtractor packetExtractor;
	std::unique_ptr<PIIODevice> configPort;
	std::unique_ptr<PIIODevice> dataPort;

    void onStart();
	void onConfigure();
	void onReadData();
	void onStop();

    void setState(State state);
	bool execDeviceCommand(PIString inStr, PIStringList* response = nullptr);
};

#endif //SMSDK_MMWAVEDEVICE_H
