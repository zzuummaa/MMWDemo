//
// Created by Stephan on 21.06.2018.
//

#include <pifile.h>
#include "mmwave/MMWaveDevice.h"
#include "mmwave2/MMWPacketExtractor.h"
#include "mmwave2/MMWMessage.h"
#include "mmwave2/Utils.h"
#include "pipacketextractor.h"

class PacketDisplay : public PIObject {
    PIOBJECT(PacketDisplay)
public:
    EVENT_HANDLER2(void, run, uchar *, data, int, size) {
        if (size == 0) return;
        piCout << PIByteArray(data, size).toHex();
    }
};

int main() {
    PIByteArray ar = generateMMWPacket();
    PIFile outFile("packet.txt");
    if (outFile.open()) {
        outFile.write(ar);
        outFile.close();
    } else {
        return 1;
    }

    PIFile file("packet.txt");
    PIPacketExtractor packetExtractor(&file, PIPacketExtractor::SplitMode::Header);
    packetExtractor.setHeader(PIByteArray(magicWord, 8));
    PacketDisplay packetDisplay;
    CONNECTU(&packetExtractor, packetReceived, &packetDisplay, run);

    if (!packetExtractor.open()) {
        piCout << "error";
        return 1;
    }

    packetExtractor.startThreadedRead();
    piSleep(2);

    return 0;
}