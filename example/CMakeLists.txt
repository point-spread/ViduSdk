cmake_minimum_required(VERSION 3.5)
project(OCULO_SDK_EXAMPLE VERSION 0.0)

find_package(OpenCV REQUIRED)
find_package(Vidu_SDK_Api REQUIRED HINTS $ENV{DYVCAM_GENTL64_PATH}/../cmake)

add_subdirectory(getCamPara)
add_subdirectory(streamShow)
add_subdirectory(multiDevice)