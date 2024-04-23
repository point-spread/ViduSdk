

# ViduSdk

### 1. System Requirement

Ubuntu 20.04 or later Ubuntu LTS version x86-64bit system; For Windows system, please checkout [windows  branch](https://github.com/point-spread/ViduSdk/tree/windows).

### 2. Quick Start

You could execute `bash camdriver/installDriver.sh` and `bash set_env.sh` to set environment variable, no installation required to open:
* **[SDKbin/vidu_viewer](./SDKbin/vidu_viewer)**
  * show the origin camera stream, point cloud and control the video stream;

### 3. Installation

If there is no opencv installed, please
```bash
sudo apt-get -y install libopencv-dev python3-opencv
```

Please execute the following commands in a `bash` terminal, **<font color=red>and do not use</font> `sh` <font color=red>execute</font> `install.sh`**:

```bash
git clone https://github.com/point-spread/ViduSdk.git
cd ViduSdk/
chmod +x ./install.sh
./install.sh
source ~/.bashrc
```

**The following actions have been merged into the `set_env.sh` and `install.sh` script**

If you want to obtain accurate metadata, linux driver patch is required, please execute the following commands:  
**Before you execute, you need to disconnect the connected camera.**
```
./camdriver/patch-okulo-ubuntu-lts-hwe.sh
```
PS: If your system has been updated, it needs to be patched again.

If you need to open the hid device to obtain imu data, please execute the following commands:
```bash
sudo cp camdriver/53-psf-camera.rules /etc/udev/rules.d/ 
sudo udevadm control --reload-rules && sudo udevadm trigger 
```

### 4. Start

The ViduSdk provides the following executable tools for you to have a quick taste of the functionality of [Okulo P1 Camera](https://www.pointspread.cn/okulo-p1) and [Okulo C1 Camera](https://www.pointspread.cn/okulo-c1):

* **[SDKbin/vidu_viewer](./SDKbin/vidu_viewer)**
  * show the origin camera stream, point cloud and control the video stream;
* **[python/vidu_test.py](./python/vidu_test.py)**
  * python example, which invokes the ``SDKpythonLib/pyvidu.cpython-xx-x86_64-linux-gnu.so``.

### 5. Development

Below we briefly introduce some important folders to the developers. The comprehensive guidance on the usage of the software development for the Okulo camera is [here](https://fv9fikyv7kp.feishu.cn/drive/folder/K4KSfXzwUl5FnIdXq1Dca5IFnWo)

* **[SDKcode/GenTL](./SDKcode/GenTL)** provides ``C`` headers by which the developer can directly invoke the ``SDKlib/libDYVGenTL.cti``  using  ``C`` interface according to the **[GenTL](https://www.emva.org/wp-content/uploads/GenICam_GenTL_1_5.pdf)** standard.
* **[SDKcode/GenTLwrapper](./SDKcode/GenTLwrapper)** provides a ``C++`` wrapper of the ``SDKlib/libDYVGenTL.cti``, which makes the coding with okulo camera more conciser and ``C++`` tone.
* **[SDKcode/common](./SDKcode/common)** provides some debug or timing code fragment.
* **[example](./example)** provides examples on how to get data from the camera. Please execute the following commands to compile those examples on your machine:

You can build example in root directory of SDK package

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
find_package(Vidu_SDK_Api REQUIRED HINTS $ENV{DYVCAM_GENTL64_PATH}/../cmake)
```
and

```
target_link_libraries(${PROJECT_NAME} Vidu_SDK_Api::GenTLwrapper ${OpenCV_LIBS})
```

in the CMakeLists.txt file.
