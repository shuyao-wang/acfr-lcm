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
CMAKE_SOURCE_DIR = /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-lcmgl

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-lcmgl/pod-build

# Include any dependencies generated for this target.
include CMakeFiles/lcmtypes_bot2-lcmgl.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/lcmtypes_bot2-lcmgl.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/lcmtypes_bot2-lcmgl.dir/flags.make

CMakeFiles/lcmtypes_bot2-lcmgl.dir/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c.o: CMakeFiles/lcmtypes_bot2-lcmgl.dir/flags.make
CMakeFiles/lcmtypes_bot2-lcmgl.dir/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c.o: ../lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-lcmgl/pod-build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/lcmtypes_bot2-lcmgl.dir/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -fPIC -o CMakeFiles/lcmtypes_bot2-lcmgl.dir/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c.o   -c /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-lcmgl/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c

CMakeFiles/lcmtypes_bot2-lcmgl.dir/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/lcmtypes_bot2-lcmgl.dir/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -fPIC -E /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-lcmgl/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c > CMakeFiles/lcmtypes_bot2-lcmgl.dir/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c.i

CMakeFiles/lcmtypes_bot2-lcmgl.dir/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/lcmtypes_bot2-lcmgl.dir/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -fPIC -S /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-lcmgl/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c -o CMakeFiles/lcmtypes_bot2-lcmgl.dir/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c.s

CMakeFiles/lcmtypes_bot2-lcmgl.dir/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c.o.requires:
.PHONY : CMakeFiles/lcmtypes_bot2-lcmgl.dir/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c.o.requires

CMakeFiles/lcmtypes_bot2-lcmgl.dir/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c.o.provides: CMakeFiles/lcmtypes_bot2-lcmgl.dir/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c.o.requires
	$(MAKE) -f CMakeFiles/lcmtypes_bot2-lcmgl.dir/build.make CMakeFiles/lcmtypes_bot2-lcmgl.dir/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c.o.provides.build
.PHONY : CMakeFiles/lcmtypes_bot2-lcmgl.dir/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c.o.provides

CMakeFiles/lcmtypes_bot2-lcmgl.dir/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c.o.provides.build: CMakeFiles/lcmtypes_bot2-lcmgl.dir/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c.o

# Object files for target lcmtypes_bot2-lcmgl
lcmtypes_bot2__lcmgl_OBJECTS = \
"CMakeFiles/lcmtypes_bot2-lcmgl.dir/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c.o"

# External object files for target lcmtypes_bot2-lcmgl
lcmtypes_bot2__lcmgl_EXTERNAL_OBJECTS =

lib/liblcmtypes_bot2-lcmgl.a: CMakeFiles/lcmtypes_bot2-lcmgl.dir/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c.o
lib/liblcmtypes_bot2-lcmgl.a: CMakeFiles/lcmtypes_bot2-lcmgl.dir/build.make
lib/liblcmtypes_bot2-lcmgl.a: CMakeFiles/lcmtypes_bot2-lcmgl.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C static library lib/liblcmtypes_bot2-lcmgl.a"
	$(CMAKE_COMMAND) -P CMakeFiles/lcmtypes_bot2-lcmgl.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lcmtypes_bot2-lcmgl.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/lcmtypes_bot2-lcmgl.dir/build: lib/liblcmtypes_bot2-lcmgl.a
.PHONY : CMakeFiles/lcmtypes_bot2-lcmgl.dir/build

CMakeFiles/lcmtypes_bot2-lcmgl.dir/requires: CMakeFiles/lcmtypes_bot2-lcmgl.dir/lcmtypes/c/lcmtypes/bot_lcmgl_data_t.c.o.requires
.PHONY : CMakeFiles/lcmtypes_bot2-lcmgl.dir/requires

CMakeFiles/lcmtypes_bot2-lcmgl.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/lcmtypes_bot2-lcmgl.dir/cmake_clean.cmake
.PHONY : CMakeFiles/lcmtypes_bot2-lcmgl.dir/clean

CMakeFiles/lcmtypes_bot2-lcmgl.dir/depend:
	cd /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-lcmgl/pod-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-lcmgl /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-lcmgl /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-lcmgl/pod-build /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-lcmgl/pod-build /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-lcmgl/pod-build/CMakeFiles/lcmtypes_bot2-lcmgl.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/lcmtypes_bot2-lcmgl.dir/depend

