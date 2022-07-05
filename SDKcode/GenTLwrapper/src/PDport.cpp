/*
 * @Author: Kian Liu
 * @Date: 2022-04-27 22:56:27
 * @LastEditTime: 2022-06-01 16:31:24
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/src/PDport.cpp
 */
#include "../inc/PDport.h"

void PDport::setPort(PDHandle handle)
{
    port = handle;
}

bool PDport::isInited() const
{
    return inited;
}

bool PDport::init()
{
    inited = true;
    return true;
}

PDHandle PDport::getPort()
{
    return port;
}

PDport::operator PDHandle()
{
    return getPort();
}

PDport::operator bool()
{
    return init();
}

bool PDport::set(const char *name, int64_t value)
{
    return set(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_INT64);
}

bool PDport::get(const char *name, int64_t &value)
{
    return get(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_INT64);
}

bool PDport::set(const char *name, uint64_t value)
{
    return set(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_UINT64);
}

bool PDport::get(const char *name, uint64_t &value)
{
    return get(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_UINT64);
}

bool PDport::set(const char *name, int32_t value)
{
    return set(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_INT32);
}

bool PDport::get(const char *name, int32_t &value)
{
    return get(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_INT32);
}

bool PDport::set(const char *name, uint32_t value)
{
    return set(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_UINT32);
}

bool PDport::get(const char *name, uint32_t &value)
{
    return get(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_UINT32);
}

bool PDport::set(const char *name, int16_t value)
{
    return set(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_INT16);
}

bool PDport::get(const char *name, int16_t &value)
{
    return get(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_INT16);
}

bool PDport::set(const char *name, uint16_t value)
{
    return set(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_UINT16);
}

bool PDport::get(const char *name, uint16_t &value)
{
    return get(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_UINT16);
}

bool PDport::set(const char *name, float value)
{
    double dvalue = value;
    return set(name, &dvalue, sizeof(dvalue), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_FLOAT64);
}

bool PDport::get(const char *name, float &value)
{
    double dvalue = 0.0;
    bool ret = get(name, &dvalue, sizeof(dvalue), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_FLOAT64);
    value = dvalue;
    return ret;
}

bool PDport::set(const char *name, bool value)
{
    return set(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_BOOL8);
}

bool PDport::get(const char *name, bool &value)
{
    return get(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_BOOL8);
}

bool PDport::set(const char *name, const void *value, size_t size, int32_t dataType)
{
    GenTL::INFO_DATATYPE dataTypeTmp = (GenTL::INFO_DATATYPE)dataType;
    bool ret = GenTL::GCWritePortByName(port, name, &dataTypeTmp, value, &size);
    if ((int32_t)dataTypeTmp != dataType)
    {
        PD_WARNING("data type not match\n");
    }
    return ret;
}

bool PDport::get(const char *name, void *value, size_t size, int32_t dataType)
{
    GenTL::INFO_DATATYPE dataTypeTmp = (GenTL::INFO_DATATYPE)dataType;
    bool ret = GenTL::GCReadPortByName(port, name, &dataTypeTmp, value, &size);
    if ((int32_t)dataTypeTmp != dataType)
    {
        PD_WARNING("data type not match\n");
    }
    return ret;
}
