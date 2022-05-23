/*
 * @Author: Kian Liu
 * @Date: 2021-12-27 10:22:50
 * @LastEditTime: 2022-04-28 00:10:23
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/inc/PDdevice.h
 */
#pragma once
#include "../inc/PDport.h"
#include "stdint.h"
#include <memory>

class PDinterface;

/**
 * @brief Class PDdevice inherited from PDport.
 *        Wrapper of the GenTL device handle
 *
 */

class PDdevice : public PDport
{
    bool inited = false;
    uint32_t streamNum = 0;
    uint64_t deviceID = 0;
    std::unique_ptr<PDinterface> pIfhUsed;
    bool autoSelectDevice = false;

  public:
    /**
     * @brief Construct a new PDdevice object
     *
     * @param _deviceID the ID of the device to be opened, user can check it by okulo_viewer
     */
    PDdevice(uint64_t _deviceID);
    /**
     * @brief Construct a new PDdevice object, this can random select a device connected to the computer
     *
     */
    PDdevice();
    /**
     * @brief Destroy the PDdevice object
     *
     */
    ~PDdevice();

    /**
     * @brief Init the PDdevice objects, must be called once bdfore using the object,
             user can also implict init it by check the object valid by bool operation,
             just like if(PDdeviceObject).
     *
     * @return true
     * @return false
     */
    bool init() override;
    /**
     * @brief Get the Stream Num object
     *
     * @return uint32_t the stream number of the stream the camera can porvide
     */
    uint32_t getStreamNum();
};
