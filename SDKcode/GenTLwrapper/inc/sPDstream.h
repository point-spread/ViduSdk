/*
 * @Author: Kian Liu
 * @Date: 2021-12-07 23:15:57
 * @LastEditTime: 2022-04-16 20:31:07
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/inc/sPDstream.h
 */
#pragma once
#include "PDdevice.h"
#include "PDport.h"
#include "basic/inc/camParaDef.h"
#include "debug.h"
#include <functional>
#include <memory>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class sPDstream;

class bufferSet
{
    bool releaseMat(PDHandle pHandle);
    PDHandle hBuffer = nullptr;
    PDHandle hStream = nullptr; // hBuffer intern will maintain the life of stream
    bool setHanle();
    friend class sPDstream;
    std::vector<cv::Mat> Mats;

  public:
    uint32_t getMatNum();
    const cv::Mat &getMat(uint32_t id = 0);
    bufferSet(sPDstream *stream);
    ~bufferSet();
    operator PDHandle()
    {
        return hBuffer;
    }
};
class sPDstream : public PDport
{
  private:
    bool inited = false;
    PDHandle hEvent = nullptr;
    std::string streamIDstr;
    int BufferNum = 8;
    std::vector<std::function<void(void)>> destructFuncs;
    bool streamStarted = false;
    bool startStream();
    virtual PDHandle getMat(std::vector<cv::Mat> &Mats, uint64_t timeOut);
    const PDHandle hDev; // hStream intern will maintain the life of dev

  public:
    sPDstream(PDdevice &device, uint32_t _streamID = 0x0);
    sPDstream(PDdevice &device, const char *streamName);
    virtual std::shared_ptr<bufferSet> waitFrames(uint64_t timeOut = 1);

    bool getCamPara(intrinsics &intr, extrinsics &extr);

    virtual ~sPDstream();
    bool init() override;
};

class PCLstream : public sPDstream
{
    std::vector<std::shared_ptr<sPDstream>> streamVec;

  public:
    PCLstream(PDdevice &device, uint32_t _streamID);
    PCLstream(PDdevice &device, const char *streamName);
    virtual std::shared_ptr<bufferSet> waitFrames(uint64_t timeOut = 1) override;
    bool init() override;
    ~PCLstream();
};
