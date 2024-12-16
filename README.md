

# ViduSdk

### 1. System Requirement

Ubuntu 20.04 or later Ubuntu LTS version x86-64bit system; For Windows system, please checkout [windows  branch](https://github.com/point-spread/ViduSdk/tree/windows).

### 2. Installation

You could install ViduSdk using package manager, execute `sudo dpkg -i Vidu_SDK_Api_x_x(_x)_amd64.deb` or `sudo apt install ./Vidu_SDK_Api_x_x(_x)_amd64.deb`.  
When you want to uninstall ViduSdk package, just execute `sudo dpkg -r vidusdkapi` or `sudo apt remove vidusdkapi`.  

If you want to obtain accurate metadata, linux driver patch is required, please execute the following commands:  
**Before you execute, you need to disconnect the connected camera.**
```
./camdriver/patch-okulo-ubuntu-lts-hwe.sh
```
PS: If your system has been updated, it needs to be patched again.

### 3. Start

The ViduSdk provides the following executable tools for you to have a quick taste of the functionality of [Okulo P1 Camera](https://www.pointspread.cn/okulo-p1) and [Okulo C1 Camera](https://www.pointspread.cn/okulo-c1):

Then you could execute `vidu_viewer` or click Ubuntu Applications `Vidu viewer` to run vidu_viewer
* **[vidu_viewer](vidu_viewer)**
  * show the origin camera stream, point cloud and control the video stream;

Run python demo, you could execute `python3 python/vidu_test.py`
* **[python/vidu_test.py](./python/vidu_test.py)**
  * python example, which invokes the ``SDKpythonLib/pyvidu.cpython-xx-x86_64-linux-gnu.so``.

### 4. Development

Below we briefly introduce some important folders to the developers. The comprehensive guidance on the usage of the software development for the Okulo camera is [here](https://fv9fikyv7kp.feishu.cn/drive/folder/K4KSfXzwUl5FnIdXq1Dca5IFnWo)

* **[Vidu_SDK_Api/SDKcode/GenTL](./Vidu_SDK_Api/SDKcode/GenTL)** provides ``C`` headers by which the developer can directly invoke the ``Vidu_SDK_Api/SDKlib/libDYVGenTL.cti``  using  ``C`` interface according to the **[GenTL](https://www.emva.org/wp-content/uploads/GenICam_GenTL_1_5.pdf)** standard.
* **[Vidu_SDK_Api/SDKcode/GenTLwrapper](./Vidu_SDK_Api/SDKcode/GenTLwrapper)** provides a ``C++`` wrapper of the ``Vidu_SDK_Api/SDKlib/libDYVGenTL.cti``, which makes the coding with okulo camera more conciser and ``C++`` tone.
* **[Vidu_SDK_Api/SDKcode/common](./Vidu_SDK_Api/SDKcode/common)** provides some debug or timing code fragment.
* **[example](./example)** provides examples on how to get data from the camera. Please execute the following commands to compile those examples on your machine:

You can build example in root directory of SDK package(need opencv-dev), you can install by `sudo apt-get -y install libopencv-dev python3-opencv`.

  ```bash
  cd ViduSdk/
  mkdir build && cd build
  cmake ../
  make -j
  ./example/streamShow/streamShow
  ```

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
