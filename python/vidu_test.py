# import os
# import sys
# print(sys.path)
# from ...build.lib import pydyv as dyv
import pyvidu as vidu
import cv2 as cv
import numpy as np

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
    with vidu.PDstream(device, i) as stream:
        suc = stream.init()
        streamName = stream.getStreamName()
        if streamName == "PCL":
            feature_list_rgb, ret_rgb = stream.GetFeatureList("RGB")
            if ret_rgb:
                for j, feature_rgb in enumerate(feature_list_rgb):
                    print("RGB feature {}".format(feature_rgb.name))
            feature_list_tof, ret_tof = stream.GetFeatureList("ToF")
            if ret_tof:
                for j, feature_tof in enumerate(feature_list_tof):
                    print("ToF feature {}".format(feature_tof.name))
        feature_list, ret = stream.GetFeatureList()
        if ret:
            for j, feature in enumerate(feature_list):
                print("feature {name}, type {type}".format(name = feature.name, type = feature.type))
                if feature.type == vidu.INFO_DATATYPE_STRING:
                    for k, entry in enumerate(feature.enum_entry):
                        print("\tentry {index} {name}".format(index = k, name = entry))
                elif feature.type == vidu.INFO_DATATYPE_INT16:
                    print("\trange min {min}, max {max}".format(min = feature.min.i16_val, max = feature.max.i16_val))
                elif feature.type == vidu.INFO_DATATYPE_INT32:
                    print("\trange min {min}, max {max}".format(min = feature.min.i32_val, max = feature.max.i32_val))
                elif feature.type == vidu.INFO_DATATYPE_FLOAT32:
                    print("\trange min {min}, max {max}".format(min = feature.min.flt_val, max = feature.max.flt_val))

        if streamName == "PCL":
            stream.set("ToF::Distance", 2.5)
            distance, ret = stream.get("ToF::Distance", 1.0)
            print(distance, ret)
        elif streamName == "ToF":
            stream.set("Distance", 2.5)
            distance, ret = stream.get("Distance", 1.0)
            print(distance, ret)
            # OKL C1 support Resolution
            # resolution, ret = stream.get("Resolution", "", 10)
            # print(resolution, ret)

        if not suc:
            print("stream {} init failed\n".format(i))
            break
        else:
            print("stream {} init success\n".format(i))

        while True:
            if streamName == "IMU":
                imudata, ret = stream.GetImuData()
                if ret:
                    blank = np.zeros((600, 800, 1), np.uint8)
                    blank.fill(255)
                    text = "accelerometer time: {}".format(imudata.AcceTime)
                    cv.putText(blank, text, (10, 30), cv.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 0), 2)
                    text = "x: {x:.6f}, y: {y:.6f}, z: {z:.6f}".format(x = imudata.AcceData[0], y = imudata.AcceData[1], z = imudata.AcceData[2])
                    cv.putText(blank, text, (10, 60), cv.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 0), 2)
                    text = "gyroscope time: {}".format(imudata.GyroTime)
                    cv.putText(blank, text, (10, 90), cv.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 0), 2)
                    text = "x: {x:.6f}, y: {y:.6f}, z: {z:.6f}".format(x = imudata.GyroData[0], y = imudata.GyroData[1], z = imudata.GyroData[2])
                    cv.putText(blank, text, (10, 120), cv.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 0), 2)
                    cv.imshow("blank", blank)
            else:
                mats = stream.getPyMat()
                for j, mat in enumerate(mats):
                    cv.imshow("%s: %i" % (streamName, j), mat)

            key = cv.waitKey(1)
            if key & 0xFF == ord('q'):
                break

        suc = stream.getCamPara(intrinsic, extrinsic)
        vidu.print_intrinsics(intrinsic)
        vidu.print_extrinsics(extrinsic)

        print("finish stream {}\n".format(i))
        cv.destroyAllWindows()

print("all set\n")
