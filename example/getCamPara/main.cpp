/*
 * @Author: Kian Liu
 * @Date: 2022-03-07 19:21:51
 * @LastEditTime: 2022-06-06 11:07:29
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/utility/getCamPara/main.cpp
 */
#include "GenICam/GenTL.h"
#include "basic/inc/camParaDef.h"
#include "debug.h"
#include "inc/PDdevice.h"
#include "inc/PDstream.h"
#include "stringFormat.h"

int main()
{
    PDdevice devInst;
    intrinsics intrin;
    extrinsics extrin;
    if (devInst.init())
    {
        size_t streamNum = devInst.getStreamNum();
        for (int i = 0; i < streamNum; i++)
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
        return system("pause");
    }
    return 0;
}
