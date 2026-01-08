#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "DeviceFactory::DeviceFactory" for configuration ""
set_property(TARGET DeviceFactory::DeviceFactory APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(DeviceFactory::DeviceFactory PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libDeviceFactory.so"
  IMPORTED_SONAME_NOCONFIG "libDeviceFactory.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS DeviceFactory::DeviceFactory )
list(APPEND _IMPORT_CHECK_FILES_FOR_DeviceFactory::DeviceFactory "${_IMPORT_PREFIX}/lib/libDeviceFactory.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
