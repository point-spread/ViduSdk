cmake_minimum_required(VERSION 3.5)
project(streamShow)

include_directories(${PROJECT_SOURCE_DIR} ${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} Vidu_SDK_Api::GenTLwrapper ${OpenCV_LIBS})

