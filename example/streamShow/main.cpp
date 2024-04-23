/*
 * @Author: Kian Liu
 * @Date: 2022-04-13 02:52:01
 * @LastEditTime: 2023-08-24 20:05:36
 * @LastEditors: liyang-leon9 liyang@pointspread.cn
 * @Description:
 * @FilePath: /DYV_SDK/utility/streamShow/main.cpp
 */
#include <chrono>
#include <cstdint>
#include <thread>

#include "Vidu_SDK_Api.h"

void FeatureListPrint(std::vector<FeatureNodeInfo> &feature_list)
{
    for (auto &&feature : feature_list)
    {
        std::cout << "\t" << feature.name_;
        switch (feature.type_)
        {
        case GenTL::INFO_DATATYPE_BOOL8: {
            std::cout << ", bool type, no min and max value" << std::endl;
            break;
        }
        case GenTL::INFO_DATATYPE_INT16: {
            std::cout << ", 16 bit int type, min: " << feature.min_.i16_val_ << ", max: " << feature.max_.i16_val_
                      << std::endl;
            break;
        }
        case GenTL::INFO_DATATYPE_INT32: {
            std::cout << ", 32 bit int type, min: " << feature.min_.i32_val_ << ", max: " << feature.max_.i32_val_
                      << std::endl;
            break;
        }
        case GenTL::INFO_DATATYPE_FLOAT32: {
            std::cout << ", float type, min: " << feature.min_.flt_val_ << ", max: " << feature.max_.flt_val_
                      << std::endl;
            break;
        }
        case GenTL::INFO_DATATYPE_STRING: {
            std::cout << ", enumeration type, entry size: " << feature.enum_entry_.size() << ", entry: " << std::endl;
            for (auto &&entry : feature.enum_entry_)
            {
                std::cout << "\t\t" << entry << std::endl;
            }
            break;
        }
        default: {
            std::cout << ", unknow data type: " << feature.type_ << std::endl;
            break;
        }
        }
    }
}

