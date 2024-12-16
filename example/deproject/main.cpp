#include "Vidu_SDK_Api.h"

#include <cfloat>
#include <cmath>
#include <iostream>
#include <thread>

#include "opencv2/opencv.hpp"

/**
 * @param[out] point camera coordinate, 3D points
 * @param[in] intrin camera intrinsics
 * @param[in] pixel depth image pixel coordinates
 * @param[in] depth values of the coordinates corresponding to the depth image, unit m
 */
void DeprojectPixelToPoint(float point[3], intrinsics &intrin, const float pixel[2], const float depth)
{
    float x = (pixel[0] - intrin.cx) / intrin.fx;
    float y = (pixel[1] - intrin.cy) / intrin.fy;

    switch (intrin.model)
    {
    case DISTORTION_BROWN_CONRADY: {
        float xo = x;
        float yo = y;
        for (int i = 0; i < 10; i++)
        {
            float r2 = x * x + y * y;
            float icdist = 1 / (1 + ((intrin.coeffs[4] * r2 + intrin.coeffs[1]) * r2 + intrin.coeffs[0]) * r2);
            float delta_x = 2 * intrin.coeffs[2] * x * y + intrin.coeffs[3] * (r2 + 2 * x * x);
            float delta_y = 2 * intrin.coeffs[3] * x * y + intrin.coeffs[2] * (r2 + 2 * y * y);
            x = (xo - delta_x) * icdist;
            y = (yo - delta_y) * icdist;
        }
        break;
    }
    case DISTORTION_KANNALA_BRANDT4: {
        float rd = sqrtf(x * x + y * y);
        if (rd < FLT_EPSILON)
        {
            rd = FLT_EPSILON;
        }

        float theta = rd;
        float theta2 = rd * rd;
        for (int i = 0; i < 4; i++)
        {
            float f = theta * (1 + theta2 * (intrin.coeffs[0] +
                                             theta2 * (intrin.coeffs[1] +
                                                       theta2 * (intrin.coeffs[2] + theta2 * intrin.coeffs[3])))) -
                      rd;
            if (fabs(f) < FLT_EPSILON)
            {
                break;
            }
            float df = 1 + theta2 * (3 * intrin.coeffs[0] +
                                     theta2 * (5 * intrin.coeffs[1] +
                                               theta2 * (7 * intrin.coeffs[2] + 9 * theta2 * intrin.coeffs[3])));
            theta -= f / df;
            theta2 = theta * theta;
        }
        float r = tan(theta);
        x *= r / rd;
        y *= r / rd;
        break;
    }
    default:
        break;
    }

    point[0] = depth * x;
    point[1] = depth * y;
    point[2] = depth;
}

void TransformPoint(float out_point[3], extrinsics &extrin, float in_point[3])
{
    out_point[0] = extrin.rotation[0] * in_point[0] + extrin.rotation[1] * in_point[1] +
                   extrin.rotation[2] * in_point[2] + extrin.translation[0];
    out_point[1] = extrin.rotation[3] * in_point[0] + extrin.rotation[4] * in_point[1] +
                   extrin.rotation[5] * in_point[2] + extrin.translation[1];
    out_point[2] = extrin.rotation[6] * in_point[0] + extrin.rotation[7] * in_point[1] +
                   extrin.rotation[8] * in_point[2] + extrin.translation[2];
}

