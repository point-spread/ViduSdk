#include "Vidu_SDK_Api.h"
#include <chrono>
#include <cv_bridge/cv_bridge.h>
#include <geometry_msgs/TransformStamped.h>
#include <image_transport/image_transport.h>
#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/point_cloud2_iterator.h>
#include <std_msgs/String.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/transform_broadcaster.h>
#include <thread>
#include <vidu_ros_wrapper/Extrinsics.h>
using namespace sensor_msgs;
using namespace cv;

// https://answers.ros.org/question/219876/using-sensor_msgspointcloud2-natively/
// convert point cloud image to ros message
sensor_msgs::PointCloud2 img_to_cloud(const cv::Mat &rgb, const cv::Mat &coords, std_msgs::Header header)
{
    // rgb is a cv::Mat with 3 color channels of size 640x480
    // coords is a cv::Mat with xyz channels of size 640x480, units in meters from calibration

    // figure out number of points
    int total = rgb.size().width * rgb.size().height;

    // declare message and sizes
    sensor_msgs::PointCloud2 cloud;
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

void sendTransform(std::string frame_id)
{
    static tf2_ros::TransformBroadcaster br;
    geometry_msgs::TransformStamped transformStamped;

    transformStamped.header.stamp = ros::Time::now();
    transformStamped.header.frame_id = "world";
    transformStamped.child_frame_id = frame_id;
    transformStamped.transform.translation.x = 0.0;
    transformStamped.transform.translation.y = 0.0;
    transformStamped.transform.translation.z = 0.0;
    tf2::Quaternion q;
    q.setRPY(-M_PI / 2.0, 0, M_PI / 2.0);
    transformStamped.transform.rotation.x = q.x();
    transformStamped.transform.rotation.y = q.y();
    transformStamped.transform.rotation.z = q.z();
    transformStamped.transform.rotation.w = q.w();

    br.sendTransform(transformStamped);
}

void getStreamCalibData(const intrinsics *intrinsic, sensor_msgs::CameraInfo *camera_info)
{
    camera_info->height = intrinsic->height;
    camera_info->width = intrinsic->width;
    const std::string model_names[distortion::DISTORTION_COUNT] = {
        "unknown", "brown_conrady", "modified_brown_conrady", "inverse_brown_conrady", "ftheta", "kannala_brandt4"};
    camera_info->distortion_model = model_names[intrinsic->model];
    camera_info->D.resize(5);
    for (int i = 0; i < 5; i++)
    {
        camera_info->D.at(i) = intrinsic->coeffs[i];
    }
    camera_info->K.at(0) = intrinsic->fx;
    camera_info->K.at(2) = intrinsic->cx;
    camera_info->K.at(4) = intrinsic->fy;
    camera_info->K.at(5) = intrinsic->cy;
    camera_info->K.at(8) = 1;

    // set R (rotation matrix) values to identity matrix
    camera_info->R.at(0) = 1.0;
    camera_info->R.at(1) = 0.0;
    camera_info->R.at(2) = 0.0;
    camera_info->R.at(3) = 0.0;
    camera_info->R.at(4) = 1.0;
    camera_info->R.at(5) = 0.0;
    camera_info->R.at(6) = 0.0;
    camera_info->R.at(7) = 0.0;
    camera_info->R.at(8) = 1.0;

    camera_info->P.at(0) = camera_info->K.at(0);
    camera_info->P.at(1) = 0;
    camera_info->P.at(2) = camera_info->K.at(2);
    camera_info->P.at(3) = 0;
    camera_info->P.at(4) = 0;
    camera_info->P.at(5) = camera_info->K.at(4);
    camera_info->P.at(6) = camera_info->K.at(5);
    camera_info->P.at(7) = 0;
    camera_info->P.at(8) = 0;
    camera_info->P.at(9) = 0;
    camera_info->P.at(10) = 1;
    camera_info->P.at(11) = 0;

    camera_info->binning_x = 0;
    camera_info->binning_y = 0;
    camera_info->roi.x_offset = 0;
    camera_info->roi.y_offset = 0;
    camera_info->roi.height = 0;
    camera_info->roi.width = 0;
    camera_info->roi.do_rectify = 0;
    return;
}

void getExtrinsic(const extrinsics *extrinsic, vidu_ros_wrapper::Extrinsics *extrinsic_info)
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
    ros::init(argc, argv, "vidu_ros_wrapper_node");
    ros::NodeHandle node;

    ros::Publisher origin_color_pub = node.advertise<sensor_msgs::Image>("camera/color/origin", 2);
    ros::Publisher aligned_color_pub = node.advertise<sensor_msgs::Image>("camera/color/aligned", 2);
    ros::Publisher depth_pub = node.advertise<sensor_msgs::Image>("camera/depth", 2);
    ros::Publisher infrared_pub = node.advertise<sensor_msgs::Image>("camera/infrared", 2);
    ros::Publisher pointcloud_pub = node.advertise<sensor_msgs::PointCloud2>("camera/pointcloud", 2);

    auto color_camera_info_pub = node.advertise<sensor_msgs::CameraInfo>("camera/info/color", 2);
    auto depth_camera_info_pub = node.advertise<sensor_msgs::CameraInfo>("camera/info/depth", 2);
    auto color2depth_info_pub = node.advertise<vidu_ros_wrapper::Extrinsics>("camera/extrinsics/color_to_depth", 2);

    int count = 0;

    PDdevice devInst;
    sensor_msgs::CameraInfo camera_color_info;
    sensor_msgs::CameraInfo camera_depth_info;
    vidu_ros_wrapper::Extrinsics color2depth_info;

    if (devInst.init())
    {
        ROS_INFO("Camera device : %s\n", devInst.getDeviceSN().c_str());

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
            ROS_INFO("Failed to get RGB intrinsic and extrinsic\n");
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
            ROS_INFO("Failed to get ToF intrinsic and extrinsic\n");
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

            sensor_msgs::Image msg_color;
            sensor_msgs::Image msg_aligned_color;
            sensor_msgs::PointCloud2 msg_pointcloud;

            // broadcasting the coordinate of the pointcloud
            ROS_INFO("Camera ready");
            while (ros::ok())
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
                    const PDimage &xyz = pPclFrame->getImage(0);
                    const PDimage &infrared = pPclFrame->getImage(1);
                    const PDimage &alignedColor = pPclFrame->getImage(2);
                    // should pclstream.set("PCL::EnableRgbAttach", true);
                    const PDimage &originColor = pPclFrame->getImage(3);

                    cv::Mat xyz_mat = cv::Mat(cv::Size(xyz.GetImageWidth(), xyz.GetImageHeight()), xyz.GetImageCVType(),
                                              xyz.GetImageData());
                    std::vector<cv::Mat> channels(3);
                    cv::split(xyz_mat, channels);
                    const cv::Mat &depth = channels[2];
                    cv::Mat u16Depth;
                    depth.convertTo(u16Depth, CV_16UC1, 65535.0 / DistRange);

                    // cv::imshow("xyz", xyz);
                    // cv::imshow("infrared", infrared);
                    // cv::imshow("alignedColor", alignedColor);
                    // cv::imshow("depth", depth);
                    // cv::imshow("originColor", originColor);
                    cv::Mat originColor_mat =
                        cv::Mat(cv::Size(originColor.GetImageWidth(), originColor.GetImageHeight()),
                                originColor.GetImageCVType(), originColor.GetImageData());
                    cv::Mat infrared_mat = cv::Mat(cv::Size(infrared.GetImageWidth(), infrared.GetImageHeight()),
                                                   infrared.GetImageCVType(), infrared.GetImageData());
                    cv::Mat alignedColor_mat =
                        cv::Mat(cv::Size(alignedColor.GetImageWidth(), alignedColor.GetImageHeight()),
                                alignedColor.GetImageCVType(), alignedColor.GetImageData());

                    auto frame_get_time = ros::Time::now();
                    std_msgs::Header header_color;
                    header_color.seq = count;
                    header_color.frame_id = "camera_color_frame";
                    header_color.stamp = frame_get_time;
                    cv_bridge::CvImage origin_image(header_color, sensor_msgs::image_encodings::TYPE_8UC3,
                                                    originColor_mat);

                    std_msgs::Header header_depth;
                    header_depth.seq = count;
                    header_depth.frame_id = "camera_depth_frame";
                    header_depth.stamp = frame_get_time;
                    cv_bridge::CvImage depth_image(header_depth, sensor_msgs::image_encodings::TYPE_16UC1, u16Depth);
                    cv_bridge::CvImage infrared_image(header_depth, sensor_msgs::image_encodings::TYPE_16UC1,
                                                      infrared_mat);

                    std_msgs::Header header_aligned_color;
                    header_aligned_color.seq = count;
                    header_aligned_color.frame_id = "camera_aligned_color_frame";
                    header_aligned_color.stamp = frame_get_time;
                    cv_bridge::CvImage aligned_image(header_aligned_color, sensor_msgs::image_encodings::TYPE_8UC3,
                                                     alignedColor_mat);
                    // TODO: reorganize xyz, rgb to PointCloud2  msg
                    std_msgs::Header header_pcl;
                    header_pcl.seq = count;
                    header_pcl.frame_id = "camera_pcl_frame";
                    header_pcl.stamp = frame_get_time;
                    msg_pointcloud = img_to_cloud(alignedColor_mat, xyz_mat, header_pcl);

                    camera_color_info.header = header_color;
                    camera_depth_info.header = header_depth;
                    color2depth_info.header = header_color;
                    // TODO: there is  a  image data copy operation
                    color_camera_info_pub.publish(camera_color_info);
                    depth_camera_info_pub.publish(camera_depth_info);
                    color2depth_info_pub.publish(color2depth_info);
                    origin_color_pub.publish(origin_image.toImageMsg());
                    aligned_color_pub.publish(aligned_image.toImageMsg());
                    depth_pub.publish(depth_image.toImageMsg());
                    infrared_pub.publish(infrared_image.toImageMsg());
                    pointcloud_pub.publish(msg_pointcloud);

                    sendTransform(header_pcl.frame_id);

                    ros::spinOnce();

                    ++count;
                }
                pPclFrame.reset(); // plz. release frame buffer imediately
                // std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
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
