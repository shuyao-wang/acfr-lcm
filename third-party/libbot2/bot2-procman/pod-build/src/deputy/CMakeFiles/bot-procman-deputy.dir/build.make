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
CMAKE_SOURCE_DIR = /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build

# Include any dependencies generated for this target.
include src/deputy/CMakeFiles/bot-procman-deputy.dir/depend.make

# Include the progress variables for this target.
include src/deputy/CMakeFiles/bot-procman-deputy.dir/progress.make

# Include the compile flags for this target's objects.
include src/deputy/CMakeFiles/bot-procman-deputy.dir/flags.make

src/deputy/CMakeFiles/bot-procman-deputy.dir/lcm_util.c.o: src/deputy/CMakeFiles/bot-procman-deputy.dir/flags.make
src/deputy/CMakeFiles/bot-procman-deputy.dir/lcm_util.c.o: ../src/deputy/lcm_util.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object src/deputy/CMakeFiles/bot-procman-deputy.dir/lcm_util.c.o"
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/src/deputy && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/bot-procman-deputy.dir/lcm_util.c.o   -c /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/src/deputy/lcm_util.c

src/deputy/CMakeFiles/bot-procman-deputy.dir/lcm_util.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/bot-procman-deputy.dir/lcm_util.c.i"
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/src/deputy && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/src/deputy/lcm_util.c > CMakeFiles/bot-procman-deputy.dir/lcm_util.c.i

src/deputy/CMakeFiles/bot-procman-deputy.dir/lcm_util.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/bot-procman-deputy.dir/lcm_util.c.s"
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/src/deputy && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/src/deputy/lcm_util.c -o CMakeFiles/bot-procman-deputy.dir/lcm_util.c.s

src/deputy/CMakeFiles/bot-procman-deputy.dir/lcm_util.c.o.requires:
.PHONY : src/deputy/CMakeFiles/bot-procman-deputy.dir/lcm_util.c.o.requires

src/deputy/CMakeFiles/bot-procman-deputy.dir/lcm_util.c.o.provides: src/deputy/CMakeFiles/bot-procman-deputy.dir/lcm_util.c.o.requires
	$(MAKE) -f src/deputy/CMakeFiles/bot-procman-deputy.dir/build.make src/deputy/CMakeFiles/bot-procman-deputy.dir/lcm_util.c.o.provides.build
.PHONY : src/deputy/CMakeFiles/bot-procman-deputy.dir/lcm_util.c.o.provides

src/deputy/CMakeFiles/bot-procman-deputy.dir/lcm_util.c.o.provides.build: src/deputy/CMakeFiles/bot-procman-deputy.dir/lcm_util.c.o

src/deputy/CMakeFiles/bot-procman-deputy.dir/procinfo.c.o: src/deputy/CMakeFiles/bot-procman-deputy.dir/flags.make
src/deputy/CMakeFiles/bot-procman-deputy.dir/procinfo.c.o: ../src/deputy/procinfo.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object src/deputy/CMakeFiles/bot-procman-deputy.dir/procinfo.c.o"
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/src/deputy && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/bot-procman-deputy.dir/procinfo.c.o   -c /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/src/deputy/procinfo.c

src/deputy/CMakeFiles/bot-procman-deputy.dir/procinfo.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/bot-procman-deputy.dir/procinfo.c.i"
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/src/deputy && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/src/deputy/procinfo.c > CMakeFiles/bot-procman-deputy.dir/procinfo.c.i

src/deputy/CMakeFiles/bot-procman-deputy.dir/procinfo.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/bot-procman-deputy.dir/procinfo.c.s"
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/src/deputy && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/src/deputy/procinfo.c -o CMakeFiles/bot-procman-deputy.dir/procinfo.c.s

src/deputy/CMakeFiles/bot-procman-deputy.dir/procinfo.c.o.requires:
.PHONY : src/deputy/CMakeFiles/bot-procman-deputy.dir/procinfo.c.o.requires

src/deputy/CMakeFiles/bot-procman-deputy.dir/procinfo.c.o.provides: src/deputy/CMakeFiles/bot-procman-deputy.dir/procinfo.c.o.requires
	$(MAKE) -f src/deputy/CMakeFiles/bot-procman-deputy.dir/build.make src/deputy/CMakeFiles/bot-procman-deputy.dir/procinfo.c.o.provides.build
.PHONY : src/deputy/CMakeFiles/bot-procman-deputy.dir/procinfo.c.o.provides

src/deputy/CMakeFiles/bot-procman-deputy.dir/procinfo.c.o.provides.build: src/deputy/CMakeFiles/bot-procman-deputy.dir/procinfo.c.o

