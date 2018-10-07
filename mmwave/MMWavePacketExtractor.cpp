//
// Created by Stephan on 15.11.2017.
//

#include <cstdint>
#include "mmw/mmw_output.h"
#include "MMWavePacketExtractor.h"

static uint16_t requiredMagicWord[4] = {0x0102, 0x0304, 0x0506, 0x0708};
PIByteArray requiredMagicWordBA(requiredMagicWord, sizeof(requiredMagicWord));

void MMWavePacketExtractor::onData(uchar *data, int size) {
	byteArray.append(data, size);
//	piCout << byteArray.size();
//	while (byteArray.size() >= 8) {
//		PIByteArray magicWord = PIByteArray(byteArray.data(0), 8);
//		piCout << magicWord.toHex();
//		if (magicWord == requiredMagicWordBA) {
//			piCout << "!!!!find!!!!";
//			byteArray.remove(0, 8);
//		}
//		byteArray.take_front();
//	}
	bool stop = false;
	while (!stop) {
		switch (condition) {
			case WAIT_MAGIC_WORLD:
				while (byteArray.size() >= 8) {
					PIByteArray magicWord = PIByteArray(byteArray.data(0), 8);
					if (magicWord == requiredMagicWordBA) {
						condition = WAIT_HEADER;
						//piCout << "find";
						break;
					} else {
						byteArray.take_front();
					}
				}
				if (byteArray.size() < 8) stop = true;
			break;
			case WAIT_HEADER:
				if (byteArray.size() >= sizeof(MmwDemo_output_message_header)) {
					memcpy(&msg_header, byteArray.data(0), sizeof(MmwDemo_output_message_header));
//					msg_header = (MmwDemo_output_message_header*)byteArray.data(0);
					//piCout << "header" << msg_header.totalPacketLen << msg_header.frameNumber << msg_header.subFrameNumber << msg_header.numDetectedObj << msg_header.version;
					condition = WAIT_PACK_END;
				} else {stop = true;}
			break;
			case WAIT_PACK_END:
				if (byteArray.size() >= msg_header.totalPacketLen) {
					packetMutex.lock();
					packetQueue.push_back(PIByteArray(byteArray.data(0), msg_header.totalPacketLen));
					packetMutex.unlock();

					byteArray.remove(0, msg_header.totalPacketLen);
					condition = WAIT_MAGIC_WORLD;
					//piCout << "get packet";
				} else {stop = true;}

//				onPacket();

				break;
			default:
				piCoutObj << "Error invalid condition";
		}
	}
}

bool MMWavePacketExtractor::hasNextPacket() {
	packetMutex.lock();
	bool hasNext = !packetQueue.isEmpty();
	packetMutex.unlock();

	return hasNext;
}

PIByteArray MMWavePacketExtractor::nextPacket() {
	PIByteArray nextPacket;

	packetMutex.lock();
	nextPacket = packetQueue.take_front();
	packetMutex.unlock();

	return nextPacket;
}

MMWavePacketExtractor::MMWavePacketExtractor() {
	piCout << "MMWavePacketExtractor initialized";
}
