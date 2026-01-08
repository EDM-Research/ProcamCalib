# Install script for directory: /home/jvanherck/test/ProcamCalib/src/3rdparty/DeviceFactory

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/include/DeviceFactory/CVImageCaptureDevice.h;/usr/local/include/DeviceFactory/CVVideoCaptureDevice.h;/usr/local/include/DeviceFactory/CameraCalibration.h;/usr/local/include/DeviceFactory/Device.h;/usr/local/include/DeviceFactory/DeviceFactory.h;/usr/local/include/DeviceFactory/FFMPEGDevice.h;/usr/local/include/DeviceFactory/RealSense2Device.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/local/include/DeviceFactory" TYPE FILE FILES
    "/home/jvanherck/test/ProcamCalib/src/3rdparty/DeviceFactory/include/DeviceFactory/CVImageCaptureDevice.h"
    "/home/jvanherck/test/ProcamCalib/src/3rdparty/DeviceFactory/include/DeviceFactory/CVVideoCaptureDevice.h"
    "/home/jvanherck/test/ProcamCalib/src/3rdparty/DeviceFactory/include/DeviceFactory/CameraCalibration.h"
    "/home/jvanherck/test/ProcamCalib/src/3rdparty/DeviceFactory/include/DeviceFactory/Device.h"
    "/home/jvanherck/test/ProcamCalib/src/3rdparty/DeviceFactory/include/DeviceFactory/DeviceFactory.h"
    "/home/jvanherck/test/ProcamCalib/src/3rdparty/DeviceFactory/include/DeviceFactory/FFMPEGDevice.h"
    "/home/jvanherck/test/ProcamCalib/src/3rdparty/DeviceFactory/include/DeviceFactory/RealSense2Device.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libDeviceFactory.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libDeviceFactory.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libDeviceFactory.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/jvanherck/test/ProcamCalib/src/build/3rdparty/DeviceFactory/libDeviceFactory.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libDeviceFactory.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libDeviceFactory.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libDeviceFactory.so"
         OLD_RPATH "/usr/local/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libDeviceFactory.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/DeviceFactory/DeviceFactoryTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/DeviceFactory/DeviceFactoryTargets.cmake"
         "/home/jvanherck/test/ProcamCalib/src/build/3rdparty/DeviceFactory/CMakeFiles/Export/lib/cmake/DeviceFactory/DeviceFactoryTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/DeviceFactory/DeviceFactoryTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/DeviceFactory/DeviceFactoryTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/DeviceFactory" TYPE FILE FILES "/home/jvanherck/test/ProcamCalib/src/build/3rdparty/DeviceFactory/CMakeFiles/Export/lib/cmake/DeviceFactory/DeviceFactoryTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/DeviceFactory" TYPE FILE FILES "/home/jvanherck/test/ProcamCalib/src/build/3rdparty/DeviceFactory/CMakeFiles/Export/lib/cmake/DeviceFactory/DeviceFactoryTargets-noconfig.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/DeviceFactory" TYPE FILE FILES
    "/home/jvanherck/test/ProcamCalib/src/build/3rdparty/DeviceFactory/DeviceFactoryConfig.cmake"
    "/home/jvanherck/test/ProcamCalib/src/build/3rdparty/DeviceFactory/DeviceFactoryConfigVersion.cmake"
    )
endif()

