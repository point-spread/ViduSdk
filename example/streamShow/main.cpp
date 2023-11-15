/*
 * @Author: Kian Liu
 * @Date: 2022-04-13 02:52:01
 * @LastEditTime: 2023-08-24 20:05:36
 * @LastEditors: liyang-leon9 liyang@pointspread.cn
 * @Description:
 * @FilePath: /DYV_SDK/utility/streamShow/main.cpp
 */
#include "Vidu_SDK_Api.h"
#include <chrono>
#include <thread>

int rgbDemo()
{
    PDdevice devInst;

    if (devInst.init())
    {
        auto stream = PDstream(devInst, "RGB");
        if (stream.init())
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
                char key = 0;
                if (frame)
                {
                    const cv::Mat &rgb = frame->getMat(0);
                    cv::imshow("rgb", rgb);
                    key = cv::waitKey(1);
                    if (key == 'c')
                    {
                        GenTL::PDBufferSave(*frame, nullptr, 0);
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
        else
        {
            printf("stream init failed\n");
        }
    }
    else
    {
        printf("no valid device\n");
    }
    return false;
}

int tofDemo()
{
    PDdevice devInst;
    if (devInst.init())
    {
        auto stream = PDstream(devInst, "ToF");
        if (stream.init())
        {
            bool isAutoExposure = false;
            stream.get("AutoExposure", isAutoExposure);
            if (isAutoExposure)
            {
                stream.set("AutoExposure", false);
            }
            stream.set("Distance", 2.5f);
            stream.set("StreamFps", 30.0f);
            stream.set("Threshold", 100);
            stream.set("Exposure", 1.0f);

            float DistRange = 0.0f;
            while (1)
            {
                auto frame = stream.waitFrames();
                char key = 0;
                if (frame)
                {
                    if (DistRange < 1e-5) // DistRange should be inited
                    {
                        size_t varSize = sizeof(varSize);
                        GenTL::PDBufferGetMetaByName(frame->getPort(), "Range", nullptr, &DistRange, &varSize);
                        // printf("max distance %f for distanceMap", DistRange);
                    }

                    // uint64_t usseTimer;
                    // GenTL::PDBufferGetMetaByName(pPclFrame->getPort(), "##LT##TimeStampUSSE", nullptr,
                    // &DistRange,
                    //                              &varSize);
                    // printf("TimeStampUSSE %ld timer from epoch \n", usseTimer);

                    const cv::Mat &pha = frame->getMat(0);
                    const cv::Mat &infrared = frame->getMat(1);
                    cv::imshow("pha", pha);
                    cv::imshow("infrared", infrared);
                    key = cv::waitKey(1);
                    if (key == 'q')
                    {
                        break;
                    }
                    if (key == 'c')
                    {
                        GenTL::PDBufferSave(*frame, nullptr, 0);
                        printf("saved  \n");
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            return true;
        }
        else
        {
            printf("stream init failed\n");
        }
    }
    else
    {
        printf("no valid device\n");
    }
    return false;
}

int pclDemo()
{
    PDdevice devInst;
    if (devInst.init())
    {
        auto pclstream = PDstream(devInst, "PCL");

        if (pclstream.init())
        {
            bool has_rgb = false;
            bool isRGBAutoExposure = false;
            if (devInst.getStreamNum() > 2)
            {
                pclstream.get("RGB::AutoExposure", isRGBAutoExposure);
                if (isRGBAutoExposure)
                {
                    pclstream.set("RGB::AutoExposure", false);
                }
                pclstream.set("RGB::Exposure", 10.0f);
                pclstream.set("RGB::Gain", 20.0f);

                has_rgb = true;
            }

            bool isTofAutoExposure = false;
            pclstream.get("ToF::AutoExposure", isTofAutoExposure);
            if (isTofAutoExposure)
            {
                pclstream.set("ToF::AutoExposure", false);
            }
            pclstream.set("ToF::Distance", 2.5f);
            pclstream.set("ToF::StreamFps", 30.0f);
            pclstream.set("ToF::Threshold", 100);
            pclstream.set("ToF::DepthFlyingPixelRemoval", 1);
            pclstream.set("ToF::Exposure", 1.0f);
            // pclstream.set("ToF::DistortRemove", true);

            pclstream.set("PCL::PCLFlyingPixelRemoval", 0.0f);
            pclstream.set("PCL::EnableRgbAttach", true);
            pclstream.set("PCL::EnableOcclusion", false);
            // pclstream.set("PCL::DepthAlign2Color", false);

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
                        GenTL::PDBufferGetMetaByName(pPclFrame->getPort(), "Range", nullptr, &DistRange, &varSize);
                        printf("max distance %f for distanceMap\n", DistRange);
                    }

                    // uint64_t usseTimer;
                    // GenTL::PDBufferGetMetaByName(pPclFrame->getPort(), "##LT##TimeStampUSSE", nullptr,
                    // &DistRange,
                    //                              &varSize);
                    // printf("TimeStampUSSE %ld timer from epoch \n", usseTimer);

                    const cv::Mat &xyz = pPclFrame->getMat(0);
                    const cv::Mat &infrared = pPclFrame->getMat(1);

                    std::vector<cv::Mat> channels(3);
                    cv::split(xyz, channels);
                    const cv::Mat &depth = channels[2];
                    cv::Mat u16Depth;
                    depth.convertTo(u16Depth, CV_16UC1, 65535.0 / DistRange);

                    cv::imshow("xyz", xyz);
                    cv::imshow("infrared", infrared);
                    cv::imshow("depth", depth);

                    if (has_rgb)
                    {
                        const cv::Mat &alignedColor = pPclFrame->getMat(2);
                        const cv::Mat &originColor =
                            pPclFrame->getMat(3); // should pclstream.set("PCL::EnableRgbAttach", true);
                        cv::imshow("alignedColor", alignedColor);
                        cv::imshow("originColor", originColor);
                    }

                    if (saveReq)
                    {
                        saveReq = false;
                        GenTL::PDBufferSave(*pPclFrame, nullptr,
                                            0); // 1 save ply with color, 2 save ply with color patch
                        cv::imwrite(stringFormat("depth-%d.png", count), u16Depth);
                        printf("saved  \n");
                        count++;
                    }
                }
                pPclFrame.reset(); // plz. release frame buffer imediately
                // std::this_thread::sleep_for();
            }
            return true;
        }
        else
        {
            printf("stream init failed\n");
        }
    }
    else
    {
        printf("no valid device\n");
    }
    return false;
}

int main()
{
    // return rgbDemo();
    // return tofDemo();
    return pclDemo();
}
