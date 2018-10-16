//
// Created by Stephan on 15.11.2017.
//

#ifndef MMWAVEDEMO_MMWAVEPACKETEXTRACTOR_H
#define MMWAVEDEMO_MMWAVEPACKETEXTRACTOR_H

#include <piobject.h>
#include <pibytearray.h>
#include "MMWMessage.h"

class MMWavePacketExtractor : public PIObject {
	PIOBJECT(MMWavePacketExtractor)

public:
	MMWavePacketExtractor();

	/**
	 * @brief
	 * Receive parts of data and split it to packets
	 *
	 * Method has 3 conditions:
	 * WAIT_MAGIC_WORLD - on waiting valid magic word from data
	 * WAIT_HEADER - when has valid magic word but not all header received
	 * WAIT_PACK_END - when has valid magic word and header, and waiting for end of packet
	 *
	 * when the end of the packet is received, it is called onPacket()
	 */
	EVENT_HANDLER2(void, onData, uchar*, data, int, size);

	//EVENT0(onPacket)

	PIByteArray nextPacket();
	bool hasNextPacket();

	MMWMessage nextPacketM();
	bool hasNextPacketM();

	enum Condition {
		WAIT_MAGIC_WORLD,
		WAIT_HEADER,
		WAIT_PACK_END
	};

	Condition condition() const;

private:
	Condition condition_;
	PIQueue<PIByteArray> packetQueue;
	PIQueue<MMWMessage> packetQueueM;
	int packetQueueCapacity;
	PIByteArray byteArray;
	MMWMessage::Header msg_header;
	PIMutex packetMutex;
};


#endif //MMWAVEDEMO_MMWAVEPACKETEXTRACTOR_H
