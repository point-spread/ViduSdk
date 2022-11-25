    

# OkuloSdk

#### 1. System Requirement

Ubuntu 18.04 or later Ubuntu LTS version x86-64bit system; For Windows system, please checkout [windows  branch](https://github.com/point-spread/OkuloSdk/tree/windows).

#### 2. Installation

Please execute the following commands in a `bash` terminal:

```bash     
git clone https://github.com/point-spread/OkuloSdk.git     
cd OkuloSdk/     
chmod +x ./install.sh     
./install.sh     
source ~/.bashrc
```

### 3. Quick Start:

The  OkuloSdk provides the following executable tools for you to have a quick taste of the [Okulo Camera](https://www.pointspread.cn/okulo-p1)'s functionality :

* **[SDKbin/okulo_viewer](https://github.com/point-spread/OkuloSdk/blob/main/SDKbin/okulo_viewer)**
  * show the origin camera stream, point cloud and control the video stream;
* **[SDKbin/streamShow](https://github.com/point-spread/OkuloSdk/blob/main/SDKbin/streamShow)**
  * show the RGB stream, ToF stream and PCL stream
* **[SDKbin/getCameraPara](https://github.com/point-spread/OkuloSdk/blob/main/SDKbin/getCameraPara)**
  * acquire the calibrated parameters recorded in the camera
* **[SDKpythonLib/okulo_test.py](https://github.com/point-spread/OkuloSdk/blob/main/SDKpythonLib/okulo_test.py)**
  * python example, which invokes the ``SDKpythonLib/pyokulo.cpython-xx-x86_64-linux-gnu.so``.

### 4. Development:

Below we briefly introduce some important folders to the developers. The comprehensive guidance on the usage of the software development for the Okulo camera is [here](http://dev.pointspread.cn:82/Okulo_Software_Developer's_Guide.pdf)

* **[SDKcode/GenTL](https://github.com/point-spread/OkuloSdk/tree/main/SDKcode/GenTL)** provides ``C`` headers by which the developer can directly invoke the ``SDKlib/libDYVGenTL.cti``  using  ``C`` interface according to the **[GenTL](https://www.emva.org/wp-content/uploads/GenICam_GenTL_1_5.pdf)** standard.
* **[SDKcode/wrapper](https://github.com/point-spread/OkuloSdk/tree/main/SDKcode/wrapper)** provides a ``C++`` wrapper of the ``SDKlib/libDYVGenTL.cti``, which makes the coding with okulo camera more conciser and ``C++`` tone.
* **[SDKcode/common](https://github.com/point-spread/OkuloSdk/tree/main/SDKcode/common)** provides some debug or timing code fragment.
* **[example](https://github.com/point-spread/OkuloSdk/tree/main/example)** provides examples on how to get data from the camera. Please execute the following commands to compile those examples on your machine:

  ```
  cd OkuloSdk/
  mkdir build && cd build
  cmake ../
  make -j
  ./example/streamShow/streamShow
  ```
  