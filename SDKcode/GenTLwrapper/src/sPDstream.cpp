/*
 * @Author: Kian Liu
 * @Date: 2021-12-07 23:15:51
 * @LastEditTime: 2022-06-06 11:02:44
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/src/sPDstream.cpp
 */
#include "GenTLwrapper/sPDstream.h"
#include "GenTLwrapper/basic/getCvMatFromBuffer.h"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <regex>
#include <string>
#include <thread>

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
    }
    if (DSnum < 1)
    {
        PD_ERROR("DevGetNumDataStreams return no stream!\n");
        return false;
    }

    PDHandle stream = nullptr;
    if (GenTL::DevOpenDataStream(hDev, streamIDstr.c_str(), &stream) != GenTL::GC_ERR_SUCCESS)
    {
        PD_ERROR("failed DevOpenDataStream!\n");
        return false;
    }
    setPort(stream);
    if (!getStreamName().compare("IMU"))
    {
        no_image_ = true;
    }
    destructFuncs.emplace_back([this] { GenTL::DSClose(getPort()); });

    size_t streamSize;
    size_t buffer_size = sizeof(streamSize);
    if (GenTL::DSGetInfo(stream, GenTL::STREAM_INFO_PAYLOAD_SIZE, nullptr, &streamSize, &buffer_size) !=
        GenTL::GC_ERR_SUCCESS)
    {
        PD_ERROR("failed DevOpenDataStream!\n");
        return false;
    }

    for (int i = 0; i < BufferNum; i++)
    {
        void *buffer;
        if (GenTL::DSAllocAndAnnounceBuffer(stream, streamSize, nullptr, &buffer) != GenTL::GC_ERR_SUCCESS)
        {
            PD_ERROR("failed DSAllocAndAnnounceBuffer!\n");
            return false;
        }
        if (GenTL::DSQueueBuffer(stream, buffer) != GenTL::GC_ERR_SUCCESS)
        {
            PD_ERROR("failed DSQueueBuffer!\n");
            return false;
        }
    }
    destructFuncs.emplace_back([this, stream] {
        for (int32_t i = 0; i < BufferNum; i++)
        {
            void *buffer;
            auto error_code = GenTL::DSGetBufferID(stream, 0, &buffer);
            if (error_code == GenTL::GC_ERR_SUCCESS)
            {
                GenTL::DSRevokeBuffer(stream, buffer, nullptr, nullptr);
            }
            else
            {
                PD_ERROR("failed DSRevokeBuffer!\n");
            }
        }
    });

    if (GenTL::GCRegisterEvent(stream, GenTL::EVENT_NEW_BUFFER, &hEvent) != GenTL::GC_ERR_SUCCESS)
    {
        PD_ERROR("failed GCRegisterEvent!\n");
        return false;
    }

    destructFuncs.emplace_back([this] { GenTL::GCUnregisterEvent(getPort(), GenTL::EVENT_NEW_BUFFER); });
    return PDport::init();
}

