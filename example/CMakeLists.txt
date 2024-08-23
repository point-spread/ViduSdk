cmake_minimum_required(VERSION 3.5)
project(OKULO_SDK_EXAMPLE VERSION 0.0)

find_package(OpenCV REQUIRED)
find_package(Vidu_SDK_Api REQUIRED)

add_subdirectory(getCamPara)
add_subdirectory(streamShow)
add_subdirectory(multiDevice)
add_subdirectory(align)
add_subdirectory(algorithm)
