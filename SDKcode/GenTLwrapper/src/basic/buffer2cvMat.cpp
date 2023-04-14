/*
 * @Author: Kian Liu
 * @Date: 2022-03-03 15:58:41
 * @LastEditTime: 2022-04-27 21:53:23
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/basic/src/buffer2cvMat.cpp
 */

#include "GenICam/GenTL.h"
#include "GenTLwrapper/basic/getCvMatFromBuffer.h"

bool getCvMatFromBuffer(void *buffer, void *stream, std::vector<cv::Mat> &Mats)
{
    uint32_t height = 0, width = 0;
    uint64_t pixelFormat = 0;
    size_t pixelFormat_size = sizeof(pixelFormat);
    size_t height_size = sizeof(height);
    size_t width_size = sizeof(width);
    void *buffer_addr = nullptr;
    size_t buffer_addr_size = sizeof(buffer_addr);
    uint32_t partsNum = 0;
    cv::Size shape;
    GenTL::DSGetNumBufferParts(stream, buffer, &partsNum);
    while (Mats.size() < partsNum)
    {
        Mats.emplace_back(cv::Mat());
    }

    if (partsNum <= 1)
    {
        GenTL::DSGetBufferInfo(stream, buffer, GenTL::BUFFER_INFO_HEIGHT, nullptr, &height, &height_size);
        GenTL::DSGetBufferInfo(stream, buffer, GenTL::BUFFER_INFO_WIDTH, nullptr, &width, &width_size);
        GenTL::DSGetBufferInfo(stream, buffer, GenTL::BUFFER_INFO_PIXELFORMAT, nullptr, &pixelFormat,
                               &pixelFormat_size);
        GenTL::DSGetBufferInfo(stream, buffer, GenTL::BUFFER_INFO_BASE, nullptr, &buffer_addr, &buffer_addr_size);

        cvMatConvert(Mats[0], width, height, buffer_addr, pixelFormat);
    }
    else
    {
        for (uint32_t i = 0; i < partsNum; i++)
        {
            GenTL::DSGetBufferPartInfo(stream, buffer, i, GenTL::BUFFER_PART_INFO_HEIGHT, nullptr, &height,
                                       &height_size);
            GenTL::DSGetBufferPartInfo(stream, buffer, i, GenTL::BUFFER_PART_INFO_WIDTH, nullptr, &width, &width_size);
            GenTL::DSGetBufferPartInfo(stream, buffer, i, GenTL::BUFFER_PART_INFO_BASE, nullptr, &buffer_addr,
                                       &buffer_addr_size);
            GenTL::DSGetBufferPartInfo(stream, buffer, i, GenTL::BUFFER_PART_INFO_DATA_FORMAT, nullptr, &pixelFormat,
                                       &pixelFormat_size);

            cvMatConvert(Mats[i], width, height, buffer_addr, pixelFormat);
        }
    }
    return !Mats.empty();
}
