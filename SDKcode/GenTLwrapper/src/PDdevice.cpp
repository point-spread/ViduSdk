/*
 * @Author: Kian Liu
 * @Date: 2021-12-27 10:20:14
 * @LastEditTime: 2022-04-14 10:19:22
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/src/PDdevice.cpp
 */

#include "GenTLwrapper/PDdevice.h"
#include "GenICam/GenTL.h"
#include "debug.h"
#include "stringFormat.h"
#include <functional>
#include <memory>
#include <vector>

class PDinterface;

class PDdeviceImpl
{
    uint64_t deviceID;
    std::string deviceSN;

    std::string stringID;
    std::shared_ptr<PDinterface> pIfhUsed;
    PDHandle dev = nullptr;
    uint32_t streamNum = 0;

  public:
    PDdeviceImpl(const char *id_cstr, const char *sn_cstr, const std::shared_ptr<PDinterface> &pIf)
    {
        stringID = std::string(id_cstr);
        deviceSN = std::string(sn_cstr);
        pIfhUsed = pIf;

        deviceID = strtol(id_cstr, nullptr, 16);
    }

    const std::string &getDeviceSN()
    {
        return deviceSN;
    }

    uint64_t getDeviceID()
    {
        return deviceID;
    }

    bool init();

    PDHandle getPort()
    {
        return dev;
    }

    uint32_t getStreamNum()
    {
        if (!streamNum)
        {
            if (GenTL::DevGetNumDataStreams(getPort(), &streamNum) != GenTL::GC_ERR_SUCCESS)
            {
                PD_ERROR("failed DevGetNumDataStreams!\n");
            }
        }
        return streamNum;
    }

    void close()
    {
        GenTL::DevClose(getPort());
    }

    ~PDdeviceImpl()
    {
        pIfhUsed.reset();
    }
};

class PDinterface : public std::enable_shared_from_this<PDinterface>
{
    bool IFopened = false;
    void *ifh = nullptr;
    const void *tl;
    std::string id;

  public:
    bool init()
    {
        if (IFopened)
        {
            return true;
        }
        GenTL::GC_ERROR err = GenTL::TLOpenInterface(const_cast<GenTL::TL_HANDLE>(tl), id.c_str(), &ifh);

        if (err == GenTL::GC_ERR_RESOURCE_IN_USE)
        {
            PD_WARNING("TLOpenInterface operated on opened interface %s\n", id.c_str());
        }
        else if (err != GenTL::GC_ERR_SUCCESS)
        {
            PD_ERROR("errcode %d\n", err);
            return false;
        }
        IFopened = true;
        return true;
    }

    PDinterface(void *_tl, const char *_id) : tl(_tl), id(_id)
    {
    }

    ~PDinterface()
    {
        if (IFopened)
        {
            GenTL::IFClose(ifh);
            IFopened = false;
        }
    }

    void *getIFh()
    {
        return ifh;
    }

    bool updateDeviceList(std::vector<std::shared_ptr<PDdeviceImpl>> &devList)
    {
        if (!init())
        {
            return false;
        }

        if (GenTL::IFUpdateDeviceList(getIFh(), nullptr, 2000) != GenTL::GC_ERR_SUCCESS)
        {
            PD_ERROR("failed IFUpdateDeviceList!\n");
            return false;
        }

        uint32_t DEVnum = 0;
        if (GenTL::IFGetNumDevices(getIFh(), &DEVnum) != GenTL::GC_ERR_SUCCESS)
        {
            PD_ERROR("failed IFGetNumDevices!\n");
            return false;
        }

        if (DEVnum < 1)
        {
            PD_DBG("IFGetNumDevices return no device!\n");
            return false;
        }

        char id_tmp[256] = "";
        size_t id_size = sizeof(id_tmp);
        for (uint32_t devIndex = 0; devIndex < DEVnum; devIndex++)
        {
            if (GenTL::IFGetDeviceID(getIFh(), devIndex, id_tmp, &id_size) != GenTL::GC_ERR_SUCCESS)
            {
                PD_ERROR("failed IFGetDeviceID, devIndex : %d\n", devIndex);
                continue;
            }

            char sn_tmp[256] = "";
            size_t sn_size = sizeof(sn_tmp);
            if (GenTL::IFGetDeviceInfo(getIFh(), id_tmp, GenTL::DEVICE_INFO_SERIAL_NUMBER, nullptr, sn_tmp, &sn_size) !=
                GenTL::GC_ERR_SUCCESS)
            {
                PD_WARNING("failed IFGetDeviceInfo DEVICE_INFO_SERIAL_NUMBER, devID : %s\n", id_tmp);
                continue;
            }
            devList.emplace_back(std::make_shared<PDdeviceImpl>(id_tmp, sn_tmp, shared_from_this()));
        }
        return true;
    }
};

bool PDdeviceImpl::init()
{
    PDHandle devTmp;
    auto err = GenTL::IFOpenDevice(pIfhUsed->getIFh(), stringID.c_str(), GenTL::DEVICE_ACCESS_CONTROL, &devTmp);
    if (err != GenTL::GC_ERR_SUCCESS)
    {
        if (err == GenTL::GC_ERR_RESOURCE_IN_USE)
        {
            PD_ERROR("Device %s used in otherwhere!\n", deviceSN.c_str());
        }
        PD_ERROR("failed IFOpenDevice %s!\n", deviceSN.c_str());
        return false;
    }
    dev = devTmp;
    return true;
}

