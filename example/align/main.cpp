#include "Vidu_SDK_Api.h"

#include "opencv2/opencv.hpp"

int main(int argc, char *argv[])
{
    std::string rgb_image;
    std::string tof_image;
    std::string intrinsics_file;
    std::string extrinsics_file;
    float max_value = 0.0f;

    for (int i = 0; i < argc; ++i)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            std::cout << "Usage: align [--help] [--rgb_image path] [--tof_image path] [--intrinsics path] "
                         "[--extrinsics path] [--tof_max_value value], ps: path requires absolute path\n"
                      << std::endl;
            std::cout << "Optional arguments:" << std::endl;
            std::cout << "  -h, --help            shows help message and exits" << std::endl;
            std::cout << "  -r, --rgb_image       the rgb image file path" << std::endl;
            std::cout << "  -t, --tof_image       the tof image file path" << std::endl;
            std::cout << "  -i, --intrinsics      the intrinsics of camera" << std::endl;
            std::cout << "  -e, --extrinsics      the extrinsics of camera" << std::endl;
            std::cout << "  -m, --tof_max_value   the max distance or depth value of tof image" << std::endl;
            return 0;
        }
        else if (((strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--rgb_image") == 0)) && (i + 1) < argc &&
                 argv[i + 1][0] != '-')
        {
            rgb_image = argv[i + 1];
        }
        else if (((strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--tof_image") == 0)) && (i + 1) < argc &&
                 argv[i + 1][0] != '-')
        {
            tof_image = argv[i + 1];
        }
        else if (((strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--intrinsics") == 0)) && (i + 1) < argc &&
                 argv[i + 1][0] != '-')
        {
            intrinsics_file = argv[i + 1];
        }
        else if (((strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--extrinsics") == 0)) && (i + 1) < argc &&
                 argv[i + 1][0] != '-')
        {
            extrinsics_file = argv[i + 1];
        }
        else if (((strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--extrinsics") == 0)) && (i + 1) < argc &&
                 argv[i + 1][0] != '-')
        {
            extrinsics_file = argv[i + 1];
        }
        else if (((strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--tof_max_value") == 0)) && (i + 1) < argc &&
                 argv[i + 1][0] != '-')
        {
            max_value = std::atof(argv[i + 1]);
        }
    }

    if (rgb_image.empty() || tof_image.empty() || intrinsics_file.empty() || extrinsics_file.empty() ||
        max_value == 0.0f)
    {
        std::cout << "Input parameters cannot be empty, need input rgb & tof image file path and intrinsics & "
                     "extrinsics file of camera, tof max value cannot be 0.0"
                  << std::endl;
        return -1;
    }

    int32_t width = 0, height = 0;
    float km[3][3] = {};
    float cx = 0.0f, cy = 0.0f, fx = 0.0f, fy = 0.0f;
    distortion model = distortion::DISTORTION_UNKNOWN;
    float distk[5] = {};

    cv::FileStorage intrin_fs(intrinsics_file, cv::FileStorage::FORMAT_JSON | cv::FileStorage::READ);
    cv::FileNode tof_node = intrin_fs["tof"];
    width = tof_node["width"];
    height = tof_node["height"];
    cv::FileNode kn = tof_node["K"];
    for (size_t i = 0; i < kn.size(); ++i)
    {
        if (i >= 3)
        {
            break;
        }

        cv::FileNode kln = kn[i];
        for (size_t j = 0; j < kln.size(); ++j)
        {
            if (j >= 3)
            {
                break;
            }

            km[i][j] = kln[j];
        }
    }
    fx = km[0][0];
    fy = km[1][1];
    cx = km[0][2];
    cy = km[1][2];
    cv::FileNode dn = tof_node["distortion"];
    model = (distortion)((int32_t)dn["model"]);
    cv::FileNode dkn = dn["distortion"];
    for (size_t i = 0; i < dkn.size(); ++i)
    {
        if (i >= 5)
        {
            break;
        }

        distk[i] = dkn[i];
    }

    intrinsics depth_intrin(width, height, cx, cy, fx, fy, model, distk[0], distk[1], distk[2], distk[3], distk[4]);
    cv::Mat depth_k = (cv::Mat1f(3, 3) << depth_intrin.fx, 0.0f, depth_intrin.cx, 0.0f, depth_intrin.fy,
                       depth_intrin.cy, 0.0f, 0.0f, 1.0f);
    cv::Mat depth_kinv = depth_k.inv();
    cv::Mat depth_distort = cv::Mat(cv::Size(4, 1), CV_32FC1, depth_intrin.coeffs);

    cv::FileNode rgb_node = intrin_fs["rgb"];
    width = rgb_node["width"];
    height = rgb_node["height"];
    kn = rgb_node["K"];
    for (size_t i = 0; i < kn.size(); ++i)
    {
        if (i >= 3)
        {
            break;
        }

        cv::FileNode kln = kn[i];
        for (size_t j = 0; j < kln.size(); ++j)
        {
            if (j >= 3)
            {
                break;
            }

            km[i][j] = kln[j];
        }
    }
    fx = km[0][0];
    fy = km[1][1];
    cx = km[0][2];
    cy = km[1][2];
    dn = rgb_node["distortion"];
    model = (distortion)((int32_t)dn["model"]);
    dkn = dn["distortion"];
    for (size_t i = 0; i < dkn.size(); ++i)
    {
        if (i >= 5)
        {
            break;
        }

        distk[i] = dkn[i];
    }

    intrinsics texture_intrin(width, height, cx, cy, fx, fy, model, distk[0], distk[1], distk[2], distk[3], distk[4]);
    cv::Mat texture_k = (cv::Mat1f(3, 3) << texture_intrin.fx, 0.0f, texture_intrin.cx, 0.0f, texture_intrin.fy,
                         texture_intrin.cy, 0.0f, 0.0f, 1.0f);
    cv::Mat texture_kinv = texture_k.inv();
    cv::Mat texture_distort = cv::Mat(cv::Size(4, 1), CV_32FC1, texture_intrin.coeffs);

    float rm[3][3] = {};
    float tm[3] = {};

    cv::FileStorage extrin_fs(extrinsics_file, cv::FileStorage::FORMAT_JSON | cv::FileStorage::READ);
    cv::FileNode rn = extrin_fs["rot"];
    for (size_t i = 0; i < rn.size(); ++i)
    {
        if (i >= 3)
        {
            break;
        }

        cv::FileNode rln = rn[i];
        for (size_t j = 0; j < rln.size(); ++j)
        {
            if (j >= 3)
            {
                break;
            }

            rm[i][j] = rln[j];
        }
    }
    cv::FileNode tn = extrin_fs["trans"];
    for (size_t i = 0; i < tn.size(); ++i)
    {
        if (i >= 3)
        {
            break;
        }

        tm[i] = tn[i];
    }

    extrinsics extrin;
    memcpy(extrin.rotation, rm, sizeof(rm));
    memcpy(extrin.translation, tm, sizeof(tm));

    cv::Mat rotation = cv::Mat(cv::Size(3, 3), CV_32FC1, extrin.rotation);
    cv::Mat translation = cv::Mat(cv::Size(1, 3), CV_32FC1, extrin.translation);

    // load texture and depth pha image
    cv::Mat texture = cv::imread(rgb_image);
    cv::Mat pha = cv::imread(tof_image, cv::IMREAD_ANYDEPTH);

    // depth pha convert to depth, need to divide the range by 65536 (max value of uint16_t)
    // range can be obtained through metadata
    cv::Mat depth = cv::Mat(cv::Size(depth_intrin.width, depth_intrin.height), CV_32FC1);
    pha.convertTo(depth, CV_32F, max_value / 65536.0f);

    // undistort
    switch (depth_intrin.model)
    {
    case distortion::DISTORTION_BROWN_CONRADY: {
        cv::undistort(depth, depth, depth_k, depth_distort, depth_k);
        cv::undistort(texture, texture, texture_k, texture_distort, texture_k);
        break;
    }
    case distortion::DISTORTION_KANNALA_BRANDT4: {
        cv::fisheye::undistortImage(depth, depth, depth_k, depth_distort, depth_k,
                                    cv::Size(depth_intrin.width, depth_intrin.height));
        cv::fisheye::undistortImage(texture, texture, texture_k, texture_distort, texture_k,
                                    cv::Size(texture_intrin.width, texture_intrin.height));
        break;
    }
    default: {
        break;
    }
    }

    // align texture and depth
    cv::Mat depth_align_to_texture = cv::Mat(cv::Size(texture_intrin.width, texture_intrin.height), CV_32FC1);
    cv::Mat texture_align_to_depth = cv::Mat(cv::Size(depth_intrin.width, depth_intrin.height), CV_8UC3);
    for (int32_t row = 0; row < depth_intrin.height; ++row)
    {
        for (int32_t col = 0; col < depth_intrin.width; ++col)
        {
            float depth_value = depth.at<float>(row, col);

            // deproject depth pixel to pcl point, ps: col is x, row is y
            cv::Mat pixel = (cv::Mat1f(3, 1) << static_cast<float>(col), static_cast<float>(row), 1.0f);
            cv::Mat point = depth_value * depth_kinv * pixel;

            // move to rgb lens
            point = rotation * point + translation;

            // project pcl point to texture pixel
            pixel = texture_k * point / depth_value;
            int32_t x = static_cast<int32_t>(pixel.at<float>(0, 0));
            int32_t y = static_cast<int32_t>(pixel.at<float>(1, 0));

            // filter pixel
            if (x < 0 || x >= texture_intrin.width || y < 0 || y >= texture_intrin.height)
            {
                continue;
            }

            // align
            depth_align_to_texture.at<float>(y, x) = depth_value;

            auto &align_color = texture_align_to_depth.at<cv::Vec3b>(row, col);
            auto &origin_color = texture.at<cv::Vec3b>(y, x);
            align_color[0] = origin_color[0];
            align_color[1] = origin_color[1];
            align_color[2] = origin_color[2];
        }
    }

    // blended
    cv::normalize(depth_align_to_texture, depth_align_to_texture, 0.0, 255.0, cv::NORM_MINMAX, CV_8UC1);
    cv::applyColorMap(depth_align_to_texture, depth_align_to_texture, cv::COLORMAP_TURBO);
    cv::Mat blended_1;
    cv::addWeighted(texture, 0.1, depth_align_to_texture, 0.9, 0.0, blended_1);

    cv::normalize(depth, depth, 0.0, 255.0, cv::NORM_MINMAX, CV_8UC1);
    cv::applyColorMap(depth, depth, cv::COLORMAP_TURBO);
    cv::Mat blended_2;
    cv::addWeighted(depth, 0.9, texture_align_to_depth, 0.1, 0.0, blended_2);

    // render
    while (true)
    {
        cv::imshow("depth", depth);
        cv::imshow("tex", texture);

        cv::imshow("depth align to texture", depth_align_to_texture);
        cv::imshow("depth texture to align", texture_align_to_depth);

        cv::imshow("blended 1", blended_1);
        cv::imshow("blended 2", blended_2);

        auto key = cv::waitKey(1);
        if (key == 'q')
        {
            break;
        }
    }

    return 0;
}
