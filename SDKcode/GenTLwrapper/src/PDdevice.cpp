/*
 * @Author: Kian Liu
 * @Date: 2021-12-27 10:20:14
 * @LastEditTime: 2022-04-14 10:19:22
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/src/PDdevice.cpp
 */

#include "inc/PDdevice.h"
#include "GenICam/GenTL.h"
#include "debug.h"
#include "stringFormat.h"
#include <memory>
class PDinterface
{
    bool IFopened = false;
    void *ifh = nullptr;

  public:
    bool init(void *tl, const char *id_tmp)
    {
        GenTL::GC_ERROR err = GenTL::TLOpenInterface(tl, id_tmp, &ifh);

        if (err == GenTL::GC_ERR_RESOURCE_IN_USE)
        {
            PD_ERROR("TLOpenInterface operated on opened interface %s\n", id_tmp);
        }
        else if (err != GenTL::GC_ERR_SUCCESS)
        {
            PD_ERROR("errcode %d\n", err);
            return false;
        }
        IFopened = true;
        return true;
    }

    ~PDinterface()
    {
        if (IFopened)
        {
            GenTL::IFClose(ifh);
        }
    }

    void *getIFh()
    {
        return ifh;
    }
};

PDdevice::PDdevice(uint64_t _deviceID) : deviceID(_deviceID)
{
}

PDdevice::PDdevice()
{
    autoSelectDevice = true;
}

bool PDdevice::init()
{
    if (isInited())
    {
        return true;
    }

    uint32_t IFnum = 0;
    char id_tmp[256] = "";
    size_t id_size = sizeof(id_tmp);
    void *tl;

    if (GenTL::GCInitLib() != GenTL::GC_ERR_SUCCESS)
    {
        PD_ERROR("warn: failed GCInitLib!\n");
    }

    GenTL::GC_ERROR err = GenTL::TLOpen(&tl);

    if (err == GenTL::GC_ERR_RESOURCE_IN_USE)
    {
        PD_ERROR("TLOpen operated on opened TL!\n");
    }
    else if (err != GenTL::GC_ERR_SUCCESS)
    {
        PD_ERROR("failed TLOpen!\n");
        return false;
    }

    if (GenTL::TLGetNumInterfaces(tl, &IFnum) != GenTL::GC_ERR_SUCCESS)
    {
        PD_ERROR("failed TLGetNumInterfaces!\n");
        return false;
    }

    if (IFnum < 1)
    {
        PD_ERROR("TLGetNumInterfaces return no interface!\n");
        return false;
    }

    id_size = sizeof(id_tmp);

    for (uint32_t IFIndex = 0; IFIndex < IFnum; IFIndex++)
    {
        if (GenTL::TLGetInterfaceID(tl, IFIndex, id_tmp, &id_size) != GenTL::GC_ERR_SUCCESS)
        {
            PD_ERROR("failed TLGetInterfaceID %s\n", id_tmp);
            return false;
        }

        // PD_INFO("IF id %s\n", id_tmp);
        auto pifh = std::make_unique<PDinterface>();
        if (!pifh->init(tl, id_tmp))
        {
            PD_ERROR("failed TLOpenInterface %s\n", id_tmp);
            return false;
        }

        if (GenTL::IFUpdateDeviceList(pifh->getIFh(), nullptr, 2000) != GenTL::GC_ERR_SUCCESS)
        {
            PD_ERROR("failed IFUpdateDeviceList!\n");
            return false;
        };

        uint32_t DEVnum = 0;
        if (GenTL::IFGetNumDevices(pifh->getIFh(), &DEVnum) != GenTL::GC_ERR_SUCCESS)
        {
            PD_ERROR("failed IFGetNumDevices!\n");
            return false;
        }

        if (DEVnum < 1)
        {
            PD_ERROR("IFGetNumDevices return no device!\n");
            return false;
        }

        for (uint32_t devIndex = 0; devIndex < DEVnum; devIndex++)
        {
            if (GenTL::IFGetDeviceID(pifh->getIFh(), devIndex, id_tmp, &id_size) != GenTL::GC_ERR_SUCCESS)
            {
                PD_ERROR("failed IFGetDeviceID %s\n", id_tmp);
                return false;
            };
            PD_INFO("deviceID %s \n", id_tmp);
            if (static_cast<uint64_t>(strtol(id_tmp, nullptr, 16)) == deviceID || autoSelectDevice)
            {
                PDHandle dev;
                if (GenTL::IFOpenDevice(pifh->getIFh(), id_tmp, GenTL::DEVICE_ACCESS_CONTROL, &dev) !=
                    GenTL::GC_ERR_SUCCESS)
                {
                    PD_ERROR("failed IFOpenDevice!\n");
                    return false;
                };
                setPort(dev);
                deviceID = strtol(id_tmp, nullptr, 16);
                pIfhUsed = std::move(pifh);
                return true;
            }
        }
    }
    return PDport::init();
}

PDdevice::~PDdevice()
{
    GenTL::DevClose(getPort());
}

uint32_t PDdevice::getStreamNum()
{
    if (!streamNum)
    {
        if (GenTL::DevGetNumDataStreams(getPort(), &streamNum) != GenTL::GC_ERR_SUCCESS)
        {
            PD_ERROR("failed DevGetNumDataStreams!\n");
        };
    }
    return streamNum;
}
