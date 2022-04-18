/*
 * @Author: Kian Liu
 * @Date: 2021-12-07 23:15:51
 * @LastEditTime: 2022-04-16 20:42:32
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/src/sPDstream.cpp
 */
#include "inc/sPDstream.h"
#include "basic/inc/buffer2cvMat.h"
#include "debug.h"
#include "stringFormat.h"
#include <algorithm>
#include <cctype>
#include <string>
static cv::Mat emptyMat;

bool sPDstream::init()
{
    if (isInited())
    {
        return true;
    }

    uint32_t DSnum = 0;
    if (GenTL::DevGetNumDataStreams(hDev, &DSnum) != GenTL::GC_ERR_SUCCESS)
    {
        PD_ERROR("failed DevGetNumDataStreams!\n");
        return false;
    };
    if (DSnum < 1)
    {
        PD_ERROR("DevGetNumDataStreams return no stream!\n");
        return false;
    };

    PDHandle stream = nullptr;
    if (GenTL::DevOpenDataStream(hDev, streamIDstr.c_str(), &stream) != GenTL::GC_ERR_SUCCESS)
    {
        PD_ERROR("failed DevOpenDataStream!\n");
        return false;
    };
    setPort(stream);
    destructFuncs.emplace_back([this] { GenTL::DSClose(getPort()); });

    size_t streamSize;
    size_t buffer_size = sizeof(streamSize);
    if (GenTL::DSGetInfo(stream, GenTL::STREAM_INFO_PAYLOAD_SIZE, nullptr, &streamSize, &buffer_size) !=
        GenTL::GC_ERR_SUCCESS)
    {
        PD_ERROR("failed DevOpenDataStream!\n");
        return false;
    };

    for (int i = 0; i < BufferNum; i++)
    {
        void *buffer;
        if (GenTL::DSAllocAndAnnounceBuffer(stream, streamSize, nullptr, &buffer) != GenTL::GC_ERR_SUCCESS)
        {
            PD_ERROR("failed DSAllocAndAnnounceBuffer!\n");
            return false;
        }
        destructFuncs.emplace_back([this, buffer] {
            void *bufferTmp;
            GenTL::DSRevokeBuffer(getPort(), buffer, &bufferTmp, nullptr);
        });
        if (GenTL::DSQueueBuffer(stream, buffer) != GenTL::GC_ERR_SUCCESS)
        {
            PD_ERROR("failed DSQueueBuffer!\n");
            return false;
        }
    };

    if (GenTL::GCRegisterEvent(stream, GenTL::EVENT_NEW_BUFFER, &hEvent) != GenTL::GC_ERR_SUCCESS)
    {
        PD_ERROR("failed GCRegisterEvent!\n");
        return false;
    };

    destructFuncs.emplace_back([this] { GenTL::GCUnregisterEvent(getPort(), GenTL::EVENT_NEW_BUFFER); });

    return PDport::init();
}

bool sPDstream::startStream()
{
    if (GenTL::DSStartAcquisition(getPort(), GenTL::ACQ_START_FLAGS_DEFAULT, GENTL_INFINITE) != GenTL::GC_ERR_SUCCESS)
    {
        PD_ERROR("failed DSStartAcquisition!\n");
        return false;
    };
    destructFuncs.emplace_back([this] { GenTL::DSStopAcquisition(getPort(), GenTL::ACQ_STOP_FLAGS_DEFAULT); });
    return true;
}

sPDstream::~sPDstream()
{
    for (auto &&iter = destructFuncs.rbegin(); iter != destructFuncs.rend(); ++iter)
    {
        (*iter)();
    }
}

PDHandle sPDstream::getMat(std::vector<cv::Mat> &Mats, uint64_t timeOut)
{
    if (!streamStarted)
    {
        startStream();
        streamStarted = true;
    }
    GenTL::EVENT_NEW_BUFFER_DATA data;
    size_t size = sizeof(GenTL::EVENT_NEW_BUFFER_DATA);
    memset(&data, 0, size);
    if (GenTL::EventGetData(hEvent, &data, &size, timeOut) == GenTL::GC_ERR_SUCCESS)
    {
        void *buffer;
        size_t ptr_size = sizeof(buffer);
        if (GenTL::EventGetDataInfo(hEvent, &data, size, GenTL::EVENT_DATA_ID, nullptr, &buffer, &ptr_size) ==
            GenTL::GC_ERR_SUCCESS)
        {
            if (buffer2cvMat(buffer, getPort(), Mats))
            {
                return buffer;
            }
        }
    }
    return nullptr;
}

bool bufferSet::releaseMat(PDHandle pHandle)
{
    if (pHandle != nullptr)
    {
        if (GenTL::DSQueueBuffer(hStream, pHandle) != GenTL::GC_ERR_SUCCESS)
        {
            PD_ERROR("failed DSQueueBuffer!\n");
            return false;
        }
    }
    return true;
}

bufferSet::bufferSet(sPDstream *stream)
{
    hStream = stream->getPort();
}

uint32_t bufferSet::getMatNum()
{
    return Mats.size();
}
const cv::Mat &bufferSet::getMat(uint32_t id)
{
    if (id >= getMatNum())
    {
        return emptyMat;
    }
    return Mats[id];
}

std::shared_ptr<bufferSet> sPDstream::waitFrames(uint64_t timeOut)
{
    auto ret = std::make_shared<bufferSet>(this);

    ret->hBuffer = getMat(ret->Mats, timeOut);

    if (!ret->hBuffer)
    {
        ret.reset();
    }
    return ret;
}

bufferSet::~bufferSet()
{
    if (this->hBuffer)
    {
        releaseMat(this->hBuffer);
    }
}

sPDstream::sPDstream(PDdevice &device, uint32_t _streamID) : hDev(device)
{
    streamIDstr = n2hexstr(_streamID);
}

sPDstream::sPDstream(PDdevice &device, const char *streamName) : hDev(device)
{
    streamIDstr = streamName;
}

bool sPDstream::getCamPara(intrinsics &intr, extrinsics &extr)
{
    size_t size = sizeof(intr);
    auto err = GenTL::PDDSCamIntPara(getPort(), &intr, &size);
    if (err != GenTL::GC_ERR_SUCCESS)
    {
        if (err == GenTL::GC_ERR_NO_DATA)
        {
            PD_WARNING("this stream don't have camera para!\n");
        }
        PD_ERROR("failed PDDSCamIntPara!\n");
        return false;
    }
    size = sizeof(extr);
    if (GenTL::PDDSCamExtPara(getPort(), &extr, &size) != GenTL::GC_ERR_SUCCESS)
    {
        PD_ERROR("failed PDDSCamIntPara!\n");
        return false;
    }
    return true;
}

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

std::shared_ptr<bufferSet> PCLstream::waitFrames(uint64_t timeOut)
{
    for (auto pStream : streamVec)
    {
        auto ret = pStream->waitFrames(1);
        ret.reset();
    }
    return sPDstream::waitFrames(timeOut);
}
