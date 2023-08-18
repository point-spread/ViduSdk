// This file is ....

#include <fstream>
#include <iostream>
#include <vector>

// Okulo SDK
#include "Vidu_SDK_Api.h"

// OpenCV
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/rgbd/kinfu.hpp>

using namespace cv;
using namespace cv::kinfu;

#ifdef HAVE_OPENCV_VIZ
#include <opencv2/viz.hpp>
#endif

#ifdef HAVE_OPENCV_VIZ
const std::string vizWindowName = "cloud";

void pauseCallback(const viz::MouseEvent &me, void *args);
#endif

void getCameraIntrinsics(PDstream &PCLstream, intrinsics &intrin);
void setParamsForKinfusion(intrinsics &intrin, kinfu::Params &params, UMat &undistortMap1, UMat &undistortMap2);
void savePly(const std::string &output_file_name, UMat &res_points, UMat &res_normals);

static const char *keys = {
    "{help h usage ? | | print this message   }"
    "{coarse | | Run on coarse settings (fast but ugly) or on default (slow but looks better),"
    " in coarse mode points and normals are displayed }"
    "{useHashTSDF | | Use the newer hashtable based TSDFVolume (relatively fast) and for larger reconstructions}"
    "{idle   | | Do not run KinFu, just display depth frames }"
    "{record | | Write result model to specified file}"};

static const std::string message =
    "\nThis demo uses live depth input from Okulo P1 camera to run KinectFusion example \n";

