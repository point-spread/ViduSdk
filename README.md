# OkuloSdk
**WINDOWS VERSION**, if linux, pls. checkout main branch
## Precondition
#### 0. System:
Windows10 x86-64bit system
#### 1. Install
* extract camdriver\FTD3XXDriver_WHQLCertified_v1.3.0.4_Installer.exe.zip
and then execute the install program
* double click `install.bat`
###  What SDK includes:

1. **okulo_viewer** *(SDKbin/okulo_viewer)* : show the origin camera stream & point cloud & control the stream;
2. **streamShow** *(SDKbin/streamShow)* : the example/streamShow output; press "c" save image or pcl, press "q" exit;
3. **getCameraPara** *(SDKbin/getCameraPara)* : acquire the calibrated parameters recorded in the camera;
4. **OkuloTest.py** *(SDKpythonLib/OkuloTest.py)* : python example, which invoke the *pyokulo.cpxx-win_amd64.pyd indeed.
## Develop:

**SDKcode/GenTL** provides C headers by which the developer can directly invoke the *SDKlib/libDYVGenTL.cti*  using  C interface according to the **GenTL** standard.

**SDKcode/wrapper** provides a C++ wrapper of the *SDKlib/libDYVGenTL.cti*, which may let the code of developer more conciser and C++ tone.

**SDKcode/common** provides some debug or timing code fragment.

**example** example of how to get data from the camera.

### How to Build Example:
#### Build tools:
It's recommended to use MSVC C++ compiler.You may need:
* Install the `CMake` https://github.com/Kitware/CMake/releases/download/v3.23.2/cmake-3.23.2-windows-x86_64.msi
* Install the Microsoft Visual C++ (MSVC) compiler toolset https://aka.ms/vs/17/release/vs_BuildTools.exe. Select the Desktop development with C++ workload.
#### Opencv:
* Download opencv on sourceforge:
https://udomain.dl.sourceforge.net/project/opencvlibrary/4.5.5/opencv-4.5.5-vc14_vc15.exe Double click to extract the file.
* Set environment variable `OpenCV_DIR` pointing to the directory : `opencv/build` (which contains OpenCVConfig.cmake).
* Add `opencv/build/x64/vc14/bin` to environment variable `path` (which contains opencv_world455.dll, vc14 can be set as vc15 according your system).
#### build:
```
#under OkuloSdk
mkdir build
cd build
cmake ../
cmake --build . --config Release -j 
```



