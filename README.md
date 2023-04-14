

# ViduSdk

### 1. System Requirement

Ubuntu 20.04 or later Ubuntu LTS version x86-64bit system; For Windows system, please checkout [windows  branch](https://github.com/point-spread/ViduSdk/tree/windows).

### 2. Installation

Please execute the following commands in a `bash` terminal:

```bash
git clone https://github.com/point-spread/ViduSdk.git
cd ViduSdk/
chmod +x ./install.sh
./install.sh
source ~/.bashrc
```

### 3. Quick Start

The ViduSdk provides the following executable tools for you to have a quick taste of the [Okulo Camera](https://www.pointspread.cn/okulo-p1)'s functionality :

* **[SDKbin/vidu_viewer](./SDKbin/vidu_viewer)**
  * show the origin camera stream, point cloud and control the video stream;
* **[SDKpythonLib/vidu_test.py](./SDKpythonLib/vidu_test.py)**
  * python example, which invokes the ``SDKpythonLib/pyvidu.cpython-xx-x86_64-linux-gnu.so``.

### 4. Development

Below we briefly introduce some important folders to the developers. The comprehensive guidance on the usage of the software development for the Okulo camera is [here](http://dev.pointspread.cn:82/Okulo_Software_Developer's_Guide.pdf)

* **[SDKcode/GenTL](./SDKcode/GenTL)** provides ``C`` headers by which the developer can directly invoke the ``SDKlib/libDYVGenTL.cti``  using  ``C`` interface according to the **[GenTL](https://www.emva.org/wp-content/uploads/GenICam_GenTL_1_5.pdf)** standard.
* **[SDKcode/wrapper](./SDKcode/wrapper)** provides a ``C++`` wrapper of the ``SDKlib/libDYVGenTL.cti``, which makes the coding with okulo camera more conciser and ``C++`` tone.
* **[SDKcode/common](./SDKcode/common)** provides some debug or timing code fragment.
* **[example](./example)** provides examples on how to get data from the camera. Please execute the following commands to compile those examples on your machine:


If there is no opencv installed, please
```bash
sudo apt-get -y install libopencv-dev python3-opencv
```
Then you can build example in root directory of SDK package

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
