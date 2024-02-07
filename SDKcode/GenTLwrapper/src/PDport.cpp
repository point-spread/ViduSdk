/*
 * @Author: Kian Liu
 * @Date: 2022-04-27 22:56:27
 * @LastEditTime: 2022-06-01 16:31:24
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/src/PDport.cpp
 */
#include "GenTLwrapper/PDport.h"

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

PDHandle PDport::getPort() const
{
    return port;
}

PDport::operator PDHandle()
{
    return getPort();
}

bool PDport::set(const char *name, int64_t value)
{
    return set(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_INT64);
}

bool PDport::get(const char *name, int64_t &value) const
{
    return get(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_INT64);
}

bool PDport::set(const char *name, uint64_t value)
{
    return set(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_UINT64);
}

bool PDport::get(const char *name, uint64_t &value) const
{
    return get(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_UINT64);
}

bool PDport::set(const char *name, int32_t value)
{
    return set(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_INT32);
}

bool PDport::get(const char *name, int32_t &value) const
{
    return get(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_INT32);
}

bool PDport::set(const char *name, uint32_t value)
{
    return set(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_UINT32);
}

bool PDport::get(const char *name, uint32_t &value) const
{
    return get(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_UINT32);
}

bool PDport::set(const char *name, int16_t value)
{
    return set(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_INT16);
}

bool PDport::get(const char *name, int16_t &value) const
{
    return get(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_INT16);
}

bool PDport::set(const char *name, uint16_t value)
{
    return set(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_UINT16);
}

bool PDport::get(const char *name, uint16_t &value) const
{
    return get(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_UINT16);
}

bool PDport::set(const char *name, float value)
{
    return set(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_FLOAT32);
}

bool PDport::get(const char *name, float &value) const
{
    return get(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_FLOAT32);
}

bool PDport::set(const char *name, double value)
{
    return set(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_FLOAT64);
}

bool PDport::get(const char *name, double &value) const
{
    return get(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_FLOAT64);
}

bool PDport::set(const char *name, bool value)
{
    return set(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_BOOL8);
}

bool PDport::get(const char *name, bool &value) const
{
    return get(name, &value, sizeof(value), GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_BOOL8);
}

bool PDport::set(const char *name, const char *value, uint32_t size)
{
    return set(name, value, size, GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_STRING);
}

bool PDport::get(const char *name, char *value, uint32_t size) const
{
    return get(name, value, size, GenTL::INFO_DATATYPE_LIST::INFO_DATATYPE_STRING);
}

#define GET_ERR_STRING()                                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        GenTL::GC_ERROR retTmp;                                                                                        \
        char errString[200];                                                                                           \
        size_t size = 200;                                                                                             \
        GenTL::GCGetLastError(&retTmp, errString, &size);                                                              \
        PD_WARNING("err_code : %d , %s\n", (int)retTmp, errString);                                                    \
    } while (0)

bool PDport::set(const char *name, const void *value, size_t size, int32_t dataType)
{
    GenTL::INFO_DATATYPE dataTypeTmp = (GenTL::INFO_DATATYPE)dataType;
    GenTL::GC_ERROR ret = GenTL::GCWritePortByName(port, name, &dataTypeTmp, value, &size);
    if ((int32_t)dataTypeTmp != dataType)
    {
        PD_WARNING("data type not match\n");
    }
    if (ret != GenTL::GC_ERR_SUCCESS)
    {
        GET_ERR_STRING();
        return false;
    }
    return true;
}

bool PDport::get(const char *name, void *value, size_t size, int32_t dataType) const
{
    GenTL::INFO_DATATYPE dataTypeTmp = (GenTL::INFO_DATATYPE)dataType;
    bool ret = GenTL::GCReadPortByName(port, name, &dataTypeTmp, value, &size);
    if ((int32_t)dataTypeTmp != dataType)
    {
        PD_WARNING("data type not match\n");
    }
    if (ret != GenTL::GC_ERR_SUCCESS)
    {
        GET_ERR_STRING();
        return false;
    }
    return true;
}
