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

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lash/git/acfr_lcm/third-party/libbot2/bot2-frames

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lash/git/acfr_lcm/third-party/libbot2/bot2-frames/pod-build

# Include any dependencies generated for this target.
include src/test/CMakeFiles/coord-frames-test.dir/depend.make

# Include the progress variables for this target.
include src/test/CMakeFiles/coord-frames-test.dir/progress.make

# Include the compile flags for this target's objects.
include src/test/CMakeFiles/coord-frames-test.dir/flags.make

src/test/CMakeFiles/coord-frames-test.dir/coord_frames_test.c.o: src/test/CMakeFiles/coord-frames-test.dir/flags.make
src/test/CMakeFiles/coord-frames-test.dir/coord_frames_test.c.o: ../src/test/coord_frames_test.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/lash/git/acfr_lcm/third-party/libbot2/bot2-frames/pod-build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object src/test/CMakeFiles/coord-frames-test.dir/coord_frames_test.c.o"
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-frames/pod-build/src/test && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/coord-frames-test.dir/coord_frames_test.c.o   -c /home/lash/git/acfr_lcm/third-party/libbot2/bot2-frames/src/test/coord_frames_test.c

src/test/CMakeFiles/coord-frames-test.dir/coord_frames_test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/coord-frames-test.dir/coord_frames_test.c.i"
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-frames/pod-build/src/test && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/lash/git/acfr_lcm/third-party/libbot2/bot2-frames/src/test/coord_frames_test.c > CMakeFiles/coord-frames-test.dir/coord_frames_test.c.i

src/test/CMakeFiles/coord-frames-test.dir/coord_frames_test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/coord-frames-test.dir/coord_frames_test.c.s"
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-frames/pod-build/src/test && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/lash/git/acfr_lcm/third-party/libbot2/bot2-frames/src/test/coord_frames_test.c -o CMakeFiles/coord-frames-test.dir/coord_frames_test.c.s

src/test/CMakeFiles/coord-frames-test.dir/coord_frames_test.c.o.requires:
.PHONY : src/test/CMakeFiles/coord-frames-test.dir/coord_frames_test.c.o.requires

src/test/CMakeFiles/coord-frames-test.dir/coord_frames_test.c.o.provides: src/test/CMakeFiles/coord-frames-test.dir/coord_frames_test.c.o.requires
	$(MAKE) -f src/test/CMakeFiles/coord-frames-test.dir/build.make src/test/CMakeFiles/coord-frames-test.dir/coord_frames_test.c.o.provides.build
.PHONY : src/test/CMakeFiles/coord-frames-test.dir/coord_frames_test.c.o.provides

src/test/CMakeFiles/coord-frames-test.dir/coord_frames_test.c.o.provides.build: src/test/CMakeFiles/coord-frames-test.dir/coord_frames_test.c.o

# Object files for target coord-frames-test
coord__frames__test_OBJECTS = \
"CMakeFiles/coord-frames-test.dir/coord_frames_test.c.o"

# External object files for target coord-frames-test
coord__frames__test_EXTERNAL_OBJECTS =

bin/coord-frames-test: src/test/CMakeFiles/coord-frames-test.dir/coord_frames_test.c.o
bin/coord-frames-test: src/test/CMakeFiles/coord-frames-test.dir/build.make
bin/coord-frames-test: src/test/CMakeFiles/coord-frames-test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable ../../bin/coord-frames-test"
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-frames/pod-build/src/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/coord-frames-test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/test/CMakeFiles/coord-frames-test.dir/build: bin/coord-frames-test
.PHONY : src/test/CMakeFiles/coord-frames-test.dir/build

src/test/CMakeFiles/coord-frames-test.dir/requires: src/test/CMakeFiles/coord-frames-test.dir/coord_frames_test.c.o.requires
.PHONY : src/test/CMakeFiles/coord-frames-test.dir/requires

src/test/CMakeFiles/coord-frames-test.dir/clean:
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-frames/pod-build/src/test && $(CMAKE_COMMAND) -P CMakeFiles/coord-frames-test.dir/cmake_clean.cmake
.PHONY : src/test/CMakeFiles/coord-frames-test.dir/clean

src/test/CMakeFiles/coord-frames-test.dir/depend:
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-frames/pod-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lash/git/acfr_lcm/third-party/libbot2/bot2-frames /home/lash/git/acfr_lcm/third-party/libbot2/bot2-frames/src/test /home/lash/git/acfr_lcm/third-party/libbot2/bot2-frames/pod-build /home/lash/git/acfr_lcm/third-party/libbot2/bot2-frames/pod-build/src/test /home/lash/git/acfr_lcm/third-party/libbot2/bot2-frames/pod-build/src/test/CMakeFiles/coord-frames-test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/test/CMakeFiles/coord-frames-test.dir/depend

