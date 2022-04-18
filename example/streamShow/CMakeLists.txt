cmake_minimum_required(VERSION 3.1.0)
project(streamShow)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIR} ${PROJECT_SOURCE_DIR} ${OpenCV_INCLUDE_DIRS}  ${CMAKE_CURRENT_SOURCE_DIR}/../../SDKcode/common
${CMAKE_CURRENT_SOURCE_DIR}/../../SDKcode/GenTLwrapper 
${CMAKE_CURRENT_SOURCE_DIR}/../../SDKcode/GenTL)
file(GLOB_RECURSE SRC *.cpp)
add_executable(${PROJECT_NAME} ${SRC})

link_directories(${PROGRAM_LIB_DIR})

message(${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME}  ${OpenCV_LIBS}   Threads::Threads  ${CMAKE_CURRENT_SOURCE_DIR}/../../SDKlib/libDYVGenTL.cti  GenTLwrapper)

