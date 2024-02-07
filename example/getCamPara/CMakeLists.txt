cmake_minimum_required(VERSION 3.5)
project(getCamPara)

include_directories(${PROJECT_SOURCE_DIR} ${OpenCV_INCLUDE_DIRS})

if(LIB_BUILD_STATIC)
    include(${DYV_SDK_PATH}/thirdParty/genicam/CMakeLists.txt)
    link_directories(${GENICAM_LIBRARIES_DIR})
endif()

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} Vidu_SDK_Api::GenTLwrapper ${OpenCV_LIBS})
