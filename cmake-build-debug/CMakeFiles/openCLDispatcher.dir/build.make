# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_COMMAND = /home/tompi/MY_WORK_ZONE/CLion/clion-2018.1.1/bin/cmake/bin/cmake

# The command to remove a file.
RM = /home/tompi/MY_WORK_ZONE/CLion/clion-2018.1.1/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/tompi/MY_WORK_ZONE/Workspace/GitHubProject/DispatcherOpenCL

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/tompi/MY_WORK_ZONE/Workspace/GitHubProject/DispatcherOpenCL/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/openCLDispatcher.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/openCLDispatcher.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/openCLDispatcher.dir/flags.make

CMakeFiles/openCLDispatcher.dir/Main.cpp.o: CMakeFiles/openCLDispatcher.dir/flags.make
CMakeFiles/openCLDispatcher.dir/Main.cpp.o: ../Main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tompi/MY_WORK_ZONE/Workspace/GitHubProject/DispatcherOpenCL/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/openCLDispatcher.dir/Main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/openCLDispatcher.dir/Main.cpp.o -c /home/tompi/MY_WORK_ZONE/Workspace/GitHubProject/DispatcherOpenCL/Main.cpp

CMakeFiles/openCLDispatcher.dir/Main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/openCLDispatcher.dir/Main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tompi/MY_WORK_ZONE/Workspace/GitHubProject/DispatcherOpenCL/Main.cpp > CMakeFiles/openCLDispatcher.dir/Main.cpp.i

CMakeFiles/openCLDispatcher.dir/Main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/openCLDispatcher.dir/Main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tompi/MY_WORK_ZONE/Workspace/GitHubProject/DispatcherOpenCL/Main.cpp -o CMakeFiles/openCLDispatcher.dir/Main.cpp.s

CMakeFiles/openCLDispatcher.dir/Main.cpp.o.requires:

.PHONY : CMakeFiles/openCLDispatcher.dir/Main.cpp.o.requires

CMakeFiles/openCLDispatcher.dir/Main.cpp.o.provides: CMakeFiles/openCLDispatcher.dir/Main.cpp.o.requires
	$(MAKE) -f CMakeFiles/openCLDispatcher.dir/build.make CMakeFiles/openCLDispatcher.dir/Main.cpp.o.provides.build
.PHONY : CMakeFiles/openCLDispatcher.dir/Main.cpp.o.provides

CMakeFiles/openCLDispatcher.dir/Main.cpp.o.provides.build: CMakeFiles/openCLDispatcher.dir/Main.cpp.o


# Object files for target openCLDispatcher
openCLDispatcher_OBJECTS = \
"CMakeFiles/openCLDispatcher.dir/Main.cpp.o"

# External object files for target openCLDispatcher
openCLDispatcher_EXTERNAL_OBJECTS =

openCLDispatcher: CMakeFiles/openCLDispatcher.dir/Main.cpp.o
openCLDispatcher: CMakeFiles/openCLDispatcher.dir/build.make
openCLDispatcher: /usr/lib/libOpenCL.so
openCLDispatcher: CMakeFiles/openCLDispatcher.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/tompi/MY_WORK_ZONE/Workspace/GitHubProject/DispatcherOpenCL/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable openCLDispatcher"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/openCLDispatcher.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/openCLDispatcher.dir/build: openCLDispatcher

.PHONY : CMakeFiles/openCLDispatcher.dir/build

CMakeFiles/openCLDispatcher.dir/requires: CMakeFiles/openCLDispatcher.dir/Main.cpp.o.requires

.PHONY : CMakeFiles/openCLDispatcher.dir/requires

CMakeFiles/openCLDispatcher.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/openCLDispatcher.dir/cmake_clean.cmake
.PHONY : CMakeFiles/openCLDispatcher.dir/clean

CMakeFiles/openCLDispatcher.dir/depend:
	cd /home/tompi/MY_WORK_ZONE/Workspace/GitHubProject/DispatcherOpenCL/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tompi/MY_WORK_ZONE/Workspace/GitHubProject/DispatcherOpenCL /home/tompi/MY_WORK_ZONE/Workspace/GitHubProject/DispatcherOpenCL /home/tompi/MY_WORK_ZONE/Workspace/GitHubProject/DispatcherOpenCL/cmake-build-debug /home/tompi/MY_WORK_ZONE/Workspace/GitHubProject/DispatcherOpenCL/cmake-build-debug /home/tompi/MY_WORK_ZONE/Workspace/GitHubProject/DispatcherOpenCL/cmake-build-debug/CMakeFiles/openCLDispatcher.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/openCLDispatcher.dir/depend