class PDsystem
{
    static PDsystem *pSys;
    void *tl;
    std::vector<std::shared_ptr<PDinterface>> ifList;
    std::vector<std::shared_ptr<PDdeviceImpl>> devList;

    PDsystem()
    {
        GenTL::GCInitLib();
        GenTL::GC_ERROR err = GenTL::TLOpen(&tl);

        if (err == GenTL::GC_ERR_RESOURCE_IN_USE)
        {
            PD_ERROR("TLOpen operated on opened TL!\n");
        }
        else if (err != GenTL::GC_ERR_SUCCESS)
        {
            PD_ERROR("failed TLOpen!\n");
        }
    }

    void updateInterfaceList()
    {
        uint32_t IFnum = 0;
        char id_tmp[256] = "";
        size_t id_size = sizeof(id_tmp);

        void *tl = PDsystem::getSystem()->getTl();
        if (GenTL::TLGetNumInterfaces(tl, &IFnum) != GenTL::GC_ERR_SUCCESS)
        {
            PD_ERROR("failed TLGetNumInterfaces!\n");
        }

        if (IFnum < 1)
        {
            PD_WARNING("TLGetNumInterfaces return no interface!\n");
        }

        id_size = sizeof(id_tmp);

        for (uint32_t IFIndex = 0; IFIndex < IFnum; IFIndex++)
        {
            if (GenTL::TLGetInterfaceID(tl, IFIndex, id_tmp, &id_size) != GenTL::GC_ERR_SUCCESS)
            {
                PD_ERROR("failed TLGetInterfaceID %s\n", id_tmp);
                continue;
            }

            ifList.emplace_back(std::make_shared<PDinterface>(tl, id_tmp));
        }
    }

    void updateDeviceList()
    {
        devList.clear();
        for (const auto &pifh : getInterfaceList())
        {
            pifh->updateDeviceList(devList);
        }
    }

  public:
    static PDsystem *getSystem()
    {
        if (!pSys)
        {
            static PDsystem sys;
            pSys = &sys;
            pSys->updateInterfaceList();
        }
        return pSys;
    }

    void Close()
    {
        for (auto &&dev : devList)
        {
            dev.reset();
        }
        devList.clear();
        for (auto &&itf : ifList)
        {
            itf.reset();
        }
        ifList.clear();
        GenTL::TLClose(tl);
        tl = nullptr;
    }

    ~PDsystem()
    {
        GenTL::GCCloseLib();
    }

    void *getTl()
    {
        return tl;
    }

    static std::vector<std::shared_ptr<PDinterface>> &getInterfaceList()
    {
        return getSystem()->ifList;
    }

    static std::vector<std::shared_ptr<PDdeviceImpl>> &getDeviceList()
    {
        getSystem()->updateDeviceList();
        return getSystem()->devList;
    }

    static std::vector<std::string> getDeviceSNList()
    {
        std::vector<std::string> SNlist;
        getSystem()->updateDeviceList();
        for (const auto &pDev : getSystem()->devList)
        {
            SNlist.emplace_back(pDev->getDeviceSN());
        }
        return SNlist;
    }
};

std::vector<std::string> getDeviceSerialsNumberList()
{
    return PDsystem::getDeviceSNList();
}

PDsystem *PDsystem::pSys = nullptr;

PDdevice::PDdevice(const char *deviceSN)
{
    _deviceSN = deviceSN;
}

PDdevice::PDdevice(const std::string &deviceSN)
{
    _deviceSN = deviceSN;
}

PDdevice::PDdevice(uint64_t deviceID)
{
    _deviceID = deviceID;
}

PDdevice::PDdevice()
{
}

bool PDdevice::init()
{
    if (isInited())
    {
        return true;
    }
    bool autoSelectDevice = _deviceID == 0 && _deviceSN.empty();
    for (const auto &pDev : PDsystem::getDeviceList())
    {
        if (autoSelectDevice || pDev->getDeviceID() == _deviceID || pDev->getDeviceSN() == _deviceSN)
        {
            if (pDev->init())
            {
                setPort(pDev->getPort());
                pDevImpl = pDev;
                return PDport::init();
            }
            else if (autoSelectDevice)
            {
                continue;
            }
            return false;
        }
    }
    return false;
}

PDdevice::~PDdevice()
{
    if (pDevImpl)
        pDevImpl->close();
    PDsystem::getSystem()->Close();
}

uint32_t PDdevice::getStreamNum()
{
    return pDevImpl->getStreamNum();
}

uint64_t PDdevice::getDeviceID()
{
    if (pDevImpl)
    {
        return pDevImpl->getDeviceID();
    }
    return _deviceID;
}

const std::string &PDdevice::getDeviceSN()
{
    if (pDevImpl)
    {
        _deviceSN = pDevImpl->getDeviceSN();
    }
    return _deviceSN;
}

const char *PDdevice::getDeviceSNchar()
{
    return getDeviceSN().c_str();
}
