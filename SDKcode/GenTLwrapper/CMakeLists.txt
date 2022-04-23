cmake_minimum_required(VERSION 3.5)
project(GenTLwrapper)

find_package(OpenCV REQUIRED)
if (CMAKE_GENERATOR MATCHES "Visual Studio")
else()
    add_compile_options(-fPIC)
endif()
include_directories(
    ${PROJECT_SOURCE_DIR} ${OpenCV_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/../common  
    ${CMAKE_CURRENT_SOURCE_DIR}/../GenTL)
file(GLOB_RECURSE SRC src/*.cpp basic/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../common/*.cpp)
add_library(${PROJECT_NAME} STATIC ${SRC})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ${Eigen3_LIBRARIES} ${CMAKE_CURRENT_SOURCE_DIR}/../../SDKlib/libDYVGenTL.cti)


