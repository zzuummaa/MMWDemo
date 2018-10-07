//
// Created by Stephan on 23.07.2018.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "mmwave/MMWaveDevice.h"
#include <climits>
#include "mmwave/mmw/detected_obj.h"

MMWaveDevice device;
const long DELAY_BETWEEN_CHECK_FRAMES = 10;

inline PICout operator <<(PICout s, const PIVector<trackingReportForTx> & trackReport) {
    PIString str;
    for (int i = 0; i < trackReport.size_s(); ++i) {
        str << "x: "   << PIString::fromNumber(trackReport[i].x / 128., 'f', 3)
            << " y: "   << PIString::fromNumber(trackReport[i].y / 128., 'f', 3)
            << " vel: " << PIString::fromNumber(trackReport[i].yd / 128., 'f', 3) << "\n";
    }
    str.pop_back();
    s << str;
    return s;
}

inline PICout operator <<(PICout s, const PIVector<PIVector<float>> & rangeDoppler) {
    s << rangeDoppler;
    return s;
}

TEST(DeviceControl, start_invalid_ports) {
    ASSERT_FALSE(device.start());
}

TEST(DeviceControl, start) {
    device.getConfigPort()->configureFromFullPath("COM3:115200");
    device.getDataPort()->("COM4:921600");
    ASSERT_TRUE(device.start());
}

TEST(DeviceControl, receive_first_frame) {
    long passedTime = 0;
    long maxDelayMS = 1500;
    bool isReady = false;

    while (!isReady && passedTime < maxDelayMS) {
        device.lock();
        isReady = device.isReadyData();
        device.unlock();
        msleep(DELAY_BETWEEN_CHECK_FRAMES);
        passedTime += DELAY_BETWEEN_CHECK_FRAMES;
    }

    ASSERT_TRUE(isReady);
}

TEST(DeviceControl, receive_until) {
    int i = 0;
    int framesReceived = 40;
    long timeout = 3000;

    if (framesReceived == 0) {
        framesReceived = INT_MAX;
    }

    if (timeout == 0) {
        timeout = LONG_MAX;
    }

    while (timeout > 0 && i < framesReceived) {
        device.lock();
        while (device.isReadyData()) {
            MMWaveDataContainer cont = device.getData();

            if (cont.trackReport.size() > 0) {
                piCout << "TrackingReport";
                piCout << cont.trackReport;
                piCout << "";
                i++;
            }
            if (cont.rangeDopplerHeatMap.size() > 0) {
                piCout << "RangeDopplerHeatMap:";
                piCout << cont.rangeDopplerHeatMap;
                piCout << " ";
                i++;
            }
        }
        device.unlock();
        msleep(DELAY_BETWEEN_CHECK_FRAMES);
        timeout -= DELAY_BETWEEN_CHECK_FRAMES;
    }

    ASSERT_EQ(i, framesReceived);
    ASSERT_GT(timeout, 0);
}

TEST(DeviceControl, stop) {
    device.interrupt();
    ASSERT_TRUE(device.waitForFinish(100));
    ASSERT_TRUE(device.close());
}

TEST(DeviceControl, restart) {
    for (int i = 0; i < 10; ++i) {
        piCout << "Restart #№" << i;
        ASSERT_TRUE(device.start());

        long passedTime = 0;
        long maxDelayMS = 1500;
        bool isReady = false;

        while (!isReady && passedTime < maxDelayMS) {
            device.lock();
            isReady = device.isReadyData();
            device.unlock();
            msleep(DELAY_BETWEEN_CHECK_FRAMES);
            passedTime += DELAY_BETWEEN_CHECK_FRAMES;
        }

        ASSERT_TRUE(isReady);

        device.interrupt();
        ASSERT_TRUE(device.waitForFinish(100));
        ASSERT_TRUE(device.close());

        piCout << "";
    }
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}