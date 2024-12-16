cmake_minimum_required(VERSION 3.5)
project(algorithm_demo)

include_directories(${PROJECT_SOURCE_DIR})

if(LIB_BUILD_STATIC)
    include(${DYV_SDK_PATH}/thirdParty/genicam/CMakeLists.txt)
    link_directories(${GENICAM_LIBRARIES_DIR})
endif()

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} Vidu_SDK_Api::GenTLwrapper)
