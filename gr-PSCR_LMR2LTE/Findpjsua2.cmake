# - Try to find pjsua2
# Once done this will define
#
#  PJSUA2_FOUND - system has pjsua2
#  PJSUA2_INCLUDE_DIRS - the pjsua2 include directory
#  PJSUA2_LIBRARIES - Link these to use pjsua2

if (PJSUA2_LIBRARIES AND PJSUA2_INCLUDE_DIRS)
  # in cache already
  set(PJSUA2_FOUND TRUE)
else (PJSUA2_LIBRARIES AND PJSUA2_INCLUDE_DIRS)
  find_path(PJSUA2_INCLUDE_DIR
    NAMES
      pjsua2.hpp
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
  )

find_library(PJSUA2_LIBRARY
	NAMES 
		pjsua2
	PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )

  if (PJSUA2_LIBRARY AND PJSUA2_INCLUDE_DIR)
    set(PJSUA2_FOUND TRUE)

    set(PJSUA2_INCLUDE_DIRS
      ${PJSUA2_INCLUDE_DIR}
    )

    set(PJSUA2_LIBRARIES
      ${PJSUA2_LIBRARIES}
      ${PJSUA2_LIBRARY}
	)

  endif (PJSUA2_LIBRARY AND PJSUA2_INCLUDE_DIR)

  if (PJSUA2_FOUND)
    if (NOT PJSUA2_FIND_QUIETLY)
      message(STATUS "Found pjsua2: ${PJSUA2_LIBRARIES}")
    endif (NOT PJSUA2_FIND_QUIETLY)
  else (PJSUA2_FOUND)
    if (PJSUA2_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find PJSUA2")
    endif (PJSUA2_FIND_REQUIRED)
  endif (PJSUA2_FOUND)

endif (PJSUA2_LIBRARIES AND PJSUA2_INCLUDE_DIRS)
