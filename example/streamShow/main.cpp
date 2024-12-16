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
#include <iostream>
#include <opencv2/opencv.hpp>
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
            std::cout << ", unknown data type: " << feature.type_ << std::endl;
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
        int32_t key = 0;
        if (frame)
        {
            // Obtain image data from frame data
            const PDimage &rgb = frame->getImage(0);
            cv::Mat rgb_mat =
                cv::Mat(cv::Size(rgb.GetImageWidth(), rgb.GetImageHeight()), rgb.GetImageCVType(), rgb.GetImageData());
            cv::imshow("rgb", rgb_mat);
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
        int32_t key = 0;
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

            const PDimage &pha = frame->getImage(0);
            const PDimage &infrared = frame->getImage(1);
            cv::Mat pha_mat =
                cv::Mat(cv::Size(pha.GetImageWidth(), pha.GetImageHeight()), pha.GetImageCVType(), pha.GetImageData());
            cv::Mat infrared_mat = cv::Mat(cv::Size(infrared.GetImageWidth(), infrared.GetImageHeight()),
                                           infrared.GetImageCVType(), infrared.GetImageData());
            cv::imshow("pha", pha_mat);
            cv::imshow("inrared", infrared_mat);
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
        int32_t key = 0;
        if (frame)
        {
            GenTL::DS_BUFFER_IMU_DATA *data = (GenTL::DS_BUFFER_IMU_DATA *)(frame->GetBuffer());

            cv::Mat imu_image = cv::Mat(600, 800, CV_8UC1, cv::Scalar::all(255));

            char text[128] = {};
            sprintf(text, "accelerometer time: %llu", (unsigned long long)data->AcceTime);
            cv::putText(imu_image, text, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, CV_RGB(0, 0, 0), 2.0f);
            memset(text, 0x0, sizeof(text));
            float ax = data->AcceData[0], ay = data->AcceData[1], az = data->AcceData[2];
            sprintf(text, "x: %s%.6f, y: %s%.6f, z: %s%.6f", ax >= 0 ? "+" : "", ax, ay >= 0 ? "+" : "", ay,
                    az >= 0 ? "+" : "", az);
            cv::putText(imu_image, text, cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 1.0, CV_RGB(0, 0, 0), 2.0f);
            memset(text, 0x0, sizeof(text));
            sprintf(text, "gyroscope time: %llu", (unsigned long long)data->GyroTime);
            cv::putText(imu_image, text, cv::Point(10, 90), cv::FONT_HERSHEY_SIMPLEX, 1.0, CV_RGB(0, 0, 0), 2.0f);
            memset(text, 0x0, sizeof(text));
            float gx = data->GyroData[0], gy = data->GyroData[1], gz = data->GyroData[2];
            sprintf(text, "x: %s%.6f, y: %s%.6f, z: %s%.6f", gx >= 0 ? "+" : "", gx, gy >= 0 ? "+" : "", gy,
                    gz >= 0 ? "+" : "", gz);
            cv::putText(imu_image, text, cv::Point(10, 120), cv::FONT_HERSHEY_SIMPLEX, 1.0, CV_RGB(0, 0, 0), 2.0f);

            cv::imshow("ImuData", imu_image);

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
        int32_t key = 0;
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

            // Image with id 0 is PCL data
            const PDimage &xyz = pPclFrame->getImage(0);
            // Image with id 1 is infrared image data
            const PDimage &infrared = pPclFrame->getImage(1);

            cv::Mat xyz_mat =
                cv::Mat(cv::Size(xyz.GetImageWidth(), xyz.GetImageHeight()), xyz.GetImageCVType(), xyz.GetImageData());
            std::vector<cv::Mat> channels(3);
            cv::split(xyz_mat, channels);
            // Decomposing PCL to obtain depth information for each point
            const cv::Mat &channel2 = channels[2];
            cv::Mat depth;
            // Obtain the phase of each point through depth information
            channel2.convertTo(depth, CV_16UC1, 65535.0 / DistRange);

            cv::Mat infrared_mat = cv::Mat(cv::Size(infrared.GetImageWidth(), infrared.GetImageHeight()),
                                           infrared.GetImageCVType(), infrared.GetImageData());

            cv::imshow("xyz", xyz_mat);
            cv::imshow("depth", depth);
            cv::imshow("infrared", infrared_mat);

            if (has_rgb)
            {
                // If there is RGB, Image with id 2 is align image
                // According to DepthAlign2Color, When true, the image is depth aligned to the texture;
                // when false, the image is texture aligned to the depth
                const PDimage &alignedColor = pPclFrame->getImage(2);
                // Image with id 3 only appears when the EnableRgbAttach is true
                // should pclstream.set("PCL::EnableRgbAttach", true);
                const PDimage &originColor = pPclFrame->getImage(3);
                cv::Mat aligned_mat = cv::Mat(cv::Size(alignedColor.GetImageWidth(), alignedColor.GetImageHeight()),
                                              alignedColor.GetImageCVType(), alignedColor.GetImageData());
                cv::Mat origin_mat = cv::Mat(cv::Size(originColor.GetImageWidth(), originColor.GetImageHeight()),
                                             originColor.GetImageCVType(), originColor.GetImageData());
                cv::imshow("alignedColor", aligned_mat);
                cv::imshow("originColor", origin_mat);
            }

            if (saveReq)
            {
                saveReq = false;
                // opt: 1 save ply with color, 2 save ply with color patch
                GenTL::PDBufferSave(*pPclFrame, nullptr, 0);
                printf("saved  \n");
                count++;
            }
        }
        pPclFrame.reset(); // plz. release frame buffer imediately

        key = cv::waitKey(1);
        if (key == 'c')
        {
            saveReq = true;
        }
        if (key == 'q')
        {
            break;
        }
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
