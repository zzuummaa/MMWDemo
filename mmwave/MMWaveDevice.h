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

typedef PIVector<MmwDemo_detectedObj> DetObjFrame;

/**
 * @brief Parses MMWaveDemo output UART packet and fill message.
 *
 * Adds to message references to buff.
 *
 * @param buff
 * @param message
 * @return true  if parse buff success
 *         false if parse buff failure
 */
bool parsePacket(uint8_t* buff, MmwDemo_detInfoMsg* message);

/**
 * Parses detected objects from tlv.
 *
 * @param tlv - subpacket with type = MMWDEMO_OUTPUT_MSG_DETECTED_POINTS from mmw_output.h
 * @return list of objects from tlv
 */
DetObjFrame parseDetectedObj(MmwDemo_msgTlv* tlv);
void freeMemory(MmwDemo_message message);


class MMWaveDevice : public PIThread {
PIOBJECT(MMWaveDevice)
public:

	enum State {
		START,
		CONFIGURE,
		READ_DATA,
		STOP,
		ERROR
	};

	MMWaveDevice();

	MMWaveDevice(const PIString &configPortPath, const PIString &dataPortPath);

	/**
	 * @brief Send to device config string.
	 *
	 * @param config - string with special format for MMWaveDevice
	 * @return true if config successfully sent
	 */
	bool configureDevice(PIString& config);
	//bool configureDevice(PIString  config) { configureDevice() }

	/**
	 * @bref Send to device start command
	 *
	 * @return true if command successfully sent
	 */
	bool configureToStart();

	/**
	 * @brief Send to device stop command
	 *
	 * @return true if command successfully sent
	 */
	bool configureToStop();

	void setConfig(const PIString& config);

	bool startDev();
	bool stopDev();

	MMWavePacketExtractor &getPacketExtractor();

	std::shared_ptr<PIIODevice> getConfigPort();
	std::shared_ptr<PIIODevice> getDataPort();

	bool execDeviceCommand(PIString inStr);

    State getState();

protected:
	void run() override;

private:
	State state;
	PIMutex mutex;
	PIString config;
	MMWavePacketExtractor packetExtractor;
	std::shared_ptr<PIIODevice> configPort;
	std::shared_ptr<PIIODevice> dataPort;
	bool isOpen;

	void onStart();
	void onConfigure();
	void onReadData();
	void onStop();

    void setState(State state);
};

#endif //SMSDK_MMWAVEDEVICE_H
