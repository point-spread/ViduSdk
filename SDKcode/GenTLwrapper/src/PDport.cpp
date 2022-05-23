/*
 * @Author: Kian Liu
 * @Date: 2022-04-27 22:56:27
 * @LastEditTime: 2022-04-27 22:57:45
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
