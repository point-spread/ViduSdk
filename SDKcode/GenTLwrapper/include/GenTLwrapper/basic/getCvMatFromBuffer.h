/*
 * @Author: Kian Liu
 * @Date: 2022-03-03 15:56:51
 * @LastEditTime: 2022-04-27 22:04:37
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/basic/inc/getCvMatFromBuffer.h
 */
#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

/**
 * @brief Get cv::Mats form GenTL buffer handle
 *
 * @param stream handle of the GenTL buffer
 * @param stream handle of the GenTL stream
 * @param Mats stl vector container of cv::Mat
 * @return true get cv::Mats successfully
 * @return false no valid data
 */
bool getCvMatFromBuffer(void *buffer, void *stream, std::vector<cv::Mat> &Mats);

/**
 * @brief Convert the raw data to cv::Mat RGB or gray image, which is used in getCvMatFromBuffer function
 *
 * @param mat cv::Mat as the result if convert success
 * @param width the image width
 * @param height the image height
 * @param data the raw data
 * @param pixelFormat the raw data format
 * @return true convert success
 * @return false convert failed
 */

bool cvMatConvert(cv::Mat &mat, uint32_t width, uint32_t height, void *data, uint64_t pixelFormat);

/**
 * @brief Convert the pixelFormat (PFNC definded, include customed defined) to a simple friendly name
 *
 * @param format the pixelFormat of raw data : only PFNC_Coord3D_ABC32f (3D), PFNC_Coord3D_C16 (Depth)(or
 * PFNC_Coord3D_DC16 (Distance)), PFNC_RGB8 (RGB), PFNC_YUV422_8 (RGB), PFNC_Confidence16 (infrared) supported
 * @return std::string
 */
std::string getFriendlyName(uint64_t format);
