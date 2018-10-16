//
// Created by Stephan on 15.11.2017.
//

#include <cstdint>
#include "MMWavePacketExtractor.h"

PIByteArray requiredMagicWordBA(requiredMagicWord, sizeof(requiredMagicWord));

void MMWavePacketExtractor::onData(uchar *data, int size) {
	byteArray.append(data, size);
	bool isAllDataHandled = false;
	while (!isAllDataHandled) {
		switch (condition_) {
			case WAIT_MAGIC_WORLD:
				while (byteArray.size() >= 8) {
					PIByteArray magicWord = PIByteArray(byteArray.data(0), 8);
					if (magicWord == requiredMagicWordBA) {
						condition_ = WAIT_HEADER;
						//piCout << "find";
						break;
					} else {
						byteArray.take_front();
					}
				}
				if (byteArray.size() < 8) isAllDataHandled = true;
			break;
			case WAIT_HEADER:
				if (byteArray.size() >= sizeof(MMWMessage::Header)) {
					memcpy(&msg_header, byteArray.data(0), sizeof(MMWMessage::Header));
					condition_ = WAIT_PACK_END;
				} else {
					isAllDataHandled = true;
				}
			break;
			case WAIT_PACK_END:
				if (byteArray.size() >= msg_header.totalPacketLen) {
					packetMutex.lock();
					if (packetQueue.size() >= packetQueueCapacity) packetQueue.take_front();
					packetQueue.push_back(PIByteArray(byteArray.data(0), msg_header.totalPacketLen));
					if (packetQueueM.size() >= packetQueueCapacity) packetQueueM.take_front();
					packetQueueM.push_back(MMWMessage(byteArray.data(), msg_header.totalPacketLen));
					packetMutex.unlock();

					byteArray.remove(0, msg_header.totalPacketLen);
					condition_ = WAIT_MAGIC_WORLD;
				} else {
					isAllDataHandled = true;
				}

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

bool MMWavePacketExtractor::hasNextPacketM() {
	packetMutex.lock();
	bool hasNext = !packetQueueM.isEmpty();
	packetMutex.unlock();
	return hasNext;
}

MMWMessage MMWavePacketExtractor::nextPacketM() {
	packetMutex.lock();
	MMWMessage nextPacket = packetQueueM.take_front();
	packetMutex.unlock();
	return nextPacket;
}

MMWavePacketExtractor::MMWavePacketExtractor(): condition_(WAIT_MAGIC_WORLD), packetQueueCapacity(10) {}

MMWavePacketExtractor::Condition MMWavePacketExtractor::condition() const {
	return condition_;
}