void ProjectPointToPixel(float pixel[2], const intrinsics &intrin, float point[3])
{
    float x = point[0] / point[2];
    float y = point[1] / point[2];
    switch (intrin.model)
    {
    case distortion::DISTORTION_BROWN_CONRADY: {
        float xy = x * y;
        float r2 = x * x + y * y;
        float f =
            (((intrin.coeffs[4] * 0.5f * r2 + intrin.coeffs[1]) * r2 * 0.5f) + intrin.coeffs[0]) * r2 * 0.5f + 1.0f;
        float xf = x * f * intrin.fx;
        float yf = y * f * intrin.fy;

        x = xf + (intrin.coeffs[2] * intrin.fx) * xy + (intrin.coeffs[3] * 0.5f * intrin.fx) * r2 +
            (intrin.coeffs[3] * intrin.fx) * (x * x);
        y = yf + (intrin.coeffs[3] * intrin.fy) * xy + (intrin.coeffs[2] * 0.5f * intrin.fy) * r2 +
            (intrin.coeffs[2] * intrin.fy) * (y * y);

        pixel[0] = x + intrin.cx;
        pixel[1] = y + intrin.cy;

        break;
    }
    case distortion::DISTORTION_KANNALA_BRANDT4: {
        float r = std::sqrt(x * x + y * y);
        if (r < FLT_EPSILON)
        {
            r = FLT_EPSILON;
        }

        float theta = std::atan(r);
        float theta2 = theta * theta;
        float series =
            1 + theta2 * (intrin.coeffs[0] +
                          theta2 * (intrin.coeffs[1] + theta2 * (intrin.coeffs[2] + theta2 * intrin.coeffs[3])));
        r = theta * series / r;

        pixel[0] = x * r * intrin.fx + intrin.cx;
        pixel[1] = y * r * intrin.fy + intrin.cy;

        break;
    }
    default:
        break;
    }
}

int main()
{
    PDdevice dev_inst;
    if (!dev_inst.init())
    {
        std::cout << "no valid device" << std::endl;
        return -1;
    }

    auto rgb_stream = PDstream(dev_inst, "RGB");
    auto tof_stream = PDstream(dev_inst, "ToF");
    if (!tof_stream.init() || !rgb_stream.init())
    {
        std::cout << "stream init failed" << std::endl;
        return -1;
    }

    intrinsics rgb_intrin;
    intrinsics tof_intrin;
    extrinsics rgb_extrin;
    extrinsics tof_extrin;
    rgb_stream.getCamPara(rgb_intrin, rgb_extrin);
    tof_stream.getCamPara(tof_intrin, tof_extrin);
    print_intrinsics(&rgb_intrin);
    print_extrinsics(&rgb_extrin);
    print_intrinsics(&tof_intrin);
    print_extrinsics(&tof_extrin);

    while (true)
    {
        auto rgb_frame = rgb_stream.waitFrames();
        auto tof_frame = tof_stream.waitFrames();
        int32_t key = 0;
        if (tof_frame)
        {
            const PDimage &pha = tof_frame->getImage(0);
            const PDimage &infrared = tof_frame->getImage(1);

            cv::Mat pha_mat =
                cv::Mat(cv::Size(pha.GetImageWidth(), pha.GetImageHeight()), pha.GetImageCVType(), pha.GetImageData());
            cv::Mat infrared_mat = cv::Mat(cv::Size(infrared.GetImageWidth(), infrared.GetImageHeight()),
                                           infrared.GetImageCVType(), infrared.GetImageData());
            cv::imshow("pha", pha_mat);
            cv::imshow("inrared", infrared_mat);
            key = cv::waitKey(1);

            float depth = static_cast<float>(pha_mat.at<uint16_t>(240, 320));
            std::cout << "tof pixel row: 240, col: 320, value: " << depth << std::endl;
            float pixel_coord[] = {320.0f, 240.0f};
            float point[3] = {};
            depth = depth * 0.001f; // unit mm -> m
            DeprojectPixelToPoint(point, tof_intrin, pixel_coord, depth);
            std::cout << "x: " << point[0] << ", y: " << point[1] << ", z: " << point[2] << std::endl;
            // move to rgb coord
            float trans_point[3] = {};
            TransformPoint(trans_point, rgb_extrin, point);
            float rgb_pixel[2] = {};
            ProjectPointToPixel(rgb_pixel, rgb_intrin, trans_point);
            std::cout << "rgb pixel row: " << rgb_pixel[1] << ", col: " << rgb_pixel[0] << std::endl;

            if (key == 'q')
            {
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}
