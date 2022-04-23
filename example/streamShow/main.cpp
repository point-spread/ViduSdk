/*
 * @Author: Kian Liu
 * @Date: 2022-04-13 02:52:01
 * @LastEditTime: 2022-04-22 17:34:23
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /OkuloSdk/example/streamShow/main.cpp
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
            while (1)
            {
                auto frame = stream.waitFrames();
                char key = cv::waitKey(1);
                if (frame)
                {
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
            bool saveReq = false;
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
                    const cv::Mat &dist = pPclFrame->getMat(0);
                    const cv::Mat &infrared = pPclFrame->getMat(1);
                    const cv::Mat &color = pPclFrame->getMat(2);
                    cv::imshow("dist", dist);
                    cv::imshow("infrared", infrared);
                    cv::imshow("color", color);
                    if (saveReq)
                    {
                        saveReq = false;
                        GenTL::PDBufferSave(*pPclFrame, nullptr);
                        printf("saved  \n");
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
