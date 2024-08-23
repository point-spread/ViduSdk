#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Vidu_SDK_Api::GenTLwrapper" for configuration "Release"
set_property(TARGET Vidu_SDK_Api::GenTLwrapper APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Vidu_SDK_Api::GenTLwrapper PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/SDKlib/GenTLwrapper.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/SDKlib/GenTLwrapper.dll"
  )

list(APPEND _cmake_import_check_targets Vidu_SDK_Api::GenTLwrapper )
list(APPEND _cmake_import_check_files_for_Vidu_SDK_Api::GenTLwrapper "${_IMPORT_PREFIX}/SDKlib/GenTLwrapper.lib" "${_IMPORT_PREFIX}/SDKlib/GenTLwrapper.dll" )

# Import target "Vidu_SDK_Api::DYVGenTL" for configuration "Release"
set_property(TARGET Vidu_SDK_Api::DYVGenTL APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Vidu_SDK_Api::DYVGenTL PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/SDKlib/DYVGenTL.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/SDKlib/DYVGenTL.cti"
  )

list(APPEND _cmake_import_check_targets Vidu_SDK_Api::DYVGenTL )
list(APPEND _cmake_import_check_files_for_Vidu_SDK_Api::DYVGenTL "${_IMPORT_PREFIX}/SDKlib/DYVGenTL.lib" "${_IMPORT_PREFIX}/SDKlib/DYVGenTL.cti" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
