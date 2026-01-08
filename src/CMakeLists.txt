cmake_minimum_required(VERSION 3.5)

project(ProcamCalib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/DeviceFactory)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/DeviceFactory/include)

set(BUILD_SHARED_LIBS OFF)

add_subdirectory(${CMAKE_SOURCE_DIR}/CamCalib)
add_subdirectory(${CMAKE_SOURCE_DIR}/ProcamCalib)
add_subdirectory(${CMAKE_SOURCE_DIR}/MirrorCalib)