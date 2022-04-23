cmake_minimum_required(VERSION 3.5)
project(streamShow)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

include_directories(${PROJECT_SOURCE_DIR} 
${CMAKE_CURRENT_SOURCE_DIR}/../../SDKcode/common
${CMAKE_CURRENT_SOURCE_DIR}/../../SDKcode/GenTLwrapper 
${CMAKE_CURRENT_SOURCE_DIR}/../../SDKcode/GenTL
${OpenCV_INCLUDE_DIRS})
file(GLOB_RECURSE SRC *.cpp)


link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../SDKlib)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME}  ${OpenCV_LIBS}   Threads::Threads  DYVGenTL  GenTLwrapper)

