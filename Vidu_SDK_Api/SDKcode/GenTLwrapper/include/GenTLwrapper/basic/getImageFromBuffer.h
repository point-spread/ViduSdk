/*
 * @Author: Kian Liu
 * @Date: 2022-03-03 15:56:51
 * @LastEditTime: 2022-04-27 22:04:37
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/basic/inc/getImageFromBuffer.h
 */
#pragma once
#include "GenTLwrapper/PDimage.h"

#include <cstdint>
#include <string>
#include <vector>

/**
 * @brief Get PDimage form GenTL buffer handle
 *
 * @param stream handle of the GenTL buffer
 * @param stream handle of the GenTL stream
 * @param images stl vector container of PDimage
 * @return true get images successfully
 * @return false no valid data
 */
bool getImageFromBuffer(void *buffer, void *stream, std::vector<PDimage> &images);

/**
 * @brief Convert the pixelFormat (PFNC definded, include customed defined) to a simple friendly name
 *
 * @param format the pixelFormat of raw data : only PFNC_Coord3D_ABC32f (3D), PFNC_Coord3D_C16 (Depth)(or
 * PFNC_Coord3D_DC16 (Distance)), PFNC_RGB8 (RGB), PFNC_YUV422_8 (RGB), PFNC_Confidence16 (infrared) supported
 * @return std::string
 */
std::string getFriendlyName(uint64_t format);
