project(kinfusion)
cmake_minimum_required(VERSION 3.10)

# opencv
# find_package(OpenCV PATHS REQUIRED)
set(OpenCV_DIR "${CMAKE_CURRENT_LIST_DIR}/opencv/build/install")
find_package(OpenCV REQUIRED HINTS ${OpenCV_DIR} NO_DEFAULT_PATH)
find_package(Vidu_SDK_Api REQUIRED HINTS $ENV{DYVCAM_GENTL64_PATH}/../cmake)

include_directories(
    ${DYV_SDK_INCLUDE}
${OpenCV_INCLUDE_DIRS})

link_directories(${DYV_SDK_LIB})
add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} Vidu_SDK_Api::GenTLwrapper ${OpenCV_LIBS})
