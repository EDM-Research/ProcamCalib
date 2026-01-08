cmake_minimum_required(VERSION 3.5)

project(CamCalib)

add_executable(CamCalib 
    CamCalib.cpp
    CameraCalibrator.cpp
    ../common/CharucoDetector.cpp
    ../common/Utils.cpp)

target_include_directories(CamCalib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../common)

target_link_libraries(CamCalib
    DeviceFactory)

install(TARGETS CamCalib 
        RUNTIME DESTINATION bin)