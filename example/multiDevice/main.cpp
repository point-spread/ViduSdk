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
#include <thread>

void showFrame(std::shared_ptr<PDbuffer> frame, int streamID, bool saveReq)
{
    if (frame)
    {
        const cv::Mat &pha = frame->getMat(0);
        const cv::Mat &infrared = frame->getMat(1);
        cv::imshow(stringFormat("pha%d", streamID).c_str(), pha);
        cv::imshow(stringFormat("infrared%d", streamID).c_str(), infrared);
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
    if (devSNlist.size() >= 2)
    {
        PDdevice devInst1(devSNlist[0]);
        PDdevice devInst2(devSNlist[1]);

        if (devInst1 && devInst2)
        {
            auto tofStream1 = PDstream(devInst1, "ToF");
            auto tofStream2 = PDstream(devInst2, "ToF");

            if (tofStream1 && tofStream2)
            {
                tofStream1.set("Distance", 7.5f);
                tofStream2.set("Distance", 7.5f);

                tofStream1.set("StreamFps",
                               60); // values below set here to avoid interference may refer to values in viewer
                                    // (version with auto sync functionality)
                tofStream2.set("StreamFps", 60);

                tofStream1.set("Exposure", 1.0f);
                tofStream2.set("Exposure", 1.0f);

                tofStream1.set("ExpoNpha", 0.5f);
                tofStream2.set("ExpoNpha", 0.5f);

                while (1)
                {
                    auto frame1 = tofStream1.waitFrames();
                    auto frame2 = tofStream2.waitFrames();
                    char key = cv::waitKey(1);
                    if (key == 'q')
                        break;
                    showFrame(frame1, 1, key == 'c');
                    showFrame(frame2, 2, key == 'c');

                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
                return true;
            }
        }
    }
    else
    {
        printf("two device needed\n");
    }
    return false;
}

int main()
{
    return multiDeviceDemo();
}
