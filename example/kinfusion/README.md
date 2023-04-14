
# Kinect fusion for Okulo P1 camera

## 1. Overview

KinectFusion is a real-time 3D reconstruction algorithm designed for consumer RGBD camera. It has been adopted by [opencv_contrib's `rgbd` module](https://docs.opencv.org/4.x/d8/d1f/classcv_1_1kinfu_1_1KinFu.html). By tracking the live depth frame relative to the global model with a coarse-to-fine iterative close point (ICP) algorithm, the current pose of sensor can be obtained. Then current frame can be fused into the global model to optimize it. More details of this algorithm can be found in [KinectFusion: Real-Time Dense Surface Mapping and Tracking](https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/ismar2011.pdf). 


## 2. OpenCV Installation

The full procedure is listed below.


```bash
cd kinfusion

# download opencv source code. The command below will make sure that the version of the cloned opencv repo is same as the version you installed before.
git clone https://github.com/opencv/opencv.git --branch $(opencv_version) --depth 1
(or "git clone https://gitee.com/mirrors/opencv.git --branch $(opencv_version) --depth 1" Chinese MainLand)

# download opencv contribution code
cd opencv/
git clone https://github.com/opencv/opencv_contrib.git --branch $(opencv_version) --depth 1
(or "git clone https://gitee.com/mirrors/opencv_contrib.git --branch $(opencv_version) --depth 1" Chinese MainLand)


mkdir build && cd build
sudo apt-get -y install libgtk2.0-dev pkg-config
# cmake configuration, enable the viz,rgbd modules, which are located in the OPENCV_EXTRA_MODULES_PATH
# please choose your install-prefix, such as kinfusion/opencv/build/install
cmake -DCMAKE_INSTALL_PREFIX="/your/absolute/path/to/kinfusion/opencv/build/install" -DBUILD_LIST=core,highgui,imgproc,calib3d,imgcodecs,cudev,rgbd,viz -DWITH_VTK=ON -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules/ -DWITH_OPENGL=ON  -DWITH_CUDA=ON -DOPENCV_ENABLE_NONFREE=ON -DWITH_GTK_2_X=ON -DBUILD_TIFF=ON -DBUILD_TESTS=OFF -DBUILD_opencv_python_tests=OFF -DOPENCV_GENERATE_PKGCONFIG=YES ..

# if you cannot succeed with one-time `make`, please execute multiple times
make -j

# install
make install
``` 
## 3. Compiling & Running

Before compiling `kinfusion`,  please modify `OpenCV_DIR` value in line 6 of [CMakeLists.txt](./CMakeLists.txt) as below:

```cmake
cd kinfusion/
# find_package(OpenCV PATHS REQUIRED)
set(OpenCV_DIR "/your/absolute/path/to/kinfusion/opencv/build/install") # line 6
find_package (OpenCV REQUIRED HINTS ${OpenCV_DIR} NO_DEFAULT_PATH)
```

If you want to compile this example separately, excute the command listed as below:

```bash
mkdir build && cd build
cmake ..
make -j
./kinfusion --record=<result_model_name> 
```

To run kinecfusion, please type commands as list below:

```bash
# run with 'help' to get usage information
./kinfusion --help

# Don't run kinectfusion, display depth frames
./kinfusion --idle

# run on coarse setting
./kinfusion --coarse

# Use the newer hashtable based TSDFVolume (relatively fast) and for larger reconstructions. (opencv 4.7)
./kinfusion --useHashTSDF

# set file name for result 3D model
./kinfusion --record=<file_name_you_want_set> # example: ./kinfusion --record=test

```

## 4. Code Overview

Below we roughly walk through the codes in the [main.cpp](./main.cpp) and explain the purposes for each of them step-by-step.

* open okulo P1 camera and open `PCL` stream for getting depth frame, then set parameters for it

    ```c++
    PDdevice devInst;
    float DistRange;
    if (!devInst)
    {
        return -1;
    }

    PDstream PCLstream = PDstream(devInst, "PCL");
    if (PCLstream)
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
    ```

* get intrinsic of Okulo P1 camera

    ```c++
    void getCameraIntrinsics(PDstream PCLstream, intrinsics &intrin)
    {
        extrinsics extrin; // the extrinsics, we don't use it ..
        PCLstream.getCamPara(intrin, extrin);
        print_intrinsics(&intrin);   // print intrinsics
    }
    ```

* declare pointers to the objects of KinFu and Params. We initialize Params with the default values, later on we'll change the relevant parameters manually.

    ```c++
    // params 
    Ptr<Params> params;
    // create kinfusion instance
    Ptr<KinFu> kf;
    if(coarse)
        params = Params::coarseParams();
    else
        params = Params::defaultParams();
    if(useHashTSDF)
        params = Params::hashTSDFParams(coarse);
    ```

* set params for kinfusion, and enable OpenCL
  
    ```c++
    // setParamsForKinfusion 
    setParamsForKinfusion(intrin, *params, undistortMap1, undistortMap2);

    // Enables OpenCL explicitly (by default can be switched-off)
    cv::setUseOptimized(true);

    // update params
    if(!idle)
        kf = KinFu::create(params);
    ```

*  set intrinsics
    ```c++   
    params.intr = Matx33f(fx, 0, cx, 0, fy, cy, 0, 0, 1);
    ```

* set frame size of depth image  
    ```c++  
    params.frameSize = Size(w, h);
    ```

* depth factor:  1000 
    ```c++  
    params.depthFactor = 1000.0f;
    ```


* set volume params ---- for TSDF

    ```c++
    float volumeSize         = 4.0f;
    params.voxelSize         = volumeSize / params.volumeDims[0];
    params.tsdf_trunc_dist   = 0.03f;
    params.truncateThreshold = 6.f;
    params.volumePose = Affine3f().translate(Vec3f(-volumeSize / 2.f, -volumeSize / 2.f, 0.05f));
    ```
* set ICP params 

    ```c++
    params.icpDistThresh         = 0.1f;
    params.bilateral_sigma_depth = 0.04f;
    ```
* init undistort rectify map 

    ```c++
    Matx<float, 1, 5> distCoeffs;
    distCoeffs(0) = k1;
    distCoeffs(1) = k2;
    distCoeffs(2) = p1;
    distCoeffs(3) = p2;
    distCoeffs(4) = k3;
    initUndistortRectifyMap(params.intr, distCoeffs, cv::noArray(), params.intr, params.frameSize, CV_16SC2, undistortMap1, undistortMap2);
    ```
*  get frame from camera

    ```c++
    auto pPclFrame = PCLstream.waitFrames();
    const cv::Mat &xyz = pPclFrame->getMat(0);
    std::vector<cv::Mat> channels(3);
    cv::split(xyz, channels);
    const cv::Mat &depth = channels[2];
    cv::Mat f;
    depth.convertTo(f, CV_16UC1, 1000); // 1000 -> depth factor
    ```

* KinectFusion is able to run on the GPU to improve performance. Since we perform depth processing and rendering on the CPU, copying frames from GPU to CPU and vice versa is required. Running on CPU alone using OpenCV's Mat instead of UMat is also possible.

    ```c++
    UMat frame(f.cols, f.rows, CV_16UC1);
    f.copyTo(frame);
    ```

* Run KinFu on the new frame(on GPU)

    ```c++
    if(!kf->update(frame))
    {
        kf->reset(); // If the algorithm failed, reset current state
        std::cout << "reset" << std::endl;
    }
    ```

* get result

    ```c++
    cv::UMat res_points;
    cv::UMat res_normals;
    kf->getCloud(res_points, res_normals);
    ```

* save result to ply file

    ```c++
    // Output the fused point cloud from KinectFusion-
    savePly(model_name + ".ply", res_points, res_normals);
    ```
## Contact
If you have any questions, please contact zouhongwei@pointspread.cn.