int main(int argc, char **argv)
{
    bool coarse = false;
    bool idle = false;
    bool useHashTSDF = false;

    std::string model_name("result");

    CommandLineParser parser(argc, argv, keys);
    parser.about(message);

    if (!parser.check())
    {
        parser.printMessage();
        parser.printErrors();
        return -1;
    }

    if (parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }
    if (parser.has("coarse"))
    {
        coarse = true;
    }
    if (parser.has("record"))
    {
        model_name = parser.get<String>("record");
        std::cout << "result model name:" << model_name << std::endl;
    }
    if (parser.has("useHashTSDF"))
    {
        useHashTSDF = true;
    }
    if (parser.has("idle"))
    {
        idle = true;
    }

    // open camera
    PDdevice devInst;
    float DistRange;
    if (!devInst.init())
    {
        printf("no valid device\n");
        return -1;
    }

    PDstream PCLstream = PDstream(devInst, "PCL");
    if (PCLstream.init())
    {
        bool isTofAutoExposure = false;
        bool isRGBAutoExposure = false;
        PCLstream.get("ToF::AutoExposure", isTofAutoExposure);
        PCLstream.get("RGB::AutoExposure", isRGBAutoExposure);
        if (isTofAutoExposure)
        {
            PCLstream.set("ToF::AutoExposure", false);
        }
        if (isRGBAutoExposure)
        {
            PCLstream.set("RGB::AutoExposure", false);
        }

        PCLstream.set("ToF::Distance", 7.5f);
        PCLstream.set("ToF::StreamFps", 30.0f);
        PCLstream.set("ToF::Threshold", 100);
        PCLstream.set("ToF::Exposure", 1.0f);
        PCLstream.set("ToF::DepthFlyingPixelRemoval", 10);
        PCLstream.set("ToF::DepthSmoothStrength", 1);

        PCLstream.set("RGB::Exposure", 15.0f);
        PCLstream.set("RGB::Gain", 5.0f);
        DistRange = 0.0f;
    }
    else
    {
        return -1;
    }

    // get ToF Intrinsics
    intrinsics intrin;
    getCameraIntrinsics(PCLstream, intrin);

    // undistort map
    UMat undistortMap1, undistortMap2;

    // params
    Ptr<Params> params;

    // create kinfusion instance
    Ptr<KinFu> kf;

    if (coarse)
        params = Params::coarseParams();
    else
        params = Params::defaultParams();

    // if (useHashTSDF)
    //     params = Params::hashTSDFParams(coarse);

    // setParamsForKinfusion
    setParamsForKinfusion(intrin, *params, undistortMap1, undistortMap2);

    // Enables OpenCL explicitly (by default can be switched-off)
    cv::setUseOptimized(true);

    // update params
    if (!idle)
        kf = KinFu::create(params);

#ifdef HAVE_OPENCV_VIZ
    cv::viz::Viz3d window(vizWindowName);
    window.setViewerPose(Affine3f::Identity());
    bool pause = false;
#endif

    UMat rendered;
    UMat points;
    UMat normals;

    int64 prevTime = getTickCount();

    while (1)
    {
        auto pPclFrame = PCLstream.waitFrames(); // pcl frames work only when tof && rgb stream grab frames!!!
        int key = waitKey(1);

        if (pPclFrame)
        {
            if (DistRange < 1e-5) // DistRange should be inited
            {
                size_t varSize = sizeof(varSize);
                GenTL::PDBufferGetMetaByName(pPclFrame->getPort(), "Range", nullptr, &DistRange, &varSize);
                printf("max distance %f for distanceMap", DistRange);
            }

            const cv::Mat &xyz = pPclFrame->getMat(0);
            std::vector<cv::Mat> channels(3);
            cv::split(xyz, channels);
            const cv::Mat &depth = channels[2];
            cv::Mat f;
            depth.convertTo(f, CV_16UC1, 1000); // 1000 -> depth factor

            UMat frame(f.cols, f.rows, CV_16UC1);
            f.copyTo(frame);

#ifdef HAVE_OPENCV_VIZ
            if (pause)
            {
                // doesn't happen in idle mode
                kf->getCloud(points, normals);
                if (!points.empty() && !normals.empty())
                {
                    viz::WCloud cloudWidget(points, viz::Color::white());
                    viz::WCloudNormals cloudNormals(points, normals, /*level*/ 1, /*scale*/ 0.05, viz::Color::gray());
                    window.showWidget("cloud", cloudWidget);
                    window.showWidget("normals", cloudNormals);

                    Vec3d volSize = kf->getParams().voxelSize * Vec3d(kf->getParams().volumeDims);
                    window.showWidget("cube", viz::WCube(Vec3d::all(0), volSize), kf->getParams().volumePose);

                    window.registerMouseCallback(pauseCallback, (void *)&kf);
                    window.showWidget("text", viz::WText(cv::String("Move camera in this window. "
                                                                    "Close the window or press Q to resume"),
                                                         Point()));
                    window.spin();
                    window.removeWidget("text");
                    window.removeWidget("cloud");
                    window.removeWidget("normals");
                    window.registerMouseCallback(0);
                }

                pause = false;
            }
            else
#endif
            {
                UMat cvt8;
                float depthFactor = params->depthFactor;
                convertScaleAbs(frame, cvt8, 256. / depthFactor);
                if (!idle)
                {
                    imshow("depth", cvt8);

                    if (!kf->update(frame))
                    {
                        kf->reset(); // If the algorithm failed, reset current state
                        std::cout << "reset" << std::endl;
                    }
#ifdef HAVE_OPENCV_VIZ
                    else
                    {
                        if (coarse)
                        {
                            kf->getCloud(points, normals);
                            if (!points.empty() && !normals.empty())
                            {
                                viz::WCloud cloudWidget(points, viz::Color::white());
                                viz::WCloudNormals cloudNormals(points, normals, /*level*/ 1, /*scale*/ 0.05,
                                                                viz::Color::gray());
                                window.showWidget("cloud", cloudWidget);
                                window.showWidget("normals", cloudNormals);
                            }
                        }

                        // window.showWidget("worldAxes", viz::WCoordinateSystem());
                        Vec3d volSize = kf->getParams().voxelSize * kf->getParams().volumeDims;
                        window.showWidget("cube", viz::WCube(Vec3d::all(0), volSize), kf->getParams().volumePose);
                        window.setViewerPose(kf->getPose());
                        window.spinOnce(1, true);
                    }
#endif

                    kf->render(rendered);
                }
                else
                {
                    rendered = cvt8;
                }
            }

            int c = waitKey(1);

            if (c == 'q')
                break;
            else if (c == 'r' && !idle)
                kf->reset();
#ifdef HAVE_OPENCV_VIZ
            else if (c == 'p' && !idle)
                pause = true;
#endif

            int64 newTime = getTickCount();
            putText(rendered,
                    cv::format("FPS: %2d press R to reset, P to pause, Q to quit",
                               (int)(getTickFrequency() / (newTime - prevTime))),
                    Point(0, rendered.rows - 1), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 255));
            prevTime = newTime;

            imshow("render", rendered);
        }
    }

    if (!idle)
    {
        // doesn't happen in idle mode
        cv::UMat res_points;
        cv::UMat res_normals;
        kf->getCloud(res_points, res_normals);
        if (!res_points.empty() && !res_normals.empty())
        {
#ifdef HAVE_OPENCV_VIZ
            // show result
            viz::WCloud cloudWidget(res_points, viz::Color::white());
            viz::WCloudNormals cloudNormals(res_points, res_normals, /*level*/ 1, /*scale*/ 0.05, viz::Color::gray());
            window.showWidget("cloud", cloudWidget);
            window.showWidget("normals", cloudNormals);

            Vec3d volSize = kf->getParams().voxelSize * Vec3d(kf->getParams().volumeDims);
            window.showWidget("cube", viz::WCube(Vec3d::all(0), volSize), kf->getParams().volumePose);

            window.registerMouseCallback(pauseCallback, (void *)&kf);
            window.showWidget("text", viz::WText(cv::String("Move camera in this window. "
                                                            "Close the window or press Q to resume"),
                                                 Point()));
            window.spin();
            window.removeWidget("text");
            window.removeWidget("cloud");
            window.removeWidget("normals");
            window.registerMouseCallback(0);
#endif
            //  save result
            savePly(model_name + ".ply", res_points, res_normals);
        }
    }

    return 0;
}

