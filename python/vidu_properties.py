import pyvidu as vidu
import numpy as np

def FeatureListPrint(feature_list):
    for i, feature in enumerate(feature_list):
        if feature.type == vidu.INFO_DATATYPE_BOOL8:
            print("\t{name}, bool type, no min and max value".format(name = feature.name))
        elif feature.type == vidu.INFO_DATATYPE_INT16:
            print("\t{name}, 16 bit int type, min: {min}, max: {max}".format(name = feature.name, min = feature.min.i16_val, max = feature.max.i16_val))
        elif feature.type == vidu.INFO_DATATYPE_INT32:
            print("\t{name}, 32 bit int type, min: {min}, max: {max}".format(name = feature.name, min = feature.min.i32_val, max = feature.max.i32_val))
        elif feature.type == vidu.INFO_DATATYPE_FLOAT32:
            print("\t{name}, float type, min: {min}, max: {max}".format(name = feature.name, min = feature.min.flt_val, max = feature.max.flt_val))
        elif feature.type == vidu.INFO_DATATYPE_STRING:
            print("\t{name}, enumeration type, entry size: {size}, entry: ".format(name = feature.name, size = len(feature.enum_entry)))
            for j, entry in enumerate(feature.enum_entry):
                print("\t\t{name}".format(name = entry))
        else:
            print("\t{name}, unknown data type: {type}".format(name = feature.name, type = feature.type))

def SetRgbProperties():
    device = vidu.PDdevice()
    if not device.init():
        print("device init failed")
        return

    with vidu.PDstream(device, "RGB") as stream:
        if not stream.init():
            print("stream init failed")
            return

        feature_list, ret = stream.GetFeatureList()
        if ret:
            FeatureListPrint(feature_list)

        # set properties
        stream.set("DistortRemove", True)          # bool
        stream.set("Resolution", "640 x 480", 10)  # enum, value is enum entry, size is value str length (contain '\0')
        stream.set("StreamFps", "20", 3)           # enum, ditto
        stream.set("AutoExposure", False)          # bool
        stream.set("Exposure", 20000)              # int32
        stream.set("Gain", 10)                     # int32

def SetTofProperties():
    device = vidu.PDdevice()
    if not device.init():
        print("device init failed")
        return

    with vidu.PDstream(device, "ToF") as stream:
        if not stream.init():
            print("stream init failed")
            return

        feature_list, ret = stream.GetFeatureList()
        if ret:
            FeatureListPrint(feature_list)

        # set properties
        stream.set("DistortRemove", True)          # bool
        stream.set("BorderRemove", True)           # bool
        stream.set("Distance", "5.0", 4)           # enum, value is enum entry, size is value str length (contain '\0')
        stream.set("Resolution", "320 x 240", 10)  # enum, ditto
        stream.set("StreamFps", "20", 3)           # enum, ditto
        stream.set("HdrExposureMode", True)        # bool
        stream.set("AutoExposure", False)          # bool
        stream.set("Exposure", 100)                # int32
        stream.set("Gain", 11.1)                   # float
        stream.set("ExpoNpha", 0.5)                # float
        stream.set("Threshold", 150)               # int16
        stream.set("DepthMedianBlur", True)        # bool
        stream.set("DepthFlyingPixelRemoval", 3)   # int32
        stream.set("KalmanFilter", True)           # bool
        stream.set("KalmanFilterFactor", 500)      # int32
        stream.set("DepthCompletion", True)        # bool
        stream.set("DepthSmoothStrength", 2)       # int16

def SetPclProperties():
    device = vidu.PDdevice()
    if not device.init():
        print("device init failed")
        return

    with vidu.PDstream(device, "PCL") as stream:
        if not stream.init():
            print("stream init failed")
            return

        feature_list, ret = stream.GetFeatureList()
        if ret:
            FeatureListPrint(feature_list)

        # set properties
        stream.set("PCLFlyingPixelRemoval", 10.5)  # float
        stream.set("NeighbourFilter", 3)           # int32
        stream.set("EnableOcclusion", True)        # bool
        stream.set("DepthAlign2Color", True)       # bool
        # set sub stream properties
        stream.set("RGB::DistortRemove", True)          # bool
        stream.set("RGB::Resolution", "640 x 480", 10)  # enum, value is enum entry, size is value str length (contain '\0')
        stream.set("RGB::StreamFps", "20", 3)           # enum, ditto
        stream.set("RGB::AutoExposure", False)          # bool
        stream.set("RGB::Exposure", 20000)              # int32
        stream.set("RGB::Gain", 10)                     # int32
        stream.set("ToF::DistortRemove", True)          # bool
        stream.set("ToF::BorderRemove", True)           # bool
        stream.set("ToF::Distance", "5.0", 4)           # enum, value is enum entry, size is value str length (contain '\0')
        stream.set("ToF::Resolution", "320 x 240", 10)  # enum, ditto
        stream.set("ToF::StreamFps", "20", 3)           # enum, ditto
        stream.set("ToF::HdrExposureMode", True)        # bool
        stream.set("ToF::AutoExposure", False)          # bool
        stream.set("ToF::Exposure", 100)                # int32
        stream.set("ToF::Gain", 11.1)                   # float
        stream.set("ToF::ExpoNpha", 0.5)                # float
        stream.set("ToF::Threshold", 150)               # int16
        stream.set("ToF::DepthMedianBlur", True)        # bool
        stream.set("ToF::DepthFlyingPixelRemoval", 3)   # int32
        stream.set("ToF::KalmanFilter", True)           # bool
        stream.set("ToF::KalmanFilterFactor", 500)      # int32
        stream.set("ToF::DepthCompletion", True)        # bool
        stream.set("ToF::DepthSmoothStrength", 2)       # int16

def main():
    # SetRgbProperties()
    # SetTofProperties()
    SetPclProperties()

if __name__ == '__main__':
    main()
