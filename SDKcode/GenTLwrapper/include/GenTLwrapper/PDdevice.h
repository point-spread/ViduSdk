/*
 * @Author: Kian Liu
 * @Date: 2021-12-27 10:22:50
 * @LastEditTime: 2023-01-04 12:24
 * @LastEditors: Guan
 * @Description: 1.add func "void SetDiveceSN(const std::string &deviceSN);"
 * @FilePath: /DYV_SDK/GenTLwrapper/inc/PDdevice.h
 */

#pragma once
#include "GenTLwrapper/PDport.h"
#include "stdint.h"
#include <memory>
#include <vector>

class PDdeviceImpl;

/**
 * @brief Class PDdevice inherited from PDport.
 *        Wrapper of the GenTL device handle
 *
 */
class PDdevice : public PDport
{
    uint64_t _deviceID = 0;
    std::string _deviceSN;
    std::shared_ptr<PDdeviceImpl> pDevImpl;

  public:
    /**
     * @brief Construct a new PDdevice object
     *
     * @param _deviceID the ID of the device to be opened, user can check it by vidu_viewer
     */
    [[deprecated]] PDdevice(uint64_t _deviceID);
    /**
     * @brief Construct a new PDdevice object
     *
     * @param deviceSN the SN (SerialsNumber) of the device to be opened, user can check it by vidu_viewer
     */
    PDdevice(const char *deviceSN);
    /**
     * @brief Construct a new PDdevice object
     *
     * @param deviceSN the SN (SerialsNumber) of the device to be opened, user can check it by vidu_viewer
     */
    PDdevice(const std::string &deviceSN);
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
     * @brief Init the PDdevice objects, must be called once before using the object.
     *
     * @return true
     * @return false
     */
    bool init() override;

    /**
     * @brief Get the SeriesNumber of device
     *
     * @return std::string the SeriesNumber of device
     */
    const std::string &getDeviceSN();
    /**
     * @brief Get the SeriesNumber of device
     *
     * @return const char * the SeriesNumber of device
     */
    const char *getDeviceSNchar();

    /**
     * @brief Get the SeriesNumber of device
     *
     * @return std::string the SeriesNumber of device
     */
    uint64_t getDeviceID();

    /**
     * @brief Get the Stream Num object
     *
     * @return uint32_t the stream number of the stream the camera can porvide
     */
    uint32_t getStreamNum();
};

/**
 * @brief Reture SerialsNumber of devices can be accessed.
 *
 * @return string vector contains SerialsNumbers.
 */
std::vector<std::string> getDeviceSerialsNumberList();
