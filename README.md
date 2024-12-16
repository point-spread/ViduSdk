# ViduSdk

### 1. System Requirement

Windows 10 x86-64bit system; For the Ubuntu system, please checkout [main branch](https://github.com/point-spread/ViduSdk).  
PS: Camera privacy permission needs to be opened: Setting > Privacy and Security > Application permissions > Camera

### 2. Installation

#### 1) SDK Installation:

* Double click `Vidu_SDK_Api_x_x(_x)_amd64.exe`, Select the installation directory and click 'Install' to proceed with the installation.
* Follow the pop-up prompt to install the dependent driver again.
* When you want to uninstall, simply delete the installation directory.

#### 2) Metadata Info:

* If you want to obtain accurate metadata, change registry is required.
* Run power shell with administrator privileges, execute `.\camdriver\okulo_c1_metadata_win.ps1`.
* If `SecurityError` occurs during execution, you can execute `Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process` to temporary change execution strategy.
* **Before you execute, you need to connect the camera that needs to modify the registry**.
```
#PS> .\camdriver\okulo_c1_metadata_win.ps1                    - Add metadata reg. keys for the connected Okulo C1 devices
#PS> .\camdriver\okulo_c1_metadata_win.ps1 -op install        - See above
#PS> .\camdriver\okulo_c1_metadata_win.ps1 -op install_all    - Add metadata reg. keys for all Okulo C1 devices that were previously connected
#PS> .\camdriver\okulo_c1_metadata_win.ps1 -op remove         - Remove metadata reg. keys for the connected Okulo C1 devices
#PS> .\camdriver\okulo_c1_metadata_win.ps1 -op remove_all     - Remove metadata reg. keys for all Okulo C1 devices that were previously connected
```

### 3. Start

The ViduSdk provides the following executable tools for you to have a quick taste of the functionality of [Okulo P1 Camera](https://www.pointspread.cn/okulo-p1) and [Okulo C1 Camera](https://www.pointspread.cn/okulo-c1):

You can double-click the vidu-viewer desktop icon to run the program.
* **[vidu_viewer](vidu_viewer.exe)**
  * show the origin camera stream, point cloud and control the video stream;

PS: **If get "Search for app in the Store?" while executing bat or exe, you can right-click > properties > general > security > Choose to unlock solve it**

Run python demo, you could execute `python3 python/vidu_test.py` on command which contain your python environment.
* **[python/vidu_test.py](./python/vidu_test.py)**
  * python example, which invokes the ``SDKpythonLib/pyvidu.cpxx-win_amd64.pyd``.

### 4. Development

Below we briefly introduce the important folders to the developers. For comprehensive guidance on the software development, please check out our [software developer's guide](https://fv9fikyv7kp.feishu.cn/drive/folder/K4KSfXzwUl5FnIdXq1Dca5IFnWo).

* **[Vidu_SDK_Api/SDKcode/GenTL](./Vidu_SDK_Api/SDKcode/GenTL)** provides ``C`` headers of the ``SDKlib/libDYVGenTL.cti`` lib that follows the **[GenTL](https://www.emva.org/wp-content/uploads/GenICam_GenTL_1_5.pdf)** standard;
* **[Vidu_SDK_Api/SDKcode/GenTLwrapper](./Vidu_SDK_Api/SDKcode/GenTLwrapper)** provides a ``C++`` wrapper of the ``SDKlib/libDYVGenTL.cti``;
* **[Vidu_SDK_Api/SDKcode/common](./Vidu_SDK_Api/SDKcode/common)** provides some debug or timing code fragments;
* **[example](./example)** provides examples of how to get data from the camera. Please see below to compile those examples on your local machine.

### 5. Building Examples Locally

```bash
cd ViduSdk/
mkdir build
cd build
cmake -A x64 ../
cmake --build . --config Release -j 
```

It's recommended to use MSVC C++ compiler. You need to:
- Install [CMake](https://github.com/Kitware/CMake/releases/download/v3.23.2/cmake-3.23.2-windows-x86_64.msi);
- Install the [Microsoft Visual C++ (MSVC) compiler toolset](https://aka.ms/vs/17/release/vs_BuildTools.exe). Select the Desktop development with C++ workload.

Also need opencv-dev:
* Download [opencv on sourceforge](https://udomain.dl.sourceforge.net/project/opencvlibrary/4.5.5/opencv-4.5.5-vc14_vc15.exe). Double-click it to extract the file.
* Set environment variable `OpenCV_DIR` pointing to the directory : `opencv/build` (which contains `OpenCVConfig.cmake`).
* Add `opencv/build/x64/vc14/bin` to the environment variable `path` , which also contains `opencv_world455.dll`, vc14 can be set as vc15 according to your system.

Also you can build your code or move the example outside of the package by adding

```
find_package(OpenCV REQUIRED)
find_package(Vidu_SDK_Api REQUIRED)
```

and

```
target_link_libraries(${PROJECT_NAME} Vidu_SDK_Api::GenTLwrapper)
```

in the CMakeLists.txt file.
