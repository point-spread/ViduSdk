/*
 * @Author: Kian Liu
 * @Date: 2022-04-12 22:24:05
 * @LastEditTime: 2022-06-06 13:53:18
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/inc/PDport.h
 */
#pragma once
#include "GenICam/GenTL.h"
#include "debug.h"
#include "stringFormat.h"

#include <vector>

typedef void *PDHandle;

struct FeatureNodeInfo
{
    std::string name_;
    GenTL::INFO_DATATYPE type_;
    union RangeMin {
        int16_t i16_val_;
        int32_t i32_val_;
        float flt_val_;
    } min_;
    union RangeMax {
        int16_t i16_val_;
        int32_t i32_val_;
        float flt_val_;
    } max_;
    std::vector<std::string> enum_entry_;
};

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
    virtual bool init();
    bool set(const char *name, const void *value, size_t size, int32_t dataType);
    bool get(const char *name, void *value, size_t size, int32_t dataType) const;

  public:
    virtual ~PDport() = default;

    virtual bool set(const char *name, int64_t value);
    virtual bool get(const char *name, int64_t &value) const;

    virtual bool set(const char *name, uint64_t value);
    virtual bool get(const char *name, uint64_t &value) const;

    virtual bool set(const char *name, int32_t value);
    virtual bool get(const char *name, int32_t &value) const;

    virtual bool set(const char *name, uint32_t value);
    virtual bool get(const char *name, uint32_t &value) const;

    virtual bool set(const char *name, int16_t value);
    virtual bool get(const char *name, int16_t &value) const;

    virtual bool set(const char *name, uint16_t value);
    virtual bool get(const char *name, uint16_t &value) const;

    virtual bool set(const char *name, float value);
    virtual bool get(const char *name, float &value) const;

    virtual bool set(const char *name, bool value);
    virtual bool get(const char *name, bool &value) const;

    virtual bool set(const char *name, double value);
    virtual bool get(const char *name, double &value) const;

    virtual bool set(const char *name, const char *value, uint32_t size);
    virtual bool get(const char *name, char *value, uint32_t size) const;

    /**
     * @brief coercion type conversion from PDport object to GenTL port
     *
     * @return PDHandle GenTL port handle, which then can be used in GenTL function with C interface
     */
    operator PDHandle();

    /**
     * @brief Get the GenTL Port handle,
     *
     * @return PDHandle GenTL port handle, which then can be used in GenTL function with C interface
     */

    PDHandle getPort() const;
};
