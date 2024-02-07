cmake_minimum_required(VERSION 3.5)
project(GenTLwrapper VERSION 0.0)

find_package(OpenCV REQUIRED)

file(GLOB_RECURSE SRC src/*.cpp basic/*.cpp)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(${PROJECT_NAME} SHARED ${SRC})

target_include_directories(${PROJECT_NAME} PUBLIC
    # ${OpenCV_INCLUDE_DIRS}    
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:Vidu_SDK_Api/SDKcode/GenTLwrapper/include> 

    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>
    $<INSTALL_INTERFACE:Vidu_SDK_Api/SDKcode/common>

    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../GenTL>
    $<INSTALL_INTERFACE:Vidu_SDK_Api/SDKcode/GenTL>
)

target_include_directories(${PROJECT_NAME} INTERFACE   
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../SDKcode> 
    $<INSTALL_INTERFACE:Vidu_SDK_Api/SDKcode>
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${OpenCV_INCLUDE_DIRS}
)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/../Vidu_SDK_Api.h)

add_library(DYVGenTL SHARED IMPORTED)

if(WIN32)
    set_property(TARGET DYVGenTL PROPERTY IMPORTED_LOCATION  ${CMAKE_CURRENT_LIST_DIR}/../../SDKlib/DYVGenTL.cti)
    set_property(TARGET DYVGenTL PROPERTY IMPORTED_IMPLIB ${CMAKE_CURRENT_LIST_DIR}/../../SDKlib/DYVGenTL.lib)
else()
    set_property(TARGET DYVGenTL PROPERTY IMPORTED_LOCATION  ${CMAKE_CURRENT_LIST_DIR}/../../SDKlib/libDYVGenTL.cti)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC DYVGenTL ${OpenCV_LIBS})

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    Vidu_SDK_ApiConfigVersion.cmake 
    VERSION ${PACKAGE_VERSION} COMPATIBILITY AnyNewerVersion
)

install(TARGETS GenTLwrapper
    EXPORT Vidu_SDK_Api
    LIBRARY DESTINATION Vidu_SDK_Api/SDKlib
    ARCHIVE DESTINATION Vidu_SDK_Api/SDKlib
    RUNTIME DESTINATION Vidu_SDK_Api/SDKlib
    INCLUDES DESTINATION Vidu_SDK_Api/SDKcode
    PUBLIC_HEADER DESTINATION Vidu_SDK_Api/SDKcode
)

install(EXPORT Vidu_SDK_Api
    FILE "Vidu_SDK_Api.cmake"
    NAMESPACE Vidu_SDK_Api::
    DESTINATION "Vidu_SDK_Api/cmake"
)

configure_file(cmake.in Vidu_SDK_ApiConfig.cmake @ONLY)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/Vidu_SDK_ApiConfig.cmake"
    DESTINATION "Vidu_SDK_Api/cmake"
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/Vidu_SDK_ApiConfigVersion.cmake"
    DESTINATION "Vidu_SDK_Api/cmake"
)

install(
    DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../../SDKlib
    DESTINATION Vidu_SDK_Api
)

install(
    DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../../SDKcode
    DESTINATION  Vidu_SDK_Api
    FILES_MATCHING PATTERN "*.h"
    PATTERN "build" EXCLUDE
)
    
export(EXPORT Vidu_SDK_Api
    FILE "${CMAKE_CURRENT_BINARY_DIR}/Vidu_SDK_Api.cmake"
)
