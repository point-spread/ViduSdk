cmake_minimum_required(VERSION 3.5)
project(getCameraPara)
 
include_directories(${PROJECT_SOURCE_DIR} 
${CMAKE_CURRENT_SOURCE_DIR}/../../SDKcode/common
${CMAKE_CURRENT_SOURCE_DIR}/../../SDKcode/GenTLwrapper 
${CMAKE_CURRENT_SOURCE_DIR}/../../SDKcode/GenTL)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../SDKlib/)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME}  GenTLwrapper  DYVGenTL)
