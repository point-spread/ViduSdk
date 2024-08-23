/*
 * @Author: Kian Liu
 * @Date: 2022-04-16 20:02:17
 * @LastEditTime: 2022-06-06 11:04:47
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/inc/PDstream.h
 */

#include "sPDstream.h"
/**
 * @brief Proxy class of sPDstream. PCL stream depends on RGB & ToF stream, and cannot work independently.
 * This class let you use the PCL same as RGB & ToF. When PCL start, RGB & ToF are inhibited at the same time,
 * since PDstream only supports RGB & ToF stream grabbing simultaneously, it's highly recommended to use PDstream other
 * than sPDstream. It's more convient to get PCL stream by PDstream, and the PDbuffer of PCL has already contained point
 * cloud (PDimage[0]), cofidence (PDimage[1]) and aligned RGB (PDimage[2]) data.
 */
class PDstream : public PDport
{
    std::string prefix_name_;
    std::string stream_name_;
    std::shared_ptr<sPDstream> pStream; // proxy class

  public:
    /**
     * @brief Construct a new PDstream object by stream ID
     *
     * @param device PDdevice object
     * @param _streamID ID of the stream. For Okulo camera, RGB is O, TOF is 1, default is 0 (RGB)
     */
    PDstream(PDdevice &device, uint32_t _streamID = 0x0);
    /**
     * @brief Construct a new PDstream object by name
     *
     * @param device PDdevice object
     * @param streamName name of the stream to be open, option now : RGB/TOF/PCL
     */
    PDstream(PDdevice &device, const char *streamName);
    /**
     * @brief Get the Stream Name object
     *
     * @return std::string
     */
    std::string getStreamName();
    /**
     * @brief Get the Cam Para object
     *
     * @param intr intrinsic paras of the camera (tof & rgb are different from each other)
     * @param extr extrinsic paras of the camera (sdk align the rgb image to tof camera,
                   so the extrinsic paras of the TOF camera may be a indentify matrix)
     * @return true : get the para recorded at the camera
     * @return false  : cannot get the valid para from the camera
     */
    bool getCamPara(intrinsics &intr, extrinsics &extr);
    /**
     * @brief to grab a set of frames
     *
     * @param timeOut try times to get the frames
     * @return std::shared_ptr<bufferSet> a smart pointer point the buffer of the frames
               if the smart pointer is null, it means no valid frame get
     */
    std::shared_ptr<PDbuffer> waitFrames(uint64_t timeOut = 1);
    /**
     * @brief Init the PDstream objects, must be called once before using the object.
     *
     * @return true
     * @return false
     */
    bool init() override;

    template <typename T> bool set(const char *name, T value)
    {
        if (!strncmp(name, prefix_name_.c_str(), prefix_name_.size()))
        {
            name = name + prefix_name_.size();
        }
        return pStream->set(name, value);
    }

    template <typename T> bool get(const char *name, T &value) const
    {
        if (!strncmp(name, prefix_name_.c_str(), prefix_name_.size()))
        {
            name = name + prefix_name_.size();
        }
        return pStream->get(name, value);
    }

    bool set(const char *name, const char *value, uint32_t size) override;

    bool get(const char *name, char *value, uint32_t size) const override;

    std::vector<FeatureNodeInfo> GetFeatureList(const char *stream_name = nullptr) const;
};
