/*
 * @Author: Kian Liu
 * @Date: 2022-04-16 19:45:49
 * @LastEditTime: 2022-04-27 22:51:59
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/src/PDstream.cpp
 */
#include "../inc/PDstream.h"
#include "../inc/PDdevice.h"

class PCLstream : public sPDstream
{
    std::vector<std::shared_ptr<sPDstream>> streamVec;

  public:
    PCLstream(PDdevice &device, uint32_t _streamID);
    PCLstream(PDdevice &device, const char *streamName);
    virtual std::shared_ptr<PDbuffer> waitFrames(uint64_t timeOut = 1) override;
    bool init() override;
    ~PCLstream();
};

PCLstream::PCLstream(PDdevice &device, uint32_t _streamID) : sPDstream(device, _streamID)
{
    streamVec.emplace_back(std::make_shared<sPDstream>(device, "RGB"));
    streamVec.emplace_back(std::make_shared<sPDstream>(device, "ToF"));
}

PCLstream::PCLstream(PDdevice &device, const char *streamName) : sPDstream(device, streamName)
{
    streamVec.emplace_back(std::make_shared<sPDstream>(device, "RGB"));
    streamVec.emplace_back(std::make_shared<sPDstream>(device, "ToF"));
}

PCLstream::~PCLstream()
{
    streamVec.clear();
}

bool PCLstream::init()
{
    bool ret = true;
    for (auto pStream : streamVec)
    {
        ret &= pStream->init();
    }
    ret &= sPDstream::init();
    return ret;
}

std::shared_ptr<PDbuffer> PCLstream::waitFrames(uint64_t timeOut)
{
    for (auto pStream : streamVec)
    {
        auto ret = pStream->waitFrames(1);
        ret.reset();
    }
    return sPDstream::waitFrames(timeOut);
}

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

std::shared_ptr<PDbuffer> PDstream::waitFrames(uint64_t timeOut)
{
    return pStream->waitFrames(timeOut);
}
