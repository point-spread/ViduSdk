/*
 * @Author: Kian Liu
 * @Date: 2022-04-12 22:24:05
 * @LastEditTime: 2022-04-27 23:05:02
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/inc/PDport.h
 */
#pragma once
#include "GenICam/GenTL.h"
#include "debug.h"
#include "stringFormat.h"

typedef void *PDHandle;

/**
 * @brief wrapper of the GenTL port handle
 *
 */
class PDport
{
  private:
    PDHandle port = nullptr;
    bool inited = false;

  protected:
    void setPort(PDHandle handle);

    bool isInited() const;

  protected:
    virtual bool init();
    bool set(const char *name, const void *value, size_t size);
    bool get(const char *name, void *value, size_t size);
    bool set(uint64_t addr, const void *value, size_t size);
    bool get(uint64_t addr, void *value, size_t size);

  public:
    virtual ~PDport() = default;

    /**
     * @brief coercion type conversion from PDport object to GenTL port
     *
     * @return PDHandle GenTL port handle, which then can be used in GenTL function with C interface
     */
    operator PDHandle();
    /**
     * @brief return the object is inited
     *
     * @return true
     * @return false
     */
    operator bool();

    /**
     * @brief Get the GenTL Port handle,
     *
     * @return PDHandle GenTL port handle, which then can be used in GenTL function with C interface
     */

    PDHandle getPort();
};
