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
CMAKE_SOURCE_DIR = /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-core

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-core/pod-build

# Utility rule file for lcmgen_c.

# Include the progress variables for this target.
include CMakeFiles/lcmgen_c.dir/progress.make

CMakeFiles/lcmgen_c:
	sh -c '[ -d /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-core/lcmtypes/c/lcmtypes ] || mkdir -p /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-core/lcmtypes/c/lcmtypes'
	sh -c '/usr/local/bin/lcm-gen --lazy -c /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-core/lcmtypes/bot_core_rigid_transform_t.lcm /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-core/lcmtypes/bot_core_image_sync_t.lcm /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-core/lcmtypes/bot_core_image_t.lcm /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-core/lcmtypes/bot_core_sensor_status_t.lcm /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-core/lcmtypes/bot_core_planar_lidar_t.lcm /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-core/lcmtypes/bot_core_pose_t.lcm /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-core/lcmtypes/bot_core_raw_t.lcm /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-core/lcmtypes/bot_core_image_metadata_t.lcm --c-cpath /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-core/lcmtypes/c/lcmtypes --c-hpath /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-core/lcmtypes/c/lcmtypes'

lcmgen_c: CMakeFiles/lcmgen_c
lcmgen_c: CMakeFiles/lcmgen_c.dir/build.make
.PHONY : lcmgen_c

# Rule to build all files generated by this target.
CMakeFiles/lcmgen_c.dir/build: lcmgen_c
.PHONY : CMakeFiles/lcmgen_c.dir/build

CMakeFiles/lcmgen_c.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/lcmgen_c.dir/cmake_clean.cmake
.PHONY : CMakeFiles/lcmgen_c.dir/clean

CMakeFiles/lcmgen_c.dir/depend:
	cd /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-core/pod-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-core /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-core /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-core/pod-build /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-core/pod-build /home/navid/proj/acfr/acfr_lcm/third-party/libbot2New/bot2-core/pod-build/CMakeFiles/lcmgen_c.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/lcmgen_c.dir/depend

