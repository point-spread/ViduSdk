/*
 * @Author: Kian Liu
 * @Date: 2022-04-13 02:52:01
 * @LastEditTime: 2022-06-06 19:33:51
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/utility/streamShow/main.cpp
 */
#include "inc/PDdevice.h"
#include "inc/PDstream.h"
#include "stringFormat.h"
#include "timeTest.h"
#include <chrono>
#include <thread>

int rgbDemo()
{
    PDdevice devInst;

    if (devInst)
    {
        auto stream = PDstream(devInst, "RGB");
        if (stream)
        {
            bool isAutoExposure = false;
            stream.get("AutoExposure", isAutoExposure);
            if (isAutoExposure)
            {
                stream.set("AutoExposure", false);
            }
            stream.set("Exposure", 1.0f);
            stream.set("Gain", 1.0f);
            while (1)
            {
                auto frame = stream.waitFrames();
                char key = cv::waitKey(1);
                if (frame)
                {
                    const cv::Mat &rgb = frame->getMat(0);
                    cv::imshow("rgb", rgb);

                    if (key == 'c')
                    {
                        GenTL::PDBufferSave(*frame, nullptr);
                        printf("saved  \n");
                    }
                }
                if (key == 'q')
                {
                    break;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            return true;
        }
    }
    return false;
}

int tofDemo()
{
    PDdevice devInst;
    if (devInst)
    {
        auto stream = PDstream(devInst, "ToF");
        if (stream)
        {
            bool isAutoExposure = false;
            stream.get("AutoExposure", isAutoExposure);
            if (isAutoExposure)
            {
                stream.set("AutoExposure", false);
            }
            stream.set("Distance", 7.5f);
            stream.set("StreamFps", 50.0f);
            stream.set("Threshold", 100);
            stream.set("Exposure", 1.0f);

            float DistRange = 0.0f;
            while (1)
            {
                auto frame = stream.waitFrames();

                char key = cv::waitKey(1);
                if (frame)
                {
                    if (DistRange < 1e-5) // DistRange should be inited
                    {
                        size_t varSize = sizeof(varSize);
                        GenTL::PDBufferGetMetaByName(frame->getPort(), "Range", &DistRange, &varSize, nullptr);
                        printf("max distance %f for distanceMap", DistRange);
                    }
                    const cv::Mat &pha = frame->getMat(0);
                    const cv::Mat &infrared = frame->getMat(1);
                    cv::imshow("pha", pha);
                    cv::imshow("infrared", infrared);
                    if (key == 'q')
                    {
                        break;
                    }
                    if (key == 'c')
                    {
                        GenTL::PDBufferSave(*frame, nullptr);
                        printf("saved  \n");
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            return true;
        }
    }
    return false;
}

int pclDemo()
{
    PDdevice devInst;
    if (devInst)
    {
        auto pclstream = PDstream(devInst, "PCL");

        if (pclstream)
        {

            bool isTofAutoExposure = false;
            bool isRGBAutoExposure = false;
            pclstream.get("ToF::AutoExposure", isTofAutoExposure);
            pclstream.get("RGB::AutoExposure", isRGBAutoExposure);
            if (isTofAutoExposure)
            {
                pclstream.set("ToF::AutoExposure", false);
            }
            if (isRGBAutoExposure)
            {
                pclstream.set("RGB::AutoExposure", false);
            }

            pclstream.set("ToF::Distance", 7.5f);
            pclstream.set("ToF::StreamFps", 50.0f);
            pclstream.set("ToF::Threshold", 100);
            pclstream.set("ToF::Exposure", 1.0f);
            pclstream.set("RGB::Exposure", 10.0f);
            pclstream.set("RGB::Gain", 20.0f);

            bool saveReq = false;
            int count = 0;
            float DistRange = 0.0f;
            while (1)
            {
                auto pPclFrame = pclstream.waitFrames(); // pcl frames work only when tof && rgb stream grab frames!!!
                char key = cv::waitKey(1);
                if (key == 'c')
                {
                    saveReq = true;
                }
                if (key == 'q')
                {
                    break;
                }
                if (pPclFrame)
                {
                    if (DistRange < 1e-5) // DistRange should be inited
                    {
                        size_t varSize = sizeof(varSize);
                        GenTL::PDBufferGetMetaByName(pPclFrame->getPort(), "Range", &DistRange, &varSize, nullptr);
                        printf("max distance %f for distanceMap\n", DistRange);
                    }
                    const cv::Mat &xyz = pPclFrame->getMat(0);
                    const cv::Mat &infrared = pPclFrame->getMat(1);
                    const cv::Mat &color = pPclFrame->getMat(2);

                    std::vector<cv::Mat> channels(3);
                    cv::split(xyz, channels);
                    const cv::Mat &depth = channels[2];
                    cv::Mat u16Depth;
                    depth.convertTo(u16Depth, CV_16UC1, 65535.0 / DistRange);

                    cv::imshow("xyz", xyz);
                    cv::imshow("infrared", infrared);
                    cv::imshow("color", color);
                    cv::imshow("depth", depth);

                    if (saveReq)
                    {
                        saveReq = false;
                        GenTL::PDBufferSave(*pPclFrame, nullptr);
                        cv::imwrite(stringFormat("depth-%d.png", count), u16Depth);
                        printf("saved  \n");
                        count++;
                    }
                }
                pPclFrame.reset(); // plz. release frame buffer imediately
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            return true;
        }
    }
    return false;
}

int main()
{
    // return rgbDemo();
    // return tofDemo();
    return pclDemo();
}
