/*
 * @Author: Kian Liu
 * @Date: 2022-04-16 20:02:17
 * @LastEditTime: 2022-04-16 20:26:32
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/inc/PDstream.h
 */

#include "sPDstream.h"
class PDstream : public PDport // proxy class
{
    std::shared_ptr<sPDstream> pStream;

  public:
    PDstream(PDdevice &device, uint32_t _streamID = 0x0);
    PDstream(PDdevice &device, const char *streamName);
    bool getCamPara(intrinsics &intr, extrinsics &extr);
    std::shared_ptr<bufferSet> waitFrames(uint64_t timeOut = 1);

    bool init() override;
};
