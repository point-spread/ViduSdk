# import os
# import sys
# print(sys.path)
# from ...build.lib import pydyv as dyv
import pyvidu as vidu
import cv2 as cv


intrinsic = vidu.intrinsics()
extrinsic = vidu.extrinsics()

device = vidu.PDdevice()

if not device.init():
    print("device init failed")
    exit(-1)
print("device init succeed  ", device.getSerialsNumber())
stream_num = device.getStreamNum()
print("stream_num = {}\n".format(stream_num))

for i in range(stream_num):
    stream = vidu.PDstream(device, i)
    suc = stream.init()
    streamName = stream.getStreamName()
    stream.set("Distance", 7.5)
    distance, ret = stream.get("Distance", 1.0)
    print(distance, ret)
    break
    if not suc:
        print("stream {} init failed\n".format(i))
    else:
        print("stream {} init success\n".format(i))
    while True:
        mats = stream.getPyMat()
        for i, mat in enumerate(mats):
            cv.imshow("%s: %i" % (streamName, i), mat)

        key = cv.waitKey(1)
        if key & 0xFF == ord('q'):
            break

    suc = stream.getCamPara(intrinsic, extrinsic)
    vidu.print_intrinsics(intrinsic)
    vidu.print_extrinsics(extrinsic)

    print("finish stream {}\n".format(i))
    cv.destroyAllWindows()


print("all set\n")