src/deputy/CMakeFiles/bot-procman-deputy.dir/procman.c.o: src/deputy/CMakeFiles/bot-procman-deputy.dir/flags.make
src/deputy/CMakeFiles/bot-procman-deputy.dir/procman.c.o: ../src/deputy/procman.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object src/deputy/CMakeFiles/bot-procman-deputy.dir/procman.c.o"
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/src/deputy && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/bot-procman-deputy.dir/procman.c.o   -c /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/src/deputy/procman.c

src/deputy/CMakeFiles/bot-procman-deputy.dir/procman.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/bot-procman-deputy.dir/procman.c.i"
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/src/deputy && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/src/deputy/procman.c > CMakeFiles/bot-procman-deputy.dir/procman.c.i

src/deputy/CMakeFiles/bot-procman-deputy.dir/procman.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/bot-procman-deputy.dir/procman.c.s"
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/src/deputy && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/src/deputy/procman.c -o CMakeFiles/bot-procman-deputy.dir/procman.c.s

src/deputy/CMakeFiles/bot-procman-deputy.dir/procman.c.o.requires:
.PHONY : src/deputy/CMakeFiles/bot-procman-deputy.dir/procman.c.o.requires

src/deputy/CMakeFiles/bot-procman-deputy.dir/procman.c.o.provides: src/deputy/CMakeFiles/bot-procman-deputy.dir/procman.c.o.requires
	$(MAKE) -f src/deputy/CMakeFiles/bot-procman-deputy.dir/build.make src/deputy/CMakeFiles/bot-procman-deputy.dir/procman.c.o.provides.build
.PHONY : src/deputy/CMakeFiles/bot-procman-deputy.dir/procman.c.o.provides

src/deputy/CMakeFiles/bot-procman-deputy.dir/procman.c.o.provides.build: src/deputy/CMakeFiles/bot-procman-deputy.dir/procman.c.o

src/deputy/CMakeFiles/bot-procman-deputy.dir/procman_deputy.c.o: src/deputy/CMakeFiles/bot-procman-deputy.dir/flags.make
src/deputy/CMakeFiles/bot-procman-deputy.dir/procman_deputy.c.o: ../src/deputy/procman_deputy.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object src/deputy/CMakeFiles/bot-procman-deputy.dir/procman_deputy.c.o"
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/src/deputy && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/bot-procman-deputy.dir/procman_deputy.c.o   -c /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/src/deputy/procman_deputy.c

src/deputy/CMakeFiles/bot-procman-deputy.dir/procman_deputy.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/bot-procman-deputy.dir/procman_deputy.c.i"
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/src/deputy && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/src/deputy/procman_deputy.c > CMakeFiles/bot-procman-deputy.dir/procman_deputy.c.i

src/deputy/CMakeFiles/bot-procman-deputy.dir/procman_deputy.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/bot-procman-deputy.dir/procman_deputy.c.s"
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/src/deputy && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/src/deputy/procman_deputy.c -o CMakeFiles/bot-procman-deputy.dir/procman_deputy.c.s

src/deputy/CMakeFiles/bot-procman-deputy.dir/procman_deputy.c.o.requires:
.PHONY : src/deputy/CMakeFiles/bot-procman-deputy.dir/procman_deputy.c.o.requires

src/deputy/CMakeFiles/bot-procman-deputy.dir/procman_deputy.c.o.provides: src/deputy/CMakeFiles/bot-procman-deputy.dir/procman_deputy.c.o.requires
	$(MAKE) -f src/deputy/CMakeFiles/bot-procman-deputy.dir/build.make src/deputy/CMakeFiles/bot-procman-deputy.dir/procman_deputy.c.o.provides.build
.PHONY : src/deputy/CMakeFiles/bot-procman-deputy.dir/procman_deputy.c.o.provides

src/deputy/CMakeFiles/bot-procman-deputy.dir/procman_deputy.c.o.provides.build: src/deputy/CMakeFiles/bot-procman-deputy.dir/procman_deputy.c.o

src/deputy/CMakeFiles/bot-procman-deputy.dir/signal_pipe.c.o: src/deputy/CMakeFiles/bot-procman-deputy.dir/flags.make
src/deputy/CMakeFiles/bot-procman-deputy.dir/signal_pipe.c.o: ../src/deputy/signal_pipe.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object src/deputy/CMakeFiles/bot-procman-deputy.dir/signal_pipe.c.o"
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/src/deputy && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/bot-procman-deputy.dir/signal_pipe.c.o   -c /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/src/deputy/signal_pipe.c

src/deputy/CMakeFiles/bot-procman-deputy.dir/signal_pipe.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/bot-procman-deputy.dir/signal_pipe.c.i"
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/src/deputy && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/src/deputy/signal_pipe.c > CMakeFiles/bot-procman-deputy.dir/signal_pipe.c.i