void getCameraIntrinsics(PDstream &PCLstream, intrinsics &intrin)
{
    extrinsics extrin;
    PCLstream.getCamPara(intrin, extrin);
    print_intrinsics(&intrin);
}

void setParamsForKinfusion(intrinsics &intrin, kinfu::Params &params, UMat &undistortMap1, UMat &undistortMap2)
{
    int w = intrin.width;
    int h = intrin.height;

    float fx = intrin.fx;
    float fy = intrin.fy;
    float cx = intrin.cx;
    float cy = intrin.cy;

    float k1 = intrin.coeffs[0];
    float k2 = intrin.coeffs[1];
    float p1 = intrin.coeffs[2];
    float p2 = intrin.coeffs[3];
    float k3 = intrin.coeffs[4];

    // set intrinsics
    params.intr = Matx33f(fx, 0, cx, 0, fy, cy, 0, 0, 1);

    // set frame size of depth image
    params.frameSize = Size(w, h);

    // depth factor
    params.depthFactor = 1000.f;

    // set volume params ---- for TSDF
    // These parameters should carefully tuned to adapt to different distance mode
    float volumeSize = 4.0f;
    params.voxelSize = volumeSize / params.volumeDims[0];
    params.tsdf_trunc_dist = 0.03f;
    params.truncateThreshold = 6.f;

    params.volumePose = Affine3f().translate(Vec3f(-volumeSize / 2.f, -volumeSize / 2.f, 0.05f));

    // set icp params
    params.icpDistThresh = 0.1f;
    params.bilateral_sigma_depth = 0.04f;

    // init undistort rectify map
    Matx<float, 1, 5> distCoeffs;
    distCoeffs(0) = k1;
    distCoeffs(1) = k2;
    distCoeffs(2) = p1;
    distCoeffs(3) = p2;
    distCoeffs(4) = k3;
    initUndistortRectifyMap(params.intr, distCoeffs, cv::noArray(), params.intr, params.frameSize, CV_16SC2,
                            undistortMap1, undistortMap2);
}

#ifdef HAVE_OPENCV_VIZ
void pauseCallback(const viz::MouseEvent &me, void *args)
{
    if (me.type == viz::MouseEvent::Type::MouseMove || me.type == viz::MouseEvent::Type::MouseScrollDown ||
        me.type == viz::MouseEvent::Type::MouseScrollUp)
    {
        Ptr<KinFu> Kf = *((Ptr<KinFu> *)(args));
        viz::Viz3d window(vizWindowName);
        UMat rendered;
        Kf->render(rendered, window.getViewerPose().matrix);
        imshow("render", rendered);
        waitKey(1);
    }
}
#endif

void savePly(const std::string &output_file_name, UMat &res_points, UMat &res_normals)
{
    // Output the fused point cloud from KinectFusion
    const Mat &out_points = res_points.getMat(cv::ACCESS_READ);
    const Mat &out_normals = res_normals.getMat(cv::ACCESS_READ);

    printf("Saving fused point cloud into ply file ...\n");

    // Save to the ply file
#define PLY_START_HEADER "ply"
#define PLY_END_HEADER "end_header"
#define PLY_ASCII "format ascii 1.0"
#define PLY_ELEMENT_VERTEX "element vertex"
    std::ofstream ofs(output_file_name); // text mode first
    ofs << PLY_START_HEADER << std::endl;
    ofs << PLY_ASCII << std::endl;
    ofs << PLY_ELEMENT_VERTEX << " " << out_points.rows << std::endl;
    ofs << "property float x" << std::endl;
    ofs << "property float y" << std::endl;
    ofs << "property float z" << std::endl;
    ofs << "property float nx" << std::endl;
    ofs << "property float ny" << std::endl;
    ofs << "property float nz" << std::endl;
    ofs << PLY_END_HEADER << std::endl;
    ofs.close();

    std::stringstream ss;
    for (int i = 0; i < out_points.rows; ++i)
    {
        ss << out_points.at<float>(i, 0) << " " << out_points.at<float>(i, 1) << " " << out_points.at<float>(i, 2)
           << " " << out_normals.at<float>(i, 0) << " " << out_normals.at<float>(i, 1) << " "
           << out_normals.at<float>(i, 2) << std::endl;
    }
    std::ofstream ofs_text(output_file_name, std::ios::out | std::ios::app);
    ofs_text.write(ss.str().c_str(), (std::streamsize)ss.str().length());
}
