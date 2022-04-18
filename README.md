# OkuloSdk

## Precondition

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

other dependencies : OpenCV ( 4.2 recommended ), CMake

#### 3. Set environment: 

```
export GENICAM_GENTL64_PATH={OkuloSdkDir}/SDKlib  ({OkuloSdkDir} should be absolute path wrt. your computer!!!)
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"{OkuloSdkDir}/SDKlib"  ({OkuloSdkDir} should be absolute path wrt. your computer!!!)
```

It's recommended to put those command in your  `~/.bashrc`, also remember to ``source ~/.bashrc``

### What SDK includes:

1. **okulo_viewer** *(SDKlib/okulo_viewer)* : show the origin camera stream & point cloud & control the stream;
2. **streamShow** *(SDKlib/streamShow)* : the example/streamShow output;
3. **getCameraPara** *(SDKlib/getCameraPara)* : acquire the calibrated parameters recorded in the camera;
4. **OkuloTest.py** *(SDKlib/OkuloTest.py)* : python example, which invoke the *SDKlib/pydyv.cpython-38-x86_64-linux-gnu.so* indeed.



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



