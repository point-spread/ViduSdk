/*
 * @Author: Kian Liu
 * @Date: 2021-12-28 15:53:58
 * @LastEditTime: 2022-04-07 15:57:08
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/inc/cvMatConvert.h
 */
#pragma once
#include <opencv2/opencv.hpp>
bool cvMatConvert(cv::Mat &mat, uint32_t width, uint32_t height, void *data, uint64_t pixelFormat);
std::string getFriendlyName(uint64_t format);
