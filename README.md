# OkuloSdk

#### 1. System Requirement

Windows10 x86-64bit system; For Ubuntu system, please checkout [main branch](https://github.com/point-spread/OkuloSdk).

#### 2. Installation

* Extract `camdriver/FTD3XXDriver_WHQLCertified_v1.3.0.4_Installer.exe.zip`
  and then execute the program `FTD3XXDriver_WHQLCertified_v1.3.0.4_Installer.exe`
* Double click `./install.bat`

### 3. Quick Start:

* **[SDKbin/okulo_viewer](https://github.com/point-spread/OkuloSdk/blob/windows/SDKbin/okulo_viewer.exe)**
  * show the origin camera stream, point cloud and control the video stream;
* **[SDKbin/streamShow](https://github.com/point-spread/OkuloSdk/blob/windows/SDKbin/streamShow.exe)**
  * show the RGB stream, ToF stream and PCL stream
* **[SDKbin/getCameraPara](https://github.com/point-spread/OkuloSdk/blob/windows/SDKbin/getCameraPara.exe)**
  * acquire the calibrated parameters recorded in the camera
* **[SDKpythonLib/okulo_test.py](https://github.com/point-spread/OkuloSdk/blob/windows/SDKpythonLib/okulo_test.py)**
  * python example, which invokes the ``SDKpythonLib/pyokulo.cpxx-win_amd64.pyd``.

### 4. Development:

Below we briefly introduce some important folders to the developers. The comprehensive guidance on the usage of software development for the Okulo camera is [here](http://dev.pointspread.cn:82/Okulo_Software_Developer's_Guide.pdf).

* **[SDKcode/GenTL](https://github.com/point-spread/OkuloSdk/tree/windows/SDKcode/GenTL)** provides ``C`` headers by which the developer can directly invoke the ``SDKlib/libDYVGenTL.cti``  using  ``C`` interface according to the **[GenTL](https://www.emva.org/wp-content/uploads/GenICam_GenTL_1_5.pdf)** standard.
* **[SDKcode/wrapper](https://github.com/point-spread/OkuloSdk/tree/windows/SDKcode/wrapper)** provides a ``C++`` wrapper of the ``SDKlib/libDYVGenTL.cti``, which makes the coding with okulo camera more conciser and ``C++`` tone.
* **[SDKcode/common](https://github.com/point-spread/OkuloSdk/tree/windows/SDKcode/common)** provides some debug or timing code fragment.
* **[example](https://github.com/point-spread/OkuloSdk/tree/windows/example)** provides examples on how to get data from the camera. Please execute the following commands to compile those examples on your machine:

### 5. Building Examples Locally:

#### 1) Building tools setup:

    It's recommended to use MSVC C++ compiler. You need to:

- Install [CMake](https://github.com/Kitware/CMake/releases/download/v3.23.2/cmake-3.23.2-windows-x86_64.msi);
- Install the [Microsoft Visual C++ (MSVC) compiler toolset](https://aka.ms/vs/17/release/vs_BuildTools.exe). Select the Desktop development with C++ workload.

#### 2) Opencv Installation:

* Download [opencv on sourceforge](https://udomain.dl.sourceforge.net/project/opencvlibrary/4.5.5/opencv-4.5.5-vc14_vc15.exe). Double-click it to extract the file.
* Set environment variable `OpenCV_DIR` pointing to the directory : `opencv/build` (which contains `OpenCVConfig.cmake`).
* Add `opencv/build/x64/vc14/bin` to environment variable `path` (which contains opencv_world455.dll, vc14 can be set as vc15 according your system).

#### 3) Build examples:

```
cd OkuloSdk/
mkdir build
cd build
cmake ../
cmake --build . --config Release -j 
```
