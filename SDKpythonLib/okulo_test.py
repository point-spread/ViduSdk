# import os
# import sys
# print(sys.path)
# from ...build.lib import pydyv as dyv
import pyokulo as okulo
import cv2 as cv


intrinsic = okulo.intrinsics()
extrinsic = okulo.extrinsics()

device = okulo.PDdevice()

if not device.init():
    print("device init failed")
    exit(-1)
print("device init succeed")
stream_num = device.getStreamNum()
print("stream_num = {}\n".format(stream_num))

for i in range(stream_num):
    stream = okulo.PDstream(device, i)
    suc= stream.init()
    streamName = stream.getStreamName()
    if not suc:
        print("stream {} init failed\n".format(i))
    else:
        print("stream {} init success\n".format(i))
    while True:
        mats = stream.getPyMat()
        for i, mat in enumerate(mats):
            cv.imshow("%s: %i" %(streamName, i), mat)
        
        key = cv.waitKey(1)
        if key & 0xFF == ord('q'):
            break

    suc =  stream.getCamPara(intrinsic, extrinsic)
    okulo.print_intrinsics(intrinsic)
    okulo.print_extrinsics(extrinsic)

    print("finish stream {}\n".format(i))
    cv.destroyAllWindows()


print("all set\n")
