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

class MMWaveDevice : public PIObject {
PIOBJECT(MMWaveDevice)
public:
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


	bool open();
	bool close();
	bool isOpened();
	bool start();

	bool waitForFinish(int time_out = -1);
	void lock();
	void unlock();

	bool isReadyData();
	MMWaveDataContainer getData();
	void clearDataQueue();

	std::shared_ptr<PIIODevice> getConfigPort();
	std::shared_ptr<PIIODevice> getDataPort();

	bool execDeviceCommand(PIString inStr);

	EVENT_HANDLER0(void, interrupt);

	EVENT(threadStop)

private:
	MMWavePacketExtractor packetExtractor;
	PIThread thread;
	std::shared_ptr<PIIODevice> configPort;
	std::shared_ptr<PIIODevice> dataPort;
	PIQueue<MMWaveDataContainer> dataQueue;
	bool isOpen;
	PIMutex dataMutex;
	PIMutex deviceMutex;

	EVENT_HANDLER0(void, run);
};

#endif //SMSDK_MMWAVEDEVICE_H
