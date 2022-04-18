/*
 * @Author: Kian Liu
 * @Date: 2022-04-12 22:24:05
 * @LastEditTime: 2022-04-13 17:18:34
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/inc/PDport.h
 */
#pragma once
#include "GenICam/GenTL.h"

typedef void *PDHandle;
class PDport
{
  private:
    PDHandle port = nullptr;
    bool inited = false;

  protected:
    void setPort(PDHandle handle)
    {
        port = handle;
    }

    bool isInited() const
    {
        return inited;
    }

  public:
    virtual bool init()
    {
        inited = true;
        return true;
    }
    bool set(const char *name, const void *value, size_t size);
    bool get(const char *name, void *value, size_t size);
    bool set(uint64_t addr, const void *value, size_t size);
    bool get(uint64_t addr, void *value, size_t size);

    virtual ~PDport() = default;

    PDHandle getPort()
    {
        return port;
    }

    operator PDHandle()
    {
        return getPort();
    }

    operator bool()
    {
        return init();
    }
};