src/deputy/CMakeFiles/bot-procman-deputy.dir/signal_pipe.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/bot-procman-deputy.dir/signal_pipe.c.s"
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/src/deputy && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/src/deputy/signal_pipe.c -o CMakeFiles/bot-procman-deputy.dir/signal_pipe.c.s

src/deputy/CMakeFiles/bot-procman-deputy.dir/signal_pipe.c.o.requires:
.PHONY : src/deputy/CMakeFiles/bot-procman-deputy.dir/signal_pipe.c.o.requires

src/deputy/CMakeFiles/bot-procman-deputy.dir/signal_pipe.c.o.provides: src/deputy/CMakeFiles/bot-procman-deputy.dir/signal_pipe.c.o.requires
	$(MAKE) -f src/deputy/CMakeFiles/bot-procman-deputy.dir/build.make src/deputy/CMakeFiles/bot-procman-deputy.dir/signal_pipe.c.o.provides.build
.PHONY : src/deputy/CMakeFiles/bot-procman-deputy.dir/signal_pipe.c.o.provides

src/deputy/CMakeFiles/bot-procman-deputy.dir/signal_pipe.c.o.provides.build: src/deputy/CMakeFiles/bot-procman-deputy.dir/signal_pipe.c.o

# Object files for target bot-procman-deputy
bot__procman__deputy_OBJECTS = \
"CMakeFiles/bot-procman-deputy.dir/lcm_util.c.o" \
"CMakeFiles/bot-procman-deputy.dir/procinfo.c.o" \
"CMakeFiles/bot-procman-deputy.dir/procman.c.o" \
"CMakeFiles/bot-procman-deputy.dir/procman_deputy.c.o" \
"CMakeFiles/bot-procman-deputy.dir/signal_pipe.c.o"

# External object files for target bot-procman-deputy
bot__procman__deputy_EXTERNAL_OBJECTS =

bin/bot-procman-deputy: src/deputy/CMakeFiles/bot-procman-deputy.dir/lcm_util.c.o
bin/bot-procman-deputy: src/deputy/CMakeFiles/bot-procman-deputy.dir/procinfo.c.o
bin/bot-procman-deputy: src/deputy/CMakeFiles/bot-procman-deputy.dir/procman.c.o
bin/bot-procman-deputy: src/deputy/CMakeFiles/bot-procman-deputy.dir/procman_deputy.c.o
bin/bot-procman-deputy: src/deputy/CMakeFiles/bot-procman-deputy.dir/signal_pipe.c.o
bin/bot-procman-deputy: src/deputy/CMakeFiles/bot-procman-deputy.dir/build.make
bin/bot-procman-deputy: src/deputy/CMakeFiles/bot-procman-deputy.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable ../../bin/bot-procman-deputy"
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/src/deputy && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/bot-procman-deputy.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/deputy/CMakeFiles/bot-procman-deputy.dir/build: bin/bot-procman-deputy
.PHONY : src/deputy/CMakeFiles/bot-procman-deputy.dir/build

src/deputy/CMakeFiles/bot-procman-deputy.dir/requires: src/deputy/CMakeFiles/bot-procman-deputy.dir/lcm_util.c.o.requires
src/deputy/CMakeFiles/bot-procman-deputy.dir/requires: src/deputy/CMakeFiles/bot-procman-deputy.dir/procinfo.c.o.requires
src/deputy/CMakeFiles/bot-procman-deputy.dir/requires: src/deputy/CMakeFiles/bot-procman-deputy.dir/procman.c.o.requires
src/deputy/CMakeFiles/bot-procman-deputy.dir/requires: src/deputy/CMakeFiles/bot-procman-deputy.dir/procman_deputy.c.o.requires
src/deputy/CMakeFiles/bot-procman-deputy.dir/requires: src/deputy/CMakeFiles/bot-procman-deputy.dir/signal_pipe.c.o.requires
.PHONY : src/deputy/CMakeFiles/bot-procman-deputy.dir/requires

src/deputy/CMakeFiles/bot-procman-deputy.dir/clean:
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/src/deputy && $(CMAKE_COMMAND) -P CMakeFiles/bot-procman-deputy.dir/cmake_clean.cmake
.PHONY : src/deputy/CMakeFiles/bot-procman-deputy.dir/clean

src/deputy/CMakeFiles/bot-procman-deputy.dir/depend:
	cd /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/src/deputy /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/src/deputy /home/lash/git/acfr_lcm/third-party/libbot2/bot2-procman/pod-build/src/deputy/CMakeFiles/bot-procman-deputy.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/deputy/CMakeFiles/bot-procman-deputy.dir/depend

