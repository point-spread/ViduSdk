# OkuloSdk

## Precondition
#### 0. System:
Ubuntu16.04 or later ubuntu LTS version (Tested on ubuntu 16.04, 18.04, 20.04)
x86-64bit system
#### 1. Install cam drivers:

execute the following commands in a terminal

```
#under OkuloSdk
cd camdriver
sudo chmod +x installDriver.sh
sudo ./installDriver.sh
```

#### 2. Install SDK dependencies: 

```
sudo apt install libglfw3-dev libglm-dev
```

other dependencies : 
CMake (minimum version 3.5)

#### 3. Set environment: 

```
export DYVCAM_GENTL64_PATH={OkuloSdkDir}/SDKlib  ({OkuloSdkDir} should be absolute path wrt. your computer!!!)
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"{OkuloSdkDir}/SDKlib"  ({OkuloSdkDir} should be absolute path wrt. your computer!!!)
```

It's recommended to put those commands in your  `~/.bashrc`, also remember to ``source ~/.bashrc``

### What SDK includes:

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



