# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-frames

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-frames/pod-build

# Include any dependencies generated for this target.
include src/renderer/CMakeFiles/test-viewer.dir/depend.make

# Include the progress variables for this target.
include src/renderer/CMakeFiles/test-viewer.dir/progress.make

# Include the compile flags for this target's objects.
include src/renderer/CMakeFiles/test-viewer.dir/flags.make

src/renderer/CMakeFiles/test-viewer.dir/test_viewer.c.o: src/renderer/CMakeFiles/test-viewer.dir/flags.make
src/renderer/CMakeFiles/test-viewer.dir/test_viewer.c.o: ../src/renderer/test_viewer.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-frames/pod-build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object src/renderer/CMakeFiles/test-viewer.dir/test_viewer.c.o"
	cd /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-frames/pod-build/src/renderer && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/test-viewer.dir/test_viewer.c.o   -c /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-frames/src/renderer/test_viewer.c

src/renderer/CMakeFiles/test-viewer.dir/test_viewer.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/test-viewer.dir/test_viewer.c.i"
	cd /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-frames/pod-build/src/renderer && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-frames/src/renderer/test_viewer.c > CMakeFiles/test-viewer.dir/test_viewer.c.i

src/renderer/CMakeFiles/test-viewer.dir/test_viewer.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/test-viewer.dir/test_viewer.c.s"
	cd /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-frames/pod-build/src/renderer && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-frames/src/renderer/test_viewer.c -o CMakeFiles/test-viewer.dir/test_viewer.c.s

src/renderer/CMakeFiles/test-viewer.dir/test_viewer.c.o.requires:
.PHONY : src/renderer/CMakeFiles/test-viewer.dir/test_viewer.c.o.requires

src/renderer/CMakeFiles/test-viewer.dir/test_viewer.c.o.provides: src/renderer/CMakeFiles/test-viewer.dir/test_viewer.c.o.requires
	$(MAKE) -f src/renderer/CMakeFiles/test-viewer.dir/build.make src/renderer/CMakeFiles/test-viewer.dir/test_viewer.c.o.provides.build
.PHONY : src/renderer/CMakeFiles/test-viewer.dir/test_viewer.c.o.provides

src/renderer/CMakeFiles/test-viewer.dir/test_viewer.c.o.provides.build: src/renderer/CMakeFiles/test-viewer.dir/test_viewer.c.o

# Object files for target test-viewer
test__viewer_OBJECTS = \
"CMakeFiles/test-viewer.dir/test_viewer.c.o"

# External object files for target test-viewer
test__viewer_EXTERNAL_OBJECTS =

bin/test-viewer: src/renderer/CMakeFiles/test-viewer.dir/test_viewer.c.o
bin/test-viewer: src/renderer/CMakeFiles/test-viewer.dir/build.make
bin/test-viewer: src/renderer/CMakeFiles/test-viewer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable ../../bin/test-viewer"
	cd /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-frames/pod-build/src/renderer && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test-viewer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/renderer/CMakeFiles/test-viewer.dir/build: bin/test-viewer
.PHONY : src/renderer/CMakeFiles/test-viewer.dir/build

src/renderer/CMakeFiles/test-viewer.dir/requires: src/renderer/CMakeFiles/test-viewer.dir/test_viewer.c.o.requires
.PHONY : src/renderer/CMakeFiles/test-viewer.dir/requires

src/renderer/CMakeFiles/test-viewer.dir/clean:
	cd /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-frames/pod-build/src/renderer && $(CMAKE_COMMAND) -P CMakeFiles/test-viewer.dir/cmake_clean.cmake
.PHONY : src/renderer/CMakeFiles/test-viewer.dir/clean

src/renderer/CMakeFiles/test-viewer.dir/depend:
	cd /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-frames/pod-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-frames /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-frames/src/renderer /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-frames/pod-build /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-frames/pod-build/src/renderer /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-frames/pod-build/src/renderer/CMakeFiles/test-viewer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/renderer/CMakeFiles/test-viewer.dir/depend

