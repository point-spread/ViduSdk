/*
 * @Author: Kian Liu
 * @Date: 2022-03-07 19:21:51
 * @LastEditTime: 2022-06-06 11:07:29
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/utility/getCamPara/main.cpp
 */
#include "Vidu_SDK_Api.h"
#include <chrono>
#include <thread>

static int demo()
{
    auto devSNlist = getDeviceSerialsNumberList();
    if (!devSNlist.empty())
    {
        for (const auto &devSN : devSNlist)
        {
            PDdevice devInst(devSN);
            intrinsics intrin;
            extrinsics extrin;
            if (devInst)
            {
                PD_INFO("device : %s\n", devInst.getDeviceSN().c_str());
                size_t streamNum = devInst.getStreamNum();
                for (size_t i = 0; i < streamNum; i++)
                {
                    PDstream streamInst(devInst, i);
                    streamInst.init();
                    PD_INFO("stream : %s\n", streamInst.getStreamName().c_str());
                    if (streamInst.getCamPara(intrin, extrin))
                    {
                        print_intrinsics(&intrin);
                        print_extrinsics(&extrin);
                    }
                }
            }
        }
    }
    else
    {
        PD_INFO("no device\n");
    }

#ifdef WIN32
    return system("pause");
#endif
    return 0;
}

int main()
{
    demo();
}
