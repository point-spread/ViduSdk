/*
 * @Author: Kian Liu
 * @Date: 2022-04-16 19:45:49
 * @LastEditTime: 2022-04-16 20:33:03
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/src/PDstream.cpp
 */
#include "../inc/PDstream.h"
#include "../inc/PDdevice.h"

PDstream::PDstream(PDdevice &device, const char *streamName)
{
    std::string streamIDstr = streamName;
    std::transform(streamIDstr.begin(), streamIDstr.end(), streamIDstr.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    if (streamIDstr == "PCL")
    {
        pStream = std::make_shared<PCLstream>(device, streamName);
    }
    else
    {
        pStream = std::make_shared<sPDstream>(device, streamName);
    }
}

PDstream::PDstream(PDdevice &device, uint32_t _streamID)
{
    if (device.getStreamNum() > 1 && _streamID == device.getStreamNum() - 1)
    {
        pStream = std::make_shared<PCLstream>(device, _streamID);
    }
    else
    {
        pStream = std::make_shared<sPDstream>(device, _streamID);
    }
}

bool PDstream::getCamPara(intrinsics &intr, extrinsics &extr)
{
    return pStream->getCamPara(intr, extr);
}

bool PDstream::init()
{
    return pStream->init();
}

std::shared_ptr<bufferSet> PDstream::waitFrames(uint64_t timeOut)
{
    return pStream->waitFrames(timeOut);
}
