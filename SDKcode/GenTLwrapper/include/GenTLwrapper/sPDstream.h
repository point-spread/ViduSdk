/*
 * @Author: Kian Liu
 * @Date: 2021-12-07 23:15:57
 * @LastEditTime: 2022-06-06 10:57:23
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/inc/sPDstream.h
 */
#pragma once
#include "GenTLwrapper/basic/camParaDef.h"
#include "PDbuffer.h"
#include "PDdevice.h"
#include "PDport.h"
#include <functional>
#include <memory>

#include <string>
#include <vector>

/**
 * @brief wrapper of the GenTL stream handle. PCL stream depends on RGB & ToF stream, it cannot work independently.
 * If you wanna open the RGB, TOF, PCL simultaneously, please use this class other than PDstream. Rember to grab RGB
 * & ToF continuously for grabing PCL stream.
 */
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

  protected:
    bool stopStream();

  public:
    /**
     * @brief Construct a new sPDstream object
     *
     * @param device PDdevice object
     * @param _streamID ID of the stream. For Okulo camera, RGB is O, TOF is 1, default is 0 (RGB)
     */
    sPDstream(PDdevice &device, uint32_t _streamID = 0x0);
    /**
     * @brief Construct a new sPDstream object
     *
     * @param device PDdevice object
     * @param streamName name of the stream to be open, option now : RGB/TOF/PCL
     */
    sPDstream(PDdevice &device, const char *streamName);

    /**
     * @brief Get the Stream Name object
     *
     * @return std::string
     */
    std::string getStreamName();

    /**
     * @brief to grab a set of frames
     *
     * @param timeOut try times to get the frames
     * @return std::shared_ptr<bufferSet> a smart pointer point the buffer of the frames
               if the smart pointer is null, it means no valid frame get
     */
    virtual std::shared_ptr<PDbuffer> waitFrames(uint64_t timeOut = 1);

    /**
     * @brief Get the Cam Para object
     *
     * @param intr intrinsic paras of the camera (tof & rgb are different from each other)
     * @param extr extrinsic paras of the camera (sdk align the rgb image to tof camera,
                   so the extrinsic paras of the TOF camera may be a indentify matrix)
     * @return true : get the para recorded at the camera
     * @return false  : cannot get the valid para from the camera
     */
    virtual bool getCamPara(intrinsics &intr, extrinsics &extr);

    /**
     * @brief Init the sPDstream objects, must be called once before using the object.
     *
     * @return true
     * @return false
     */
    bool init() override;

    /**
     * @brief Destroy the sPDstream object
     *
     */
    virtual ~sPDstream();
};
