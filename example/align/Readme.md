# ToF and RGB image align demo

## 1. Install Dependencies

### 1.1 OpenCV installation

* ubuntu:

    If there is no opencv installed, please 

    ```
    sudo apt-get -y install libopencv-dev python3-opencv
    ```

* windows:

    * Download opencv on sourceforge. Double-click it to extract the file.
    * Set environment variable OpenCV_DIR pointing to the directory : opencv/build (which contains OpenCVConfig.cmake).
    * Add opencv/build/x64/vc14/bin to the environment variable path , which also contains opencv_world455.dll, vc14 can be set as vc15 according to your system.

### 1.2 ViduSDK installation

For detailed steps, please refer to **[Readme](../../README.md)**.

## 2. Build Align

* ubuntu:

    ```
    mkdir build
    cd build
    cmake ..
    make -j
    ```

* windows:

    ```
    mkdir build
    cd build
    cmake ..
    cmake --build . --config Release -j
    ```

## 3. Use Align

* ubuntu:

    ```
    ./align -h
    ```

* windows:

    ```
    ./Release/align -h
    ```

You can get:
```
Usage: align [--help] [--rgb_image path] [--tof_image path] [--intrinsics path] [--extrinsics path] [--tof_max_value value], ps: path requires absolute path

Optional arguments:
  -h, --help            shows help message and exits
  -r, --rgb_image       the rgb image file path
  -t, --tof_image       the tof image file path
  -i, --intrinsics      the intrinsics of camera
  -e, --extrinsics      the extrinsics of camera
  -m, --tof_max_value   the max distance or depth value of tof image
```

The current directory provides some files for reference and use. Depth image captured in 5m distance, so you can use parameters `-m 5.0`.

```
├── extrinsics.json
├── image
│   ├── infrared_image.pgm
│   ├── pha_image.pgm
│   └── rgb_image.png
├── intrinsics.json
```
