#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "xlnt::xlnt" for configuration "Release"
set_property(TARGET xlnt::xlnt APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(xlnt::xlnt PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libxlnt.so.1.5.0"
  IMPORTED_SONAME_RELEASE "libxlnt.so.1.5.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS xlnt::xlnt )
list(APPEND _IMPORT_CHECK_FILES_FOR_xlnt::xlnt "${_IMPORT_PREFIX}/lib/libxlnt.so.1.5.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
