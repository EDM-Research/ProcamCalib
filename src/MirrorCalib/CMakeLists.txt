cmake_minimum_required(VERSION 3.12)
project(MirrorCalib LANGUAGES CXX)

add_executable(MirrorCalib
    MirrorCalib.cpp
    MirrorCalibrator.cpp
    ../common/CharucoDetector.cpp
    ../common/Utils.cpp
    ../common/MirrorPlane.cpp
)

target_include_directories(MirrorCalib
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../common
)


target_link_libraries(MirrorCalib
    PRIVATE
        DeviceFactory              
)

install(TARGETS MirrorCalib
        RUNTIME DESTINATION bin)
