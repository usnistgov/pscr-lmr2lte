# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build

# Utility rule file for PSCR_LMR2LTE_swig_swig_compilation.

# Include the progress variables for this target.
include swig/CMakeFiles/PSCR_LMR2LTE_swig_swig_compilation.dir/progress.make

swig/CMakeFiles/PSCR_LMR2LTE_swig_swig_compilation: swig/CMakeFiles/PSCR_LMR2LTE_swig.dir/PSCR_LMR2LTE_swigPYTHON.stamp


swig/CMakeFiles/PSCR_LMR2LTE_swig.dir/PSCR_LMR2LTE_swigPYTHON.stamp: /home/jonc/gnuradio/lib/python3/dist-packages/gnuradio/gr/_runtime_swig.so
swig/CMakeFiles/PSCR_LMR2LTE_swig.dir/PSCR_LMR2LTE_swigPYTHON.stamp: ../swig/PSCR_LMR2LTE_swig.i
swig/CMakeFiles/PSCR_LMR2LTE_swig.dir/PSCR_LMR2LTE_swigPYTHON.stamp: ../swig/PSCR_LMR2LTE_swig.i
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Swig source PSCR_LMR2LTE_swig.i"
	cd /home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/swig && /usr/bin/cmake -E make_directory /home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/swig /home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/swig/CMakeFiles/PSCR_LMR2LTE_swig.dir
	cd /home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/swig && /usr/bin/cmake -E touch /home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/swig/CMakeFiles/PSCR_LMR2LTE_swig.dir/PSCR_LMR2LTE_swigPYTHON.stamp
	cd /home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/swig && /usr/bin/cmake -E env SWIG_LIB=/usr/share/swig4.0 /usr/bin/swig4.0 -python -fvirtual -keyword -w511 -w314 -relativeimport -py3 -module PSCR_LMR2LTE_swig -I/home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/swig -I/home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/swig -I/home/jonc/gnuradio/include/gnuradio/swig -I/usr/include/python3.8 -I/home/jonc/gnuradio/include -I/home/jonc/gnuradio/include -I/usr/include -I/usr/include -I/usr/include -I/usr/include -I/home/jonc/gnuradio/include -I/usr/include -I/usr/include -I/usr/include -I/usr/include -I/usr/include -I/home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/lib/../include -I/usr/include/opus/ -I/usr/include/ -I/usr/local/include -I/home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/swig -I/home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/swig -I/home/jonc/gnuradio/include/gnuradio/swig -I/usr/include/python3.8 -I/home/jonc/gnuradio/include -I/home/jonc/gnuradio/include -I/usr/include -I/usr/include -I/usr/include -I/usr/include -I/home/jonc/gnuradio/include -I/usr/include -I/usr/include -I/usr/include -I/usr/include -I/usr/include -I/home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/lib/../include -I/usr/include/opus/ -I/usr/include/ -I/usr/local/include -outdir /home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/swig -c++ -o /home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/swig/CMakeFiles/PSCR_LMR2LTE_swig.dir/PSCR_LMR2LTE_swigPYTHON_wrap.cxx /home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/swig/PSCR_LMR2LTE_swig.i

PSCR_LMR2LTE_swig_swig_compilation: swig/CMakeFiles/PSCR_LMR2LTE_swig_swig_compilation
PSCR_LMR2LTE_swig_swig_compilation: swig/CMakeFiles/PSCR_LMR2LTE_swig.dir/PSCR_LMR2LTE_swigPYTHON.stamp
PSCR_LMR2LTE_swig_swig_compilation: swig/CMakeFiles/PSCR_LMR2LTE_swig_swig_compilation.dir/build.make

.PHONY : PSCR_LMR2LTE_swig_swig_compilation

# Rule to build all files generated by this target.
swig/CMakeFiles/PSCR_LMR2LTE_swig_swig_compilation.dir/build: PSCR_LMR2LTE_swig_swig_compilation

.PHONY : swig/CMakeFiles/PSCR_LMR2LTE_swig_swig_compilation.dir/build

swig/CMakeFiles/PSCR_LMR2LTE_swig_swig_compilation.dir/clean:
	cd /home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/swig && $(CMAKE_COMMAND) -P CMakeFiles/PSCR_LMR2LTE_swig_swig_compilation.dir/cmake_clean.cmake
.PHONY : swig/CMakeFiles/PSCR_LMR2LTE_swig_swig_compilation.dir/clean

swig/CMakeFiles/PSCR_LMR2LTE_swig_swig_compilation.dir/depend:
	cd /home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE /home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/swig /home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build /home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/swig /home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/swig/CMakeFiles/PSCR_LMR2LTE_swig_swig_compilation.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : swig/CMakeFiles/PSCR_LMR2LTE_swig_swig_compilation.dir/depend
