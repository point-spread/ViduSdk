#include <BundleFusion.h>
#include <GlobalAppState.h>
#include <algorithm>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <regex>
#include <string>
#include <sys/time.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        std::cout << "usage: ./bundle_fusion_okulo /path/to/zParametersDefault.txt "
                     "/path/to/zParametersBundlingDefault.txt /path/to/dataset"
                  << std::endl;
        return -1;
    }

    std::cout << "======================BundleFusion Okulo =========================" << std::endl;
    std::cout << "==  " << std::endl;
    std::cout << "==  This is an example usage for BundleFusion SDK interface" << std::endl;
    std::cout << "==  Author: FangGet" << std::endl;
    std::cout << "==  " << std::endl;
    std::cout << "===================================================================" << std::endl;

    std::string app_config_file = "";
    std::string bundle_config_file = "";
    std::string dataset_root = "";

    app_config_file = std::string(argv[1]);
    bundle_config_file = std::string(argv[2]);
    dataset_root = std::string(argv[3]);

    // step 1: init all resources
    if (!initBundleFusion(app_config_file, bundle_config_file))
    {
        std::cerr << "BundleFusion init failed, exit." << std::endl;
        return -1;
    }

    // read for bundlefusion dataset from http://graphics.stanford.edu/projects/bundlefusion/
    struct dirent *ptr;
    DIR *dir;
    dir = opendir(dataset_root.c_str());
    std::vector<std::string> filenames;

    // Find the file names matching with PCL_p2_RGB_tXXXXXXX.png
    // BundleFusion requires RGB file (PCL_p2_RGB_tXXXXXXX.png) and Depth file (PCL_p4_Depth_tXXXXXXX.png)
    while ((ptr = readdir(dir)) != nullptr)
    {
        // std::cout << "walk through:\t " << ptr->d_name << std::endl;
        if (ptr->d_name[0] == '.')
        {
            continue;
        }
        std::string filename = std::string(ptr->d_name);
        // std::cout << "\t filename:\t " << filename << std::endl;
        if (filename.size() < 10)
            continue;
        if ((filename.substr(0, 10) == "PCL_p2_RGB") && (filename.substr(filename.size() - 4, 4) == ".png"))
        {
            std::cout << "matched filename:\t " << filename << std::endl;
            filenames.push_back(filename);
        }
    }
    std::sort(filenames.begin(), filenames.end());
    int count = 0;
    for (auto &filename : filenames)
    {
        std::cout << "processing: \t" << filename << std::endl;

        std::string rgb_path = dataset_root + filename;
        std::cout << "expecting rgb file : \t" << rgb_path << std::endl;
        // waiting the streamShow to change depth-x.png to PCL_p4_Depth_tXXXXXXX.png
        std::string dep_path =
            dataset_root +
            std::regex_replace(std::regex_replace(filename, std::regex("RGB"), "Depth"), std::regex("p2"), "p4");
        // filename.replace("RGB", "Depth"); //.replace("p2", "p4");
        std::cout << "expecting depth file : \t" << dep_path << std::endl;

        static int i = 0;
        dep_path = dataset_root + std::string("depth-") + std::to_string(i++) + std::string(".png");
        std::cout << "currently try depth file : \t" << dep_path << std::endl;

        // std::string pos_path = data_root + "/" + filename + ".pose.txt";
        cv::Mat rgbImage = cv::imread(rgb_path);
        cv::Mat depthImage = cv::imread(dep_path, cv::IMREAD_UNCHANGED);

        if (rgbImage.empty() || depthImage.empty())
        {
            std::cout << "no image founded" << std::endl;
        }

        cv::imshow("rgb_image", rgbImage);
        cv::imshow("depth_image", depthImage * 50);
        char c = cv::waitKey(20);

        if (processInputRGBDFrame(rgbImage, depthImage))
        {
            count++;
            // if(count % 500 == 0)
            //     saveMeshIntoFile("model.ply", true);
            std::cout << "\tSuccess! frame " << filename << " added into BundleFusion." << std::endl;
        }
        else
        {
            // std::cout<<"\Failed! frame " << filename << " not added into BundleFusion." << std::endl;
        }

        if (c == 'q')
            break;
        else if (c == 's' || count % 1000 == 0)
        {
            std::cout << "saving ...... model ........\n";
            std::string filename = std::string("temp") + std::to_string(count) + std::string(".ply");
            saveMeshIntoFile(filename, true);
        }
    }

    saveMeshIntoFile("result.ply", true);

    deinitBundleFusion();

    return 0;
}
