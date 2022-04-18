/*
 * @Author: Kian Liu
 * @Date: 2021-12-27 10:22:50
 * @LastEditTime: 2022-04-13 02:41:59
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/inc/PDdevice.h
 */
#pragma once
#include "../inc/PDport.h"
#include "stdint.h"
#include <memory>

class PDinterface;
class PDdevice : public PDport
{
    bool inited = false;
    uint32_t streamNum = 0;
    uint64_t deviceID = 0;
    std::unique_ptr<PDinterface> pIfhUsed;
    bool autoSelectDevice = false;

  public:
    PDdevice(uint64_t _deviceID);
    PDdevice();
    ~PDdevice();
    bool init() override;
    uint32_t getStreamNum();
};
