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
    // get the list of serial numbers for the currently connected devices
    auto devSNlist = getDeviceSerialsNumberList();
    if (devSNlist.empty())
    {
        PD_INFO("no device\n");
    }

    for (const auto &devSN : devSNlist)
    {
        PDdevice devInst(devSN);
        intrinsics intrin;
        extrinsics extrin;
        if (devInst.init())
        {
            PD_INFO("device : %s\n", devInst.getDeviceSN().c_str());
            // get how many stream currently exist on the device
            size_t streamNum = devInst.getStreamNum();
            for (size_t i = 0; i < streamNum; i++)
            {
                PDstream streamInst(devInst, i);
                streamInst.init();
                PD_INFO("stream : %s\n", streamInst.getStreamName().c_str());
                // Obtain internal and external parameters with distortion in the current data stream
                if (streamInst.getCamPara(intrin, extrin))
                {
                    PD_INFO("Intrin : \n");
                    print_intrinsics(&intrin);
                    PD_INFO("Extrin : \n");
                    print_extrinsics(&extrin);
                }
                streamInst.set("DistortRemove", true);
                // Obtain internal parameters of the current data stream without distortion
                if (streamInst.getCamPara(intrin, extrin))
                {
                    PD_INFO("Undistort Intrin : \n");
                    print_intrinsics(&intrin);
                }
                streamInst.set("DepthAlign2Color", true);
                // Obtain external parameters of the current data stream after depth align to color
                if (streamInst.getCamPara(intrin, extrin))
                {
                    PD_INFO("Extrin : \n");
                    print_extrinsics(&extrin);
                }
            }
        }
    }

#ifdef _WIN32
    return system("pause");
#else
    return 0;
#endif
}

int main()
{
    demo();
    return 0;
}
