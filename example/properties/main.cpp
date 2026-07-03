#include <iostream>

#include "Vidu_SDK_Api.h"

void FeatureListPrint(std::vector<FeatureNodeInfo> &feature_list) {
    for (auto &&feature : feature_list) {
        std::cout << "\t" << feature.name_;
        switch (feature.type_) {
            case GenTL::INFO_DATATYPE_BOOL8: {
                std::cout << ", bool type, no min and max value" << std::endl;
                break;
            }
            case GenTL::INFO_DATATYPE_INT16: {
                std::cout << ", 16 bit int type, min: " << feature.min_.i16_val_ << ", max: " << feature.max_.i16_val_
                          << std::endl;
                break;
            }
            case GenTL::INFO_DATATYPE_INT32: {
                std::cout << ", 32 bit int type, min: " << feature.min_.i32_val_ << ", max: " << feature.max_.i32_val_
                          << std::endl;
                break;
            }
            case GenTL::INFO_DATATYPE_FLOAT32: {
                std::cout << ", float type, min: " << feature.min_.flt_val_ << ", max: " << feature.max_.flt_val_
                          << std::endl;
                break;
            }
            case GenTL::INFO_DATATYPE_STRING: {
                std::cout << ", enumeration type, entry size: " << feature.enum_entry_.size()
                          << ", entry: " << std::endl;
                for (auto &&entry : feature.enum_entry_) {
                    std::cout << "\t\t" << entry << std::endl;
                }
                break;
            }
            default: {
                std::cout << ", unknown data type: " << feature.type_ << std::endl;
                break;
            }
        }
    }
}

void SetRgbProperties() {
    PDdevice dev_inst;
    if (!dev_inst.init()) {
        std::cout << "no valid device" << std::endl;
        return;
    }

    auto stream = PDstream(dev_inst, "RGB");
    if (!stream.init()) {
        std::cout << "no valid device" << std::endl;
        return;
    }

    auto feature_list = stream.GetFeatureList();
    FeatureListPrint(feature_list);

    // set properties
    stream.set("DistortRemove", true);          // bool
    stream.set("Resolution", "640 x 480", 10);  // enum, value is enum entry, size is value str length (contain '\0')
    stream.set("StreamFps", "20", 3);           // enum, ditto
    stream.set("AutoExposure", false);          // bool
    stream.set("Exposure", 20000);              // int32
    stream.set("Gain", 10);                     // int32
}

void SetTofProperties() {
    PDdevice dev_inst;
    if (!dev_inst.init()) {
        std::cout << "no valid device" << std::endl;
        return;
    }

    auto stream = PDstream(dev_inst, "ToF");
    if (!stream.init()) {
        std::cout << "no valid device" << std::endl;
        return;
    }

    auto feature_list = stream.GetFeatureList();
    FeatureListPrint(feature_list);

    // set properties
    stream.set("DistortRemove", true);          // bool
    stream.set("BorderRemove", true);           // bool
    stream.set("Distance", "5.0", 4);           // enum, value is enum entry, size is value str length (contain '\0')
    stream.set("Resolution", "320 x 240", 10);  // enum, ditto
    stream.set("StreamFps", "20", 3);           // enum, ditto
    stream.set("HdrExposureMode", true);        // bool
    stream.set("AutoExposure", false);          // bool
    stream.set("Exposure", 100);                // int32
    stream.set("Gain", 11.1f);                  // float
    stream.set("ExpoNpha", 0.5f);               // float
    stream.set("Threshold", 150);               // int16
    stream.set("DepthMedianBlur", true);        // bool
    stream.set("DepthFlyingPixelRemoval", 3);   // int32
    stream.set("KalmanFilter", true);           // bool
    stream.set("KalmanFilterFactor", 500);      // int32
    stream.set("DepthCompletion", true);        // bool
    stream.set("DepthSmoothStrength", 2);       // int16
}

void SetPclProperties() {
    PDdevice dev_inst;
    if (!dev_inst.init()) {
        std::cout << "no valid device" << std::endl;
        return;
    }

    auto stream = PDstream(dev_inst, "PCL");
    if (!stream.init()) {
        std::cout << "no valid device" << std::endl;
        return;
    }

    auto feature_list = stream.GetFeatureList();
    FeatureListPrint(feature_list);

    // set properties
    stream.set("PCLFlyingPixelRemoval", 10.5f);  // float
    stream.set("NeighbourFilter", 3);            // int32
    stream.set("EnableOcclusion", true);         // bool
    stream.set("DepthAlign2Color", true);        // bool
    // set sub stream properties
    stream.set("RGB::DistortRemove", true);
    stream.set("RGB::Resolution", "640 x 480", 10);
    stream.set("RGB::StreamFps", "20", 3);
    stream.set("RGB::AutoExposure", false);
    stream.set("RGB::Exposure", 20000);
    stream.set("RGB::Gain", 10);
    stream.set("ToF::DistortRemove", true);
    stream.set("ToF::BorderRemove", true);
    stream.set("ToF::Distance", "5.0", 4);
    stream.set("ToF::Resolution", "320 x 240", 10);
    stream.set("ToF::StreamFps", "20", 3);
    stream.set("ToF::HdrExposureMode", true);
    stream.set("ToF::AutoExposure", false);
    stream.set("ToF::Exposure", 100);
    stream.set("ToF::Gain", 11.1f);
    stream.set("ToF::ExpoNpha", 0.5f);
    stream.set("ToF::Threshold", 150);
    stream.set("ToF::DepthMedianBlur", true);
    stream.set("ToF::DepthFlyingPixelRemoval", 3);
    stream.set("ToF::KalmanFilter", true);
    stream.set("ToF::KalmanFilterFactor", 500);
    stream.set("ToF::DepthCompletion", true);
    stream.set("ToF::DepthSmoothStrength", 2);
}

int main() {
    // SetRgbProperties();
    // SetTofProperties();
    SetPclProperties();
    return 0;
}