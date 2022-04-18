project(OCULO_SDK)
cmake_minimum_required(VERSION 3.10)

include_directories(${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../common)
add_subdirectory(example)
add_subdirectory(code/GenTLwrapper)