int rgbDemo()
{
    PDdevice devInst;

    if (!devInst.init())
    {
        printf("no valid device\n");
        return -1;
    }

    auto stream = PDstream(devInst, "RGB");
    if (!stream.init())
    {
        printf("stream init failed\n");
        return -1;
    }

    bool isAutoExposure = false;
    // Obtain whether the RGB data stream is automatic exposure
    stream.get("AutoExposure", isAutoExposure);
    if (isAutoExposure)
    {
        // If it is auto exposure, turn off auto exposure
        stream.set("AutoExposure", false);
    }
    // Obtain the list of all features supported by RGB data flow control
    auto feature_list = stream.GetFeatureList();
    std::cout << "RGB feature: " << std::endl;
    FeatureListPrint(feature_list);

    for (auto &&feature : feature_list)
    {
        // support with prefix "RGB::", set exposure to half the maximum value
        if (feature.name_.compare("Exposure") == 0)
        {
            switch (feature.type_)
            {
            case GenTL::INFO_DATATYPE_INT32: {
                // C1, exposure time unit is us and type is int
                stream.set("RGB::Exposure", feature.max_.i32_val_ / 2);
                break;
            }
            case GenTL::INFO_DATATYPE_FLOAT32: {
                // P1, exposure time unit is ms and type is float
                stream.set("RGB::Exposure", feature.max_.flt_val_ / 2.0f);
                break;
            }
            }
        }
    }
    // support without prefix, set the gain value to 1.0
    stream.set("Gain", 1.0f);

    while (1)
    {
        // Obtain RGB frame data
        auto frame = stream.waitFrames();
        char key = 0;
        if (frame)
        {
            // Obtain image data from frame data
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

    return 0;
}

int tofDemo()
{
    PDdevice devInst;
    if (!devInst.init())
    {
        printf("no valid device\n");
        return -1;
    }

    auto stream = PDstream(devInst, "ToF");
    if (!stream.init())
    {
        printf("stream init failed\n");
        return -1;
    }

    bool isAutoExposure = false;
    // support with prefix
    stream.get("ToF::AutoExposure", isAutoExposure);
    if (isAutoExposure)
    {
        stream.set("ToF::AutoExposure", false);
    }

    // support get stream feature list, type, value range and enum entry
    auto feature_list = stream.GetFeatureList();
    std::cout << "ToF feature: " << std::endl;
    FeatureListPrint(feature_list);

    for (auto &&feature : feature_list)
    {
        // support without prefix
        if (feature.name_.compare("Exposure") == 0)
        {
            switch (feature.type_)
            {
            case GenTL::INFO_DATATYPE_INT32: {
                stream.set(feature.name_.c_str(), feature.max_.i32_val_ / 2); // C1
                break;
            }
            case GenTL::INFO_DATATYPE_FLOAT32: {
                stream.set(feature.name_.c_str(), feature.max_.flt_val_ / 2.0f); // P1
                break;
            }
            }
        }
    }

    // support with prefix
    stream.set("ToF::Distance", 2.5f);
    stream.set("ToF::StreamFps", 30.0f);
    stream.set("ToF::Threshold", 100);

    // OC1 support Resolution
    // char resolution[10] = "320 x 240";
    // if (stream.set("Resolution", resolution, sizeof(resolution)))
    // {
    //     std::cout << resolution << std::endl;
    // }
    // if (stream.get("Resolution", resolution, sizeof(resolution)))
    // {
    //     std::cout << resolution << std::endl;
    // }

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
            else if (key == 'c')
            {
                GenTL::PDBufferSave(*frame, nullptr, 0);
                printf("saved  \n");
            }
            else if (key == 'm')
            {
                frame.reset();

                stream = PDstream(devInst, "ToF");
                if (!stream.init())
                {
                    return -1;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}

int ImuDemo()
{
    PDdevice dev_inst;
    if (!dev_inst.init())
    {
        printf("no valid device\n");
        return -1;
    }

    auto stream = PDstream(dev_inst, "IMU");
    if (!stream.init())
    {
        printf("stream init failed\n");
        return -1;
    }

    while (true)
    {
        auto frame = stream.waitFrames();
        char key = 0;
        if (frame)
        {
            cv::Mat blank = cv::Mat(600, 800, CV_8UC1, cv::Scalar::all(255));

            GenTL::DS_BUFFER_IMU_DATA *data = (GenTL::DS_BUFFER_IMU_DATA *)(frame->GetBuffer());
            char text[128] = {};
            sprintf(text, "accelerometer time: %lu", data->AcceTime);
            cv::putText(blank, text, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, CV_RGB(0, 0, 0), 2.0f);
            memset(text, 0x0, sizeof(text));
            sprintf(text, "x: %.6f, y: %.6f, z: %.6f", data->AcceData[0], data->AcceData[1], data->AcceData[2]);
            cv::putText(blank, text, cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 1.0, CV_RGB(0, 0, 0), 2.0f);
            memset(text, 0x0, sizeof(text));
            sprintf(text, "gyroscope time: %lu", data->GyroTime);
            cv::putText(blank, text, cv::Point(10, 90), cv::FONT_HERSHEY_SIMPLEX, 1.0, CV_RGB(0, 0, 0), 2.0f);
            memset(text, 0x0, sizeof(text));
            sprintf(text, "x: %.6f, y: %.6f, z: %.6f", data->GyroData[0], data->GyroData[1], data->GyroData[2]);
            cv::putText(blank, text, cv::Point(10, 120), cv::FONT_HERSHEY_SIMPLEX, 1.0, CV_RGB(0, 0, 0), 2.0f);
            cv::imshow("blank", blank);

            key = cv::waitKey(1);
            if (key == 'q')
            {
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}

int pclDemo()
{
    PDdevice devInst;
    if (!devInst.init())
    {
        printf("no valid device\n");
        return -1;
    }

    auto pclstream = PDstream(devInst, "PCL");

    if (!pclstream.init())
    {
        printf("stream init failed\n");
        return -1;
    }

    bool has_rgb = false;
    // Determine whether RGB exists in the PCL data stream
    if (devInst.getStreamNum() > 2)
    {
        // If the data flow is greater than 2, it indicates the presence of RGB, ToF and PCL
        bool isRGBAutoExposure = false;
        // Must with prefix "RGB::", turn off automatic exposure of RGB data stream
        pclstream.get("RGB::AutoExposure", isRGBAutoExposure);
        if (isRGBAutoExposure)
        {
            pclstream.set("RGB::AutoExposure", false);
        }
        // Set RGB gain value to 20.0
        pclstream.set("RGB::Gain", 20.0f);

        has_rgb = true;

        // Obtain the list of RGB features supported for control in the PCL data stream
        auto feature_list_rgb = pclstream.GetFeatureList("RGB");
        std::cout << "RGB feature: " << std::endl;
        FeatureListPrint(feature_list_rgb);

        for (auto &&feature_rgb : feature_list_rgb)
        {
            // Set RGB exposure time to half the maximum value
            if (feature_rgb.name_.compare("Exposure") == 0)
            {
                switch (feature_rgb.type_)
                {
                case GenTL::INFO_DATATYPE_INT32: {
                    pclstream.set("RGB::Exposure", feature_rgb.max_.i32_val_ / 2); // C1
                    break;
                }
                case GenTL::INFO_DATATYPE_FLOAT32: {
                    pclstream.set("RGB::Exposure", feature_rgb.max_.flt_val_ / 2.0f); // P1
                    break;
                }
                }
            }
        }
    }

    // Must with prefix "ToF::", turn off automatic exposure of ToF data stream
    bool isTofAutoExposure = false;
    pclstream.get("ToF::AutoExposure", isTofAutoExposure);
    if (isTofAutoExposure)
    {
        pclstream.set("ToF::AutoExposure", false);
    }
    // Set the maximum distance measured by ToF to 2.5m
    pclstream.set("ToF::Distance", 2.5f);
    // Set the ToF stream frame rate to 30
    pclstream.set("ToF::StreamFps", 30.0f);
    // Set ToF threshold to 100, Intensity below this value will not be displayed on the image
    pclstream.set("ToF::Threshold", 100);
    // Set ToF fly point removal intensity to 1
    pclstream.set("ToF::DepthFlyingPixelRemoval", 1);
    // Enable ToF distortion removal
    // pclstream.set("ToF::DistortRemove", true);

    // Can with prefix "PCL::" or not, Set PCL normal filtering intensity
    pclstream.set("PCL::PCLFlyingPixelRemoval", 0.0f);
    // Enable PCL to carry original images
    // According to DepthAlign2Color, if it is true, it carries RGB; otherwise, it carries ToF
    pclstream.set("PCL::EnableRgbAttach", true);
    // Enable PCL to remove occlusion
    pclstream.set("PCL::EnableOcclusion", false);
    // Align depth to texture when enable
    // Otherwise, align the texture to the depth
    // pclstream.set("PCL::DepthAlign2Color", false);

    // Obtain the list of ToF features supported for control in the PCL data stream
    auto feature_list_tof = pclstream.GetFeatureList("ToF");
    std::cout << "ToF feature: " << std::endl;
    FeatureListPrint(feature_list_tof);

    for (auto &&feature_tof : feature_list_tof)
    {
        // Set ToF exposure time to half the maximum value
        if (feature_tof.name_.compare("Exposure") == 0)
        {
            switch (feature_tof.type_)
            {
            case GenTL::INFO_DATATYPE_INT32: {
                pclstream.set("ToF::Exposure", feature_tof.max_.i32_val_ / 2); // C1
                break;
            }
            case GenTL::INFO_DATATYPE_FLOAT32: {
                pclstream.set("ToF::Exposure", feature_tof.max_.flt_val_ / 2.0f); // P1
                break;
            }
            }
        }
    }
    // Obtain the list of PCL features supported for control in the PCL data stream
    auto feature_list = pclstream.GetFeatureList();
    std::cout << "PCL feature: " << std::endl;
    FeatureListPrint(feature_list);

    bool saveReq = false;
    int count = 0;
    float DistRange = 0.0f;
    while (1)
    {
        // Obtain PCL frame data
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
            size_t varSize = sizeof(size_t);
            if (DistRange < 1e-5) // DistRange should be inited
            {
                // Get the value of Range in metadata data
                GenTL::PDBufferGetMetaByName(pPclFrame->getPort(), "Range", nullptr, &DistRange, &varSize);
                printf("max distance %f for distanceMap\n", DistRange);
            }

            // uint64_t usseTimer;
            // GenTL::PDBufferGetMetaByName(pPclFrame->getPort(), "##LT##TimeStampUSSE", nullptr, &DistRange, &varSize);
            // printf("TimeStampUSSE %ld timer from epoch \n", usseTimer);

            // Mat with id 0 is PCL data
            const cv::Mat &xyz = pPclFrame->getMat(0);
            // Mat with id 1 is infrared image data
            const cv::Mat &infrared = pPclFrame->getMat(1);

            std::vector<cv::Mat> channels(3);
            cv::split(xyz, channels);
            // Decomposing PCL to obtain depth information for each point
            const cv::Mat &depth = channels[2];
            cv::Mat u16Depth;
            // Obtain the phase of each point through depth information
            depth.convertTo(u16Depth, CV_16UC1, 65535.0 / DistRange);

            cv::imshow("xyz", xyz);
            cv::imshow("infrared", infrared);
            cv::imshow("depth", depth);

            if (has_rgb)
            {
                // If there is RGB, Mat with id 2 is align image
                // According to DepthAlign2Color, When true, the image is depth aligned to the texture;
                // when false, the image is texture aligned to the depth
                const cv::Mat &alignedColor = pPclFrame->getMat(2);
                // Mat with id 3 only appears when the EnableRgbAttach is true
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

    return 0;
}

int main()
{
    // return rgbDemo();
    // return tofDemo();
    return pclDemo();
    // return ImuDemo();
}
