/*
 * @Author: Kian Liu
 * @Date: 2022-04-16 19:45:49
 * @LastEditTime: 2022-06-06 11:05:12
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/src/PDstream.cpp
 */
#include "GenTLwrapper/PDstream.h"
#include "GenTLwrapper/PDdevice.h"

#include <thread>

class PCLstream : public sPDstream
{
    std::vector<std::shared_ptr<sPDstream>> streamVec;

  public:
    PCLstream(PDdevice &device, uint32_t _streamID);
    PCLstream(PDdevice &device, const char *streamName);
    virtual std::shared_ptr<PDbuffer> waitFrames(uint64_t timeOut = 1) override;
    bool init() override;
    ~PCLstream();

    size_t getStreamIndex(const char *name, int &offset) const
    {
        auto nameStr = std::string(name);
        if (nameStr.size() > 5 && nameStr.compare(0, 5, "ToF::") == 0)
        {
            offset = 5;
            return 0;
        }
        else if (nameStr.size() > 5 && nameStr.compare(0, 5, "RGB::") == 0)
        {
            offset = 5;
            return 1;
        }
        else if (nameStr.size() > 5 && nameStr.compare(0, 5, "PCL::") == 0)
        {
            offset = 5;
            return SIZE_MAX;
        }
        offset = 0;
        return SIZE_MAX;
    }

#define SET_GET_OVERRIDE_FUNC(type)                                                                                    \
    bool set(const char *name, type value) override                                                                    \
    {                                                                                                                  \
        int offset = 0;                                                                                                \
        size_t index = getStreamIndex(name, offset);                                                                   \
        if (index != SIZE_MAX)                                                                                         \
            return streamVec[index]->set(&name[offset], value);                                                        \
        return sPDstream::set(&name[offset], value);                                                                   \
    }                                                                                                                  \
    bool get(const char *name, type &value) const override                                                             \
    {                                                                                                                  \
        int offset = 0;                                                                                                \
        size_t index = getStreamIndex(name, offset);                                                                   \
        if (index != SIZE_MAX)                                                                                         \
            return streamVec[index]->get(&name[offset], value);                                                        \
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

    bool set(const char *name, const char *value, uint32_t size) override;

    bool get(const char *name, char *value, uint32_t size) const override;

    std::vector<FeatureNodeInfo> GetFeatureList(const char *stream_name) const override;
};

PCLstream::PCLstream(PDdevice &device, uint32_t _streamID) : sPDstream(device, _streamID)
{
    streamVec.emplace_back(std::make_shared<sPDstream>(device, "ToF"));
    if (device.getStreamNum() > 2)
    {
        streamVec.emplace_back(std::make_shared<sPDstream>(device, "RGB"));
    }
}

PCLstream::PCLstream(PDdevice &device, const char *streamName) : sPDstream(device, streamName)
{
    streamVec.emplace_back(std::make_shared<sPDstream>(device, "ToF"));
    if (device.getStreamNum() > 2)
    {
        streamVec.emplace_back(std::make_shared<sPDstream>(device, "RGB"));
    }
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
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
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

bool PCLstream::set(const char *name, const char *value, uint32_t size)
{
    int offset = 0;
    size_t index = getStreamIndex(name, offset);
    if (index != SIZE_MAX)
        return streamVec[index]->set(&name[offset], value, size);
    return sPDstream::set(&name[offset], value, size);
}

bool PCLstream::get(const char *name, char *value, uint32_t size) const
{
    int offset = 0;
    size_t index = getStreamIndex(name, offset);
    if (index != SIZE_MAX)
        return streamVec[index]->get(&name[offset], value, size);
    return sPDstream::get(&name[offset], value, size);
}

std::vector<FeatureNodeInfo> PCLstream::GetFeatureList(const char *stream_name) const
{
    if (!stream_name || !strcmp(stream_name, "PCL"))
    {
        return sPDstream::GetFeatureList(stream_name);
    }
    else if (!strcmp(stream_name, "ToF"))
    {
        return streamVec[0]->GetFeatureList(stream_name);
    }
    else if (!strcmp(stream_name, "RGB") && streamVec.size() > 1)
    {
        return streamVec[1]->GetFeatureList(stream_name);
    }

    PD_WARNING("Stream node has not %s stream\n", stream_name);
    return {};
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
    if (stream_name_.empty())
    {
        stream_name_ = pStream->getStreamName();
        prefix_name_ = stream_name_ + "::";
    }
    return stream_name_;
}

bool PDstream::getCamPara(intrinsics &intr, extrinsics &extr)
{
    return pStream->getCamPara(intr, extr);
}

bool PDstream::getCamPara(intrinsics &undistort_intr)
{
    return pStream->getCamPara(undistort_intr);
}

bool PDstream::init()
{
    if (pStream->init())
    {
        getStreamName();
        return true;
    }
    return false;
}

std::shared_ptr<PDbuffer> PDstream::waitFrames(uint64_t timeOut)
{
    return pStream->waitFrames(timeOut);
}

bool PDstream::set(const char *name, const char *value, uint32_t size)
{
    if (!strncmp(name, prefix_name_.c_str(), prefix_name_.size()))
    {
        name = name + prefix_name_.size();
    }
    return pStream->set(name, value, size);
}

bool PDstream::get(const char *name, char *value, uint32_t size) const
{
    if (!strncmp(name, prefix_name_.c_str(), prefix_name_.size()))
    {
        name = name + prefix_name_.size();
    }
    return pStream->get(name, value, size);
}

std::vector<FeatureNodeInfo> PDstream::GetFeatureList(const char *stream_name) const
{
    return pStream->GetFeatureList(stream_name);
}
