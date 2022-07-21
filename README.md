# OkuloSdk


[![Okulo P1](https://res.cloudinary.com/marcomontalbano/image/upload/v1658410834/video_to_markdown/images/youtube--t8YRnP8hS3M-c05b58ac6eb4c4700831b2b3070cd403.jpg)](https://www.youtube.com/watch?v=t8YRnP8hS3M "Okulo P1")


## Precondition
#### 0. System:
Ubuntu18.04 or later ubuntu LTS version (Tested on ubuntu 18.04, 20.04) x86-64bit system
#### 1. Install

execute the following commands in a terminal

```
#under OkuloSdk
chmod +x ./install.sh 
sudo ./install.sh 
source ~/.bashrc
```
###  What SDK includes:

1. **okulo_viewer** *(SDKbin/okulo_viewer)* : show the origin camera stream & point cloud & control the stream;
2. **streamShow** *(SDKbin/streamShow)* : the example/streamShow output; press "c" save image or pcl, press "q" exit;
3. **getCameraPara** *(SDKbin/getCameraPara)* : acquire the calibrated parameters recorded in the camera;
4. **OkuloTest.py** *(SDKpythonLib/OkuloTest.py)* : python example, which invoke the *SDKpythonLib/pydyv.cpython-xx-x86_64-linux-gnu.so* indeed.


## Develop:

**SDKcode/GenTL** provides C headers by which the developer can directly invoke the *SDKlib/libDYVGenTL.cti*  using  C interface according to the **GenTL** standard.

**SDKcode/wrapper** provides a C++ wrapper of the *SDKlib/libDYVGenTL.cti*, which may let the code of developer more conciser and C++ tone.

**SDKcode/common** provides some debug or timing code fragment.

**example** example of how to get data from the camera.

```
#under OkuloSdk
mkdir build && cd build
cmake ../
make -j
./example/streamShow/
```



