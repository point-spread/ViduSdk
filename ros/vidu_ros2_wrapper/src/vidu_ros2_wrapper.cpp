#include <functional>
#include <memory>
#include <string>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

#include "Vidu_SDK_Api.h"
#include <chrono>
#include <cv_bridge/cv_bridge.h>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <image_transport/image_transport.hpp>
#include <sensor_msgs/image_encodings.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <sensor_msgs/point_cloud2_iterator.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/transform_broadcaster.h>
#include <thread>
#include <vidu_ros2_interfaces/msg/extrinsics.hpp>
// #include <sstream>
using namespace sensor_msgs;
using namespace cv;

// https://answers.ros.org/question/219876/using-sensor_msgspointcloud2-natively/
// convert point cloud image to ros message
sensor_msgs::msg::PointCloud2 img_to_cloud(const cv::Mat &rgb, const cv::Mat &coords, std_msgs::msg::Header header)
{
    // rgb is a cv::Mat with 3 color channels of size 640x480
    // coords is a cv::Mat with xyz channels of size 640x480, units in meters from calibration

    // figure out number of points
    int total = rgb.size().width * rgb.size().height;

    // declare message and sizes
    sensor_msgs::msg::PointCloud2 cloud;
    cloud.header = header;
    // cloud.width = numpoints;
    // cloud.height = 1;
    cloud.is_bigendian = false;
    cloud.is_dense = false; // there may be invalid points

    // for fields setup
    sensor_msgs::PointCloud2Modifier modifier(cloud);
    modifier.setPointCloud2FieldsByString(2, "xyz", "rgb");
    modifier.resize(total); // pre allocate total points,but may not be all valid

    // iterators
    sensor_msgs::PointCloud2Iterator<float> out_x(cloud, "x");
    sensor_msgs::PointCloud2Iterator<float> out_y(cloud, "y");
    sensor_msgs::PointCloud2Iterator<float> out_z(cloud, "z");
    sensor_msgs::PointCloud2Iterator<uint8_t> out_r(cloud, "r");
    sensor_msgs::PointCloud2Iterator<uint8_t> out_g(cloud, "g");
    sensor_msgs::PointCloud2Iterator<uint8_t> out_b(cloud, "b");

    int numpoints = 0; // rgb.size().width * rgb.size().height;
    for (int y = 0; y < coords.rows; y++)
    {
        for (int x = 0; x < coords.cols; x++)
        {
            // get the image coordinate for this point and convert to mm
            cv::Vec3f pointcoord = coords.at<cv::Vec3f>(y, x);
            float X_World = pointcoord.val[0];
            float Y_World = pointcoord.val[1];
            float Z_World = pointcoord.val[2];
            if (Z_World == 0)
                continue;
            // get the colors
            cv::Vec3b color = rgb.at<cv::Vec3b>(y, x);
            uint8_t r = (color[2]);
            uint8_t g = (color[1]);
            uint8_t b = (color[0]);

            // store xyz in point cloud, transforming from image coordinates, (Z Forward to X Forward)
            *out_x = X_World;
            *out_y = Y_World;
            *out_z = Z_World;

            // store colors
            *out_r = r;
            *out_g = g;
            *out_b = b;

            // increment
            ++out_x;
            ++out_y;
            ++out_z;
            ++out_r;
            ++out_g;
            ++out_b;
            ++numpoints;
        }
    }
    cloud.width = numpoints;
    cloud.height = 1;
    modifier.resize(numpoints);
    // ROS_INFO("Valid num. of points %d", numpoints);

    return cloud;
}

void getStreamCalibData(const intrinsics *intrinsic, sensor_msgs::msg::CameraInfo *camera_info)
{
    camera_info->height = intrinsic->height;
    camera_info->width = intrinsic->width;
    const std::string model_names[distortion::DISTORTION_COUNT] = {
        "unknown", "brown_conrady", "modified_brown_conrady", "inverse_brown_conrady", "ftheta", "kannala_brandt4"};
    camera_info->distortion_model = model_names[intrinsic->model];
    camera_info->d.resize(5);
    for (int i = 0; i < 5; i++)
    {
        camera_info->d.at(i) = intrinsic->coeffs[i];
    }
    camera_info->k.at(0) = intrinsic->fx;
    camera_info->k.at(2) = intrinsic->cx;
    camera_info->k.at(4) = intrinsic->fy;
    camera_info->k.at(5) = intrinsic->cy;
    camera_info->k.at(8) = 1;

    // set R (rotation matrix) values to identity matrix
    camera_info->r.at(0) = 1.0;
    camera_info->r.at(1) = 0.0;
    camera_info->r.at(2) = 0.0;
    camera_info->r.at(3) = 0.0;
    camera_info->r.at(4) = 1.0;
    camera_info->r.at(5) = 0.0;
    camera_info->r.at(6) = 0.0;
    camera_info->r.at(7) = 0.0;
    camera_info->r.at(8) = 1.0;

    camera_info->p.at(0) = camera_info->k.at(0);
    camera_info->p.at(1) = 0;
    camera_info->p.at(2) = camera_info->k.at(2);
    camera_info->p.at(3) = 0;
    camera_info->p.at(4) = 0;
    camera_info->p.at(5) = camera_info->k.at(4);
    camera_info->p.at(6) = camera_info->k.at(5);
    camera_info->p.at(7) = 0;
    camera_info->p.at(8) = 0;
    camera_info->p.at(9) = 0;
    camera_info->p.at(10) = 1;
    camera_info->p.at(11) = 0;

    camera_info->binning_x = 0;
    camera_info->binning_y = 0;
    camera_info->roi.x_offset = 0;
    camera_info->roi.y_offset = 0;
    camera_info->roi.height = 0;
    camera_info->roi.width = 0;
    camera_info->roi.do_rectify = 0;
    return;
}

