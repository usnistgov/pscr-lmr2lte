#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "gnuradio::gnuradio-PSCR_LMR2LTE" for configuration "Release"
set_property(TARGET gnuradio::gnuradio-PSCR_LMR2LTE APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gnuradio::gnuradio-PSCR_LMR2LTE PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libgnuradio-PSCR_LMR2LTE.so.v1.0-compat-xxx-xunknown"
  IMPORTED_SONAME_RELEASE "libgnuradio-PSCR_LMR2LTE.so.1.0.0git"
  )

list(APPEND _IMPORT_CHECK_TARGETS gnuradio::gnuradio-PSCR_LMR2LTE )
list(APPEND _IMPORT_CHECK_FILES_FOR_gnuradio::gnuradio-PSCR_LMR2LTE "${_IMPORT_PREFIX}/lib/libgnuradio-PSCR_LMR2LTE.so.v1.0-compat-xxx-xunknown" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
