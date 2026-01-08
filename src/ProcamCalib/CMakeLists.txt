cmake_minimum_required(VERSION 3.5)

project(ProcamCalib)
add_executable(ProcamCalib 
    ProcamCalib.cpp
    ProcamCalibrator.cpp
    Projector.cpp
    ../common/CharucoDetector.cpp
    ../common/Utils.cpp
    ../common/MirrorPlane.cpp)

target_include_directories(ProcamCalib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../common)

target_link_libraries(ProcamCalib
    DeviceFactory)

install(TARGETS ProcamCalib 
        RUNTIME DESTINATION bin)