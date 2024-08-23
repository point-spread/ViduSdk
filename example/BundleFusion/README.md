# BundleFusion

[BundleFusion](https://graphics.stanford.edu/projects/bundlefusion/) is a GPU-based real-time 3D reconstruction method. 
In this doc, we will introduce how to use BundleFusion to do 3D reconstruction with data sampled by [Okulo](https://www.pointspread.cn/okulo-p1) camera.

## 1. Check GPU availability and NVCC version
Please use the following command to check if you have an NVIDIA GPU card installed on your computer. If not, the BundleFusion cannot be working well.
Also, `nvcc` command is used to check if NVIDIA's `cuda` toolkit is installed. 
```bash
lspci | grep NVIDIA
nvcc -V
```


## 2. Install Dependencies

The BundleFusion has been tested under Ubuntu16.04/GCC7/CUDA10.1 (GPU: RTX2060) and Ubuntu20.04/GCC8/CUDA10.1 (GPU:1080 Ti).

Requirements:

- CMake(> 3.0)
- Eigen 3.1.0
- NVIDIA CUDA 9.0/10.+
- OpenCV
- Pangolin (optional for VISUALIZATION)

### 1) OpenCV installation
As this project need OpenCV with CUDA support, users have to install OpenCV from source code. Detailed steps are listed below:

```bash
cd BundleFusion

# download opencv source code. The command below will make sure that the version of the cloned opencv repo is same as the version you installed before.
git clone https://github.com/opencv/opencv.git --branch $(opencv_version) --depth 1
(or "git clone https://gitee.com/mirrors/opencv.git --branch $(opencv_version) --depth 1" Chinese MainLand)

# download opencv contribution code
cd opencv/
git clone https://github.com/opencv/opencv_contrib.git --branch $(opencv_version) --depth 1
(or "git clone https://gitee.com/mirrors/opencv_contrib.git --branch $(opencv_version) --depth 1" Chinese MainLand)

# for ubuntu20.04/GCC8/CUDA10.1 (GPU:1080 Ti), please use gcc 8 by: 
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 8
gcc --version


mkdir build/ && cd build
# cmake configuration
# please choose your install-prefix, such as BundleFusion/opencv/build/install
cmake --install-prefix="/your/absolute/path/to/BundleFusion/opencv/build/install" -DBUILD_LIST=core,highgui,imgproc,calib3d,imgcodecs,cudev -DWITH_VTK=ON -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules/ -DWITH_OPENGL=ON  -DWITH_CUDA=ON -DOPENCV_ENABLE_NONFREE=ON -DWITH_GTK_2_X=ON -DBUILD_TIFF=ON -DBUILD_TESTS=OFF -DBUILD_opencv_python_tests=OFF -DOPENCV_GENERATE_PKGCONFIG=YES ..

# if you cannot succeed with one-time `make`, please execute multiple times
make -j

# install
make install
```

> PS: If you get `error: parameter packs not expanded with ‘...’:`, [this](https://github.com/NVIDIA/nccl/issues/650) may be helpful.

### 2) Pangoline Installation for -DVISUALIZATION=ON
Please skip Pangolin installation if you don't need visualization.

```bash
cd BundleFusion/
git clone https://github.com/stevenlovegrove/Pangolin.git
cd Pangolin
./scripts/install_prerequisites.sh recommended
git submodule init
git submodule update
mkdir build && cd build
cmake ..
cmake --build . -j
sudo make install
cd ../
```

## 3. Download BundleFusion

Please run the commands below in a terminal to download BundleFusion:

```bash
cd BundleFusion/
git clone https://github.com/FangGet/BundleFusion_Ubuntu_Pangolin
cd BundleFusion_Ubuntu_Pangolin
```
## 4. Build BundleFusion
Before build `BundleFusion`,  please modify `OpenCV` path in line 19 of `BundleFusion_Ubuntu_Pangolin`'s [CMakeLists.txt](./BundleFusion_Ubuntu_Pangolin/CMakeLists.txt) as below:

```cmake
# find_package (OpenCV REQUIRED) # comment line 19
set(OpenCV_DIR "/your/absolute/path/to/BundleFusion/opencv/build/install")
find_package (OpenCV REQUIRED HINTS ${OpenCV_DIR} NO_DEFAULT_PATH)
```

Then you can make project with:

```bash
cd BundleFusion/BundleFusion_Ubuntu_Pangolin
mkdir build && cd build
cmake -DVISUALIZATION=OFF ..
make -j8
```

Here uses `-DVISUALIZATION=OFF/ON` to switch visualization plug. If you do not use Pangolin, please set `-DVISUALIZATION=OFF`.

> PS: If you get
> * `error: parameter packs not expanded with ‘...’:`, [this](https://github.com/NVIDIA/nccl/issues/650) may be helpful.  
> Or
> * `error: ‘glewInit’ was not declared in this scope`, run `sudo cp pangolin_fix.h /usr/local/include/pangolin/gl/glplatform.h` on Pangolin path may be helpful.  

## 5. Use BundleFusion

### 1) Run BundleFusion with data captured by Okulo camera

* In OkuloSdk, use [streamShow](../example/streamShow) to capture a sequence of images, type `c` to save a bunch of frames at a time.
 
* Copy [bundle_fusion_okulo.cpp](./bundle_fusion_okulo.cpp) to BundleFusion project and add executable target by the following commands.
    ```bash
    cd BundleFusion/
    cp bundle_fusion_okulo.cpp BundleFusion_Ubuntu_Pangolin/example/
    echo -e "add_executable(bundle_fusion_okulo example/bundle_fusion_okulo.cpp)
    target_link_libraries(bundle_fusion_okulo BundleFusion)" >> BundleFusion_Ubuntu_Pangolin/CMakeLists.txt
    ```

* Modify depth scale factor in [PrimeSenseSensor.cpp](./BundleFusion_Ubuntu_Pangolin/src/PrimeSenseSensor.cpp) (line 97). The default `YOUR_DEPTH_SCALE` is `0.00011444266f`, you can leave it unchanged for your first usage. Note that ``streamShow`` application sets DistRange to `7.5m` by default, thus the YOUR_DEPTH_SCALE = 7.5/(2**16-1)= 0.00011444266f. If you modify streamShow to another depth range, you should change YOUR_DEPTH_SCALE accrodingly.

  ```c++
  float dF = (float)p * 0.00011444266f;
  float dF = ( float ) p * YOUR_DEPTH_SCALE;
  ```

* Modify [ZparametersDefault.txt](./BundleFusion_Ubuntu_Pangolin/zParametersDefault.txt) to configure camera intrincs and algorithm params. The camera intrinsics **MUST** be modified according to your Okulo camera's at hand, you can run [getCamParam](https://github.com/point-spread/OkuloSdk/tree/main/SDKbin) command provided by OkuloSdk repo to get camera intrincs.

  ```c++
  // line 118-121: camera intrincs
  s_cameraIntrinsicFx = 530;
  s_cameraIntrinsicFy = 530;
  s_cameraIntrinsicCx = 314;
  s_cameraIntrinsicCy = 245;
  ````

    The other parameters should be chosen according to you data and GPU memory available.

  ```c++
  // params for algorithm, depends on your data and GPU memory avaiblable 
  // line 33-36: depth range
  s_sensorDepthMax = 2.5f;	//maximum sensor depth in meter
  s_sensorDepthMin = 0.20f;	//minimum sensor depth in meter
  s_renderDepthMax = 2.5f;	//maximum render depth in meter
  s_renderDepthMin = 0.20f;	//minimum render depth in meter
  
  // line 39-50: sdf and hash settings
  s_SDFVoxelSize = 0.002f;//0.010f;		//voxel size in meter (IMPORTANT: reduces to improve perf.)
  s_SDFMarchingCubeThreshFactor = 5.0f;	//marching cube thresh: s_SDFMarchingCubeThresh = s_SDFMarchingCubeThreshFactor*s_SDFVoxelSize
  s_SDFTruncation = 0.020f;				//truncation in meter
  s_SDFTruncationScale = 0.02f;			//truncation scale in meter per meter
  s_SDFMaxIntegrationDistance = 2.5f;		//maximum integration in meter
  s_SDFIntegrationWeightSample = 1;		//weight for an integrated depth value
  s_SDFIntegrationWeightMax = 99999999;	//maximum integration weight for a voxel
  // s_SDFBlockSize is pound defined (SDF_BLOCK_SIZE)
  // s_hashBucketSize is pound defined (HASH_BUCKET_SIZE)
  s_hashNumBuckets = 4000000;				//smaller voxels require more space
  s_hashNumSDFBlocks = 800000;//50000;//100000;	//smaller voxels require more space
  s_hashMaxCollisionLinkedListSize = 7;
  ```

* Re-build the BundleFusion project:

  ```bash
  cd BundleFusion/BundleFusion_Ubuntu_Pangolin/
  cd build
  cmake ..
  make -j
  ```
  
* Run:

  ```bash
  ./bundle_fusion_okulo ../zParametersDefault.txt ../zParametersBundlingDefault.txt /PATH/TO/dataset
  ```

The `result.ply` will be generated beside the `./bundle_fusion_okulo `, you can use MeshLab tool to open `result.ply` to check 3D reconstruction results.


### 2) Run BundleFusion with BundleFusion's official dataset

* Download datasets from BundleFusion project mainpage [http://graphics.stanford.edu/projects/bundlefusion/](http://graphics.stanford.edu/projects/bundlefusion/) and unzip it. Below we use the [`office2`](http://graphics.stanford.edu/projects/bundlefusion/data/office2/office2.zip) dataset to make an example.
* Run Commands:

    ```bash
    cd build
    ./bundle_fusion_example ../zParametersDefault.txt ../zParametersBundlingDefault.txt /PATH/TO/dataset/office2
    ```

    A pangolin window will show up and get real time reconstruction  result.

* Save Mesh:

    we provide save mesh button at pangoln GUI, you need to specify the save path at zParametersDefault.txt for item `s_generateMeshDir`.

### 3) Issues

* Pangolin OpenGL error:

    <b>Problem:</b>

    ```bash
    /usr/local/include/pangolin/gl/glsl.h:709:70: error: ‘glUniformMatrix3dv’ was not declared in this scope
        glUniformMatrix3dv( GetUniformHandle(name), 1, GL_FALSE, m.data());
                                                                        ^
    /usr/local/include/pangolin/gl/glsl.h: In member function ‘void pangolin::GlSlProgram::SetUniform(const string&, const Matrix4d&)’:
    /usr/local/include/pangolin/gl/glsl.h:713:70: error: ‘glUniformMatrix4dv’ was not declared in this scope
        glUniformMatrix4dv( GetUniformHandle(name), 1, GL_FALSE, m.data());
    ```

    <b>Solution:</b>

    ```
    sudo vim /usr/local/include/pangolin/gl/glplatform.h
    #goto line#58
    #replace "GL/glew.h" with "/usr/include/GL/glew.h"
    ```



**## Contact** zouhongwei@pointspread.cn