bool sPDstream::startStream()
{
    if (GenTL::DSStartAcquisition(getPort(), GenTL::ACQ_START_FLAGS_DEFAULT, GENTL_INFINITE) != GenTL::GC_ERR_SUCCESS)
    {
        PD_ERROR("failed DSStartAcquisition!\n");
        return false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
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

std::vector<FeatureNodeInfo> sPDstream::GetFeatureList(const char *stream_name) const
{
    if (stream_name && getStreamName().compare(stream_name))
    {
        PD_WARNING("Stream node has not %s stream\n", stream_name);
        return {};
    }

    void *port = getPort();
    if (port)
    {
        GenTL::INFO_DATATYPE type = GenTL::INFO_DATATYPE_UNKNOWN;
        char info[512] = {};
        size_t size = sizeof(info);
        if (GenTL::DSGetInfo(port, GenTL::STREAM_FEATURE_LIST, &type, info, &size) != GenTL::GC_ERR_SUCCESS)
        {
            return {};
        }
        const std::string str_info = info;
        const std::regex info_split_ch("\\;");
        std::vector<std::string> feature_list(
            std::sregex_token_iterator(str_info.begin(), str_info.end(), info_split_ch, -1),
            std::sregex_token_iterator());
        std::vector<FeatureNodeInfo> feature_node_info_list;
        for (auto &&feature : feature_list)
        {
            const std::regex feature_split_ch("\\|");
            std::vector<std::string> info_list(
                std::sregex_token_iterator(feature.begin(), feature.end(), feature_split_ch, -1),
                std::sregex_token_iterator());
            FeatureNodeInfo node_info;
            node_info.name_ = info_list[0];
            node_info.type_ = std::stoi(info_list[1]);
            switch (node_info.type_)
            {
            case GenTL::INFO_DATATYPE_BOOL8: {
                node_info.min_.i32_val_ = 0;
                node_info.max_.i32_val_ = 0;
                node_info.enum_entry_.clear();
                break;
            }
            case GenTL::INFO_DATATYPE_INT16: {
                node_info.min_.i16_val_ = std::stoi(info_list[2]);
                node_info.max_.i16_val_ = std::stoi(info_list[3]);
                node_info.enum_entry_.clear();
                break;
            }
            case GenTL::INFO_DATATYPE_INT32: {
                node_info.min_.i32_val_ = std::stoi(info_list[2]);
                node_info.max_.i32_val_ = std::stoi(info_list[3]);
                node_info.enum_entry_.clear();
                break;
            }
            case GenTL::INFO_DATATYPE_FLOAT32: {
                node_info.min_.flt_val_ = std::stoi(info_list[2]);
                node_info.max_.flt_val_ = std::stoi(info_list[3]);
                node_info.enum_entry_.clear();
                break;
            }
            case GenTL::INFO_DATATYPE_STRING: {
                node_info.min_.i32_val_ = 0;
                node_info.max_.i32_val_ = 0;
                const std::regex entry_split_ch("\\,");
                std::vector<std::string> entry_list(
                    std::sregex_token_iterator(info_list[4].begin(), info_list[4].end(), entry_split_ch, -1),
                    std::sregex_token_iterator());
                node_info.enum_entry_.insert(node_info.enum_entry_.end(), entry_list.begin(), entry_list.end());
                break;
            }
            default: {
                PD_WARNING("Unknow data type %d\n", node_info.type_);
                continue;
            }
            }
            feature_node_info_list.emplace_back(node_info);
        }
        return feature_node_info_list;
    }

    return {};
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
            if (no_image_)
            {
                return buffer;
            }

            if (getCvMatFromBuffer(buffer, getPort(), Mats))
            {
                return buffer;
            }
        }
    }
    return nullptr;
}

sPDstream::sPDstream(PDdevice &device, uint32_t _streamID) : hDev(device)
{
    streamIDstr = n2hexstr(_streamID);
}

sPDstream::sPDstream(PDdevice &device, const char *streamName) : hDev(device)
{
    streamIDstr = streamName;
}

std::string sPDstream::getStreamName() const
{
    std::string streamName;
    char tmp[1024];
    size_t size = sizeof(tmp);
    if (GenTL::DSGetInfo(getPort(), GenTL::STREAM_INFO_ID, nullptr, &tmp, &size) != GenTL::GC_ERR_SUCCESS)
    {
        PD_ERROR("failed getStreamName!\n");
    }
    else
    {
        streamName = std::string(tmp);
    }
    return streamName;
}

bool sPDstream::getCamPara(intrinsics &intr, extrinsics &extr)
{
    size_t size = sizeof(intr);
    auto err = GenTL::DSGetInfo(getPort(), GenTL::STREAM_INFO_CMD_LIST::STREAM_CAM_INT_PARA, nullptr, &intr, &size);
    if (err != GenTL::GC_ERR_SUCCESS)
    {
        if (err == GenTL::GC_ERR_NO_DATA)
        {
            PD_WARNING("this stream doesn't have camera para!\n");
        }
        PD_ERROR("failed CamIntPara!\n");
        return false;
    }
    size = sizeof(extr);
    if (GenTL::DSGetInfo(getPort(), GenTL::STREAM_INFO_CMD_LIST::STREAM_CAM_EXT_PARA, nullptr, &extr, &size) !=
        GenTL::GC_ERR_SUCCESS)
    {
        PD_ERROR("failed CamIntPara!\n");
        return false;
    }
    return true;
}

bool sPDstream::getCamPara(intrinsics &undistort_intr)
{
    size_t size = sizeof(undistort_intr);
    auto err = GenTL::DSGetInfo(getPort(), GenTL::STREAM_INFO_CMD_LIST::STREAM_CAM_UNDISTORT_INT_PARA, nullptr,
                                &undistort_intr, &size);
    if (err != GenTL::GC_ERR_SUCCESS)
    {
        if (err == GenTL::GC_ERR_NO_DATA)
        {
            PD_WARNING("this stream doesn't have camera para!\n");
        }
        PD_ERROR("failed CamIntPara!\n");
        return false;
    }
    return true;
}
