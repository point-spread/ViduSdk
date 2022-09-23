/*
 * @Author: Kian Liu
 * @Date: 2022-04-16 19:45:49
 * @LastEditTime: 2022-06-06 11:05:12
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
    bool getCamPara(intrinsics &intr, extrinsics &extr) override;

    sPDstream *getStream(const char *name, int &offset)
    {
        auto nameStr = std::string(name);
        if (nameStr.size() > 5 && nameStr.compare(0, 5, "ToF::") == 0)
        {
            offset = 5;
            return streamVec[0].get();
        }
        else if (nameStr.size() > 5 && nameStr.compare(0, 5, "RGB::") == 0)
        {
            offset = 5;
            return streamVec[1].get();
        }
        else if (nameStr.size() > 5 && nameStr.compare(0, 5, "PCL::") == 0)
        {
            offset = 5;
            return this;
        }
        offset = 0;
        return this;
    }

#define SET_GET_OVERRIDE_FUNC(type)                                                                                    \
    bool set(const char *name, type value) override                                                                    \
    {                                                                                                                  \
        int offset = 0;                                                                                                \
        sPDstream *pSubStream = getStream(name, offset);                                                               \
        if (pSubStream != this)                                                                                        \
            return pSubStream->set(&name[offset], value);                                                              \
        return sPDstream::set(&name[offset], value);                                                                   \
    }                                                                                                                  \
    bool get(const char *name, type &value) override                                                                   \
    {                                                                                                                  \
        int offset = 0;                                                                                                \
        sPDstream *pSubStream = getStream(name, offset);                                                               \
        if (pSubStream != this)                                                                                        \
            return pSubStream->get(&name[offset], value);                                                              \
        return sPDstream::get(&name[offset], value);                                                                   \
    }

    SET_GET_OVERRIDE_FUNC(int64_t)
    SET_GET_OVERRIDE_FUNC(uint64_t)
    SET_GET_OVERRIDE_FUNC(int32_t)
    SET_GET_OVERRIDE_FUNC(uint32_t)
    SET_GET_OVERRIDE_FUNC(int16_t)
    SET_GET_OVERRIDE_FUNC(uint16_t)
    SET_GET_OVERRIDE_FUNC(float)
    SET_GET_OVERRIDE_FUNC(bool)
};

PCLstream::PCLstream(PDdevice &device, uint32_t _streamID) : sPDstream(device, _streamID)
{
    streamVec.emplace_back(std::make_shared<sPDstream>(device, "ToF"));
    streamVec.emplace_back(std::make_shared<sPDstream>(device, "RGB"));
}

PCLstream::PCLstream(PDdevice &device, const char *streamName) : sPDstream(device, streamName)
{
    streamVec.emplace_back(std::make_shared<sPDstream>(device, "ToF"));
    streamVec.emplace_back(std::make_shared<sPDstream>(device, "RGB"));
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

bool PCLstream::getCamPara(intrinsics &intr, extrinsics &extr)
{
    return streamVec[0]->getCamPara(intr, extr);
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

std::string PDstream::getStreamName()
{
    return pStream->getStreamName();
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