void getExtrinsic(const extrinsics *extrinsic, vidu_ros2_interfaces::msg::Extrinsics *extrinsic_info)
{
    for (int i = 0; i < 9; ++i)
    {
        extrinsic_info->rotation[i] = extrinsic->rotation[i];
    }
    for (int i = 0; i < 3; ++i)
    {
        extrinsic_info->translation[i] = extrinsic->translation[i];
    }
    return;
}

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("vidu_ros2_wrapper_node");
    auto origin_color_pub = node->create_publisher<sensor_msgs::msg::Image>("camera/color/origin", 2);
    auto aligned_color_pub = node->create_publisher<sensor_msgs::msg::Image>("camera/color/aligned", 2);
    auto depth_pub = node->create_publisher<sensor_msgs::msg::Image>("camera/depth", 2);
    auto infrared_pub = node->create_publisher<sensor_msgs::msg::Image>("camera/infrared", 2);
    auto pointcloud_pub = node->create_publisher<sensor_msgs::msg::PointCloud2>("camera/pointcloud", 2);

    auto color_camera_info_pub = node->create_publisher<sensor_msgs::msg::CameraInfo>("camera/info/color", 2);
    auto depth_camera_info_pub = node->create_publisher<sensor_msgs::msg::CameraInfo>("camera/info/depth", 2);
    auto color2depth_info_pub =
        node->create_publisher<vidu_ros2_interfaces::msg::Extrinsics>("camera/extrinsics/color_to_depth", 2);

    static std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster;

    tf_broadcaster = std::make_unique<tf2_ros::TransformBroadcaster>(*node);

    PDdevice devInst;
    sensor_msgs::msg::CameraInfo camera_color_info;
    sensor_msgs::msg::CameraInfo camera_depth_info;
    vidu_ros2_interfaces::msg::Extrinsics color2depth_info;

    if (devInst.init())
    {
        RCLCPP_INFO(node->get_logger(), "Camera device : %s\n", devInst.getDeviceSN().c_str());

        // obtain the camera info
        intrinsics rgb_intrinsic;
        extrinsics rgb_extrinsic;
        PDstream *rgbstream = new PDstream(devInst, "RGB");
        rgbstream->init();
        if (rgbstream->getCamPara(rgb_intrinsic, rgb_extrinsic))
        {
            // print_intrinsics(&rgb_intrinsic);
            // print_extrinsics(&rgb_extrinsic);
            getStreamCalibData(&rgb_intrinsic, &camera_color_info);
        }
        else
        {
            RCLCPP_WARN_ONCE(node->get_logger(), "Failed to get RGB intrinsic and extrinsic\n");
        }
        delete rgbstream;

        intrinsics tof_intrinsic;
        extrinsics tof_extrinsic;
        PDstream *tofstream = new PDstream(devInst, "ToF");
        tofstream->init();
        if (tofstream->getCamPara(tof_intrinsic, tof_extrinsic))
        {
            getStreamCalibData(&tof_intrinsic, &camera_depth_info);
        }
        else
        {
            RCLCPP_WARN_ONCE(node->get_logger(), "Failed to get ToF intrinsic and extrinsic\n");
        }
        delete tofstream;

        getExtrinsic(&rgb_extrinsic, &color2depth_info);

        auto pclstream = PDstream(devInst, "PCL");
        if (pclstream.init())
        {
            bool isTofAutoExposure = false;
            bool isRGBAutoExposure = false;
            pclstream.get("ToF::AutoExposure", isTofAutoExposure);
            pclstream.get("RGB::AutoExposure", isRGBAutoExposure);
            if (isTofAutoExposure)
            {
                pclstream.set("ToF::AutoExposure", true);
            }
            if (isRGBAutoExposure)
            {
                pclstream.set("RGB::AutoExposure", true);
            }
            pclstream.set("RGB::StreamFps", 30.0f);

            pclstream.set("ToF::Distance", 2.5f);
            pclstream.set("ToF::StreamFps", 30.0f);
            pclstream.set("ToF::Threshold", 100);
            // pclstream.set("ToF::RemoveStrength", 1);
            // pclstream.set("ToF::Exposure", 1.0f);
            // pclstream.set("RGB::Exposure", 10.0f);
            // pclstream.set("RGB::Gain", 20.0f);
            // pclstream.set("PCL::FilterStrength", 0.0f);
            // Enable original image attached to PCL for displaying purpose only
            pclstream.set("PCL::EnableRgbAttach", true);

            int count = 0;
            float DistRange = 0.0f;

            sensor_msgs::msg::Image msg_color;
            sensor_msgs::msg::Image msg_aligned_color;
            sensor_msgs::msg::PointCloud2 msg_pointcloud;

            // broadcasting the coordinate of the pointcloud
            RCLCPP_INFO(node->get_logger(), "Camera ready");

            while (rclcpp::ok())
            {
                auto pPclFrame = pclstream.waitFrames(); // pcl frames work only when tof && rgb stream grab frames!!!

                if (pPclFrame)
                {
                    if (DistRange < 1e-5) // DistRange should be inited
                    {
                        size_t varSize = sizeof(varSize);
                        GenTL::PDBufferGetMetaByName(pPclFrame->getPort(), "Range", nullptr, &DistRange, &varSize);
                        // printf("max distance %f for distanceMap\n", DistRange);
                    }
                    const cv::Mat &xyz = pPclFrame->getMat(0);
                    const cv::Mat &infrared = pPclFrame->getMat(1);
                    const cv::Mat &alignedColor = pPclFrame->getMat(2);
                    const cv::Mat &originColor =
                        pPclFrame->getMat(3); // should pclstream.set("PCL::EnableRgbAttach", true);

                    std::vector<cv::Mat> channels(3);
                    cv::split(xyz, channels);
                    const cv::Mat &depth = channels[2];
                    cv::Mat u16Depth;
                    depth.convertTo(u16Depth, CV_16UC1, 65535.0 / DistRange);

                    // cv::imshow("xyz", xyz);
                    // cv::imshow("infrared", infrared);
                    // cv::imshow("alignedColor", alignedColor);
                    // cv::imshow("depth", depth);
                    // cv::imshow("originColor", originColor);

                    auto frame_get_time = rclcpp::Clock().now();
                    std_msgs::msg::Header header_color;
                    // header_color.seq = count;
                    header_color.frame_id = "camera_color_frame";

                    header_color.stamp = frame_get_time;
                    cv_bridge::CvImage origin_image(header_color, sensor_msgs::image_encodings::TYPE_8UC3, originColor);

                    std_msgs::msg::Header header_depth;
                    // header_depth.seq = count;
                    header_depth.frame_id = "camera_depth_frame";
                    header_depth.stamp = frame_get_time;
                    cv_bridge::CvImage depth_image(header_depth, sensor_msgs::image_encodings::TYPE_16UC1, u16Depth);
                    cv_bridge::CvImage infrared_image(header_depth, sensor_msgs::image_encodings::TYPE_16UC1, infrared);

                    std_msgs::msg::Header header_aligned_color;
                    // header_aligned_color.seq = count;
                    header_aligned_color.frame_id = "camera_aligned_color_frame";
                    header_aligned_color.stamp = frame_get_time;
                    cv_bridge::CvImage aligned_image(header_aligned_color, sensor_msgs::image_encodings::TYPE_8UC3,
                                                     alignedColor);
                    // TODO: reorganize xyz, rgb to PointCloud2  msg
                    std_msgs::msg::Header header_pcl;
                    // header_pcl.seq = count;
                    header_pcl.frame_id = "camera_pcl_frame";
                    header_pcl.stamp = frame_get_time;
                    msg_pointcloud = img_to_cloud(alignedColor, xyz, header_pcl);

                    camera_color_info.header = header_color;
                    camera_depth_info.header = header_depth;
                    color2depth_info.header = header_color;
                    // TODO: there is  a  image data copy operation
                    color_camera_info_pub->publish(camera_color_info);
                    depth_camera_info_pub->publish(camera_depth_info);
                    color2depth_info_pub->publish(color2depth_info);
                    origin_color_pub->publish(*origin_image.toImageMsg().get());
                    aligned_color_pub->publish(*aligned_image.toImageMsg().get());
                    depth_pub->publish(*depth_image.toImageMsg().get());
                    infrared_pub->publish(*infrared_image.toImageMsg().get());
                    pointcloud_pub->publish(msg_pointcloud);

                    // the rviz tool needs a frame to show pointcloud

                    geometry_msgs::msg::TransformStamped transformStamped;
                    transformStamped.header.stamp = frame_get_time;
                    transformStamped.header.frame_id = "world";
                    transformStamped.child_frame_id = header_pcl.frame_id;
                    transformStamped.transform.translation.x = 0.0;
                    transformStamped.transform.translation.y = 0.0;
                    transformStamped.transform.translation.z = 0.0;
                    tf2::Quaternion q;
                    q.setRPY(-M_PI / 2.0, 0, M_PI / 2.0);
                    transformStamped.transform.rotation.x = q.x();
                    transformStamped.transform.rotation.y = q.y();
                    transformStamped.transform.rotation.z = q.z();
                    transformStamped.transform.rotation.w = q.w();

                    tf_broadcaster->sendTransform(transformStamped);

                    rclcpp::spin_some(node);

                    ++count;
                }
                pPclFrame.reset(); // plz. release frame buffer imediately
                                   // std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            return true;
        }
        else
        {
            printf("stream init failed\n");
        }
    }
    else
    {
        printf("no valid device\n");
    }

    return 0;
}