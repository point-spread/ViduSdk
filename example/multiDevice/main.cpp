/*
 * @Author: Kian Liu
 * @Date: 2022-04-13 02:52:01
 * @LastEditTime: 2022-06-06 19:33:51
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/utility/streamShow/main.cpp
 */
#include "Vidu_SDK_Api.h"
#include <chrono>
#include <opencv2/opencv.hpp>
#include <thread>

void showFrame(std::shared_ptr<PDbuffer> frame, int streamID, bool saveReq)
{
    if (frame)
    {
        const PDimage &pha = frame->getImage(0);
        const PDimage &infrared = frame->getImage(1);

        cv::Mat pha_mat =
            cv::Mat(cv::Size(pha.GetImageWidth(), pha.GetImageHeight()), pha.GetImageCVType(), pha.GetImageData());
        cv::Mat infrared_mat = cv::Mat(cv::Size(infrared.GetImageWidth(), infrared.GetImageHeight()),
                                       infrared.GetImageCVType(), infrared.GetImageData());

        cv::imshow(stringFormat("pha%d", streamID).c_str(), pha_mat);
        cv::imshow(stringFormat("infrared%d", streamID).c_str(), infrared_mat);

        if (saveReq)
        {
            GenTL::PDBufferSave(*frame, nullptr, 0);
            printf("stream %d saved\n", streamID);
        }
    }
}

int multiDeviceDemo()
{
    auto devSNlist = getDeviceSerialsNumberList();
    if (devSNlist.size() < 2)
    {
        printf("two device needed\n");
        return -1;
    }

    PDdevice devInst1(devSNlist[0]);
    PDdevice devInst2(devSNlist[1]);

    if (!devInst1.init() || !devInst2.init())
    {
        printf("device initialization failed\n");
        return -1;
    }

    auto tofStream1 = PDstream(devInst1, "ToF");
    auto tofStream2 = PDstream(devInst2, "ToF");

    if (!tofStream1.init() || !tofStream2.init())
    {
        printf("stream initialization failed\n");
        return -1;
    }

    // values below set here to avoid interference may refer to values in viewer(version with auto sync functionality)
    // Set to the same distance, with different modulation frequencies for different distances
    tofStream1.set("Distance", 2.5f);
    tofStream2.set("Distance", 2.5f);

    tofStream1.set("StreamFps", 60);
    tofStream2.set("StreamFps", 60);

    tofStream1.set("Exposure", 1.0f);
    tofStream2.set("Exposure", 1.0f);

    tofStream1.set("ExpoNpha", 0.5f);
    tofStream2.set("ExpoNpha", 0.5f);

    while (1)
    {
        auto frame1 = tofStream1.waitFrames();
        auto frame2 = tofStream2.waitFrames();
        int32_t key = 0;
        key = cv::waitKey(1);
        showFrame(frame1, 1, key == 'c');
        showFrame(frame2, 2, key == 'c');
        if (key == 'q')
            break;

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}

int main()
{
    return multiDeviceDemo();
}
