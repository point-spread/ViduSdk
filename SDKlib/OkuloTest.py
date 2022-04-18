# import os
# import sys
# print(sys.path)
# from ...build.lib import pydyv as dyv
import pydyv as dyv
import cv2 as cv


intrinsic = dyv.intrinsics()
extrinsic = dyv.extrinsics()

device = dyv.PDdevice()

if not device.init():
    print("device init failed")
    exit(-1)
print("device init succeed")
stream_num = device.getStreamNum()
print("stream_num = {}\n".format(stream_num))

for i in range(stream_num):
    stream = dyv.PDstream(device, i)
    suc= stream.init()
    if not suc:
        print("stream {} init failed\n".format(i))
    else:
        print("stream {} init success\n".format(i))
    while True:
        mats = stream.getPyMat()
        for i, mat in enumerate(mats):
            cv.imshow("%d" %i, mat)
        
        key = cv.waitKey(1)
        if key & 0xFF == ord('q'):
            break

    suc =  stream.getCamPara(intrinsic, extrinsic)
    dyv.print_intrinsics(intrinsic)
    dyv.print_extrinsics(extrinsic)

    print("finish stream {}\n".format(i))


print("all set\n")
