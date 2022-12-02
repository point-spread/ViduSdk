/*
 * @Author: Kian Liu
 * @Date: 2021-12-28 15:56:14
 * @LastEditTime: 2022-04-27 21:53:30
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/basic/src/cvMatConvert.cpp
 */

#include "../inc/getCvMatFromBuffer.h"
#include "GenICam/PFNC_Supplement.h"
#include "debug.h"

bool cvMatConvert(cv::Mat &mat, uint32_t width, uint32_t height, void *data, uint64_t pixelFormat)
{
    cv::Size shape(width, height);
    switch (pixelFormat)
    {
    case PFNC_Coord3D_ABC32f:
        mat = cv::Mat(shape, CV_32FC3, data);
        break;
    case PFNC_Confidence16:
    case PFNC_Coord3D_C16:
    case PFNC_Coord3D_DC16:
        mat = cv::Mat(shape, CV_16UC1, data);
        break;
    case PFNC_RGB8:
        mat = cv::Mat(shape, CV_8UC3, data);
        break;
    case PFNC_YUV422_8:
        cv::cvtColor(cv::Mat(shape, CV_8UC2, data), mat, cv::COLOR_YUV2BGR_YUYV, 3);
        break;
    case PFNC_BayerBG8:
        cv::demosaicing(cv::Mat(shape, CV_8UC1, data), mat, cv::COLOR_BayerBG2BGR, 3);
        break;
    default:
        return false;
        break;
    }
    return true;
}

std::string getFriendlyName(uint64_t format)
{
    std::string partName;
    switch (format)
    {
    case PFNC_Coord3D_ABC32f:
        partName = "3D";
        break;
    case PFNC_Coord3D_C16:
        partName = "Depth";
        break;
    case PFNC_Coord3D_DC16:
        partName = "Distance";
        break;
    case PFNC_RGB8:
    case PFNC_YUV422_8:
        partName = "RGB";
        break;
    case PFNC_Confidence16:
        partName = "infrared";
        break;
    default:
        partName = "UnSpecified";
        break;
    }
    return partName;
}
