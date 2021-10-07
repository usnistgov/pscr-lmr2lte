# Install script for directory: /home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/swig

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/jonc/gnuradio")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/PSCR_LMR2LTE/_PSCR_LMR2LTE_swig.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/PSCR_LMR2LTE/_PSCR_LMR2LTE_swig.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/PSCR_LMR2LTE/_PSCR_LMR2LTE_swig.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/PSCR_LMR2LTE" TYPE MODULE FILES "/home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/swig/_PSCR_LMR2LTE_swig.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/PSCR_LMR2LTE/_PSCR_LMR2LTE_swig.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/PSCR_LMR2LTE/_PSCR_LMR2LTE_swig.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/PSCR_LMR2LTE/_PSCR_LMR2LTE_swig.so"
         OLD_RPATH "/home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/lib:/usr/local/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/PSCR_LMR2LTE/_PSCR_LMR2LTE_swig.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/PSCR_LMR2LTE" TYPE FILE FILES "/home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/swig/PSCR_LMR2LTE_swig.py")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/PSCR_LMR2LTE" TYPE FILE FILES
    "/home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/swig/PSCR_LMR2LTE_swig.pyc"
    "/home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/swig/PSCR_LMR2LTE_swig.pyo"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/PSCR_LMR2LTE/PSCR_LMR2LTE/swig" TYPE FILE FILES
    "/home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/swig/PSCR_LMR2LTE_swig.i"
    "/home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/swig/PSCR_LMR2LTE_swig_doc.i"
    )
endif()

