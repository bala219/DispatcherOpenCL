# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

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
CMAKE_COMMAND = /home/gurumurt/clion-2017.3.2/bin/cmake/bin/cmake

# The command to remove a file.
RM = /home/gurumurt/clion-2017.3.2/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/gurumurt/Phd/HeroDB/SQL_flow/db-engine-paradigms

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/gurumurt/Phd/HeroDB/SQL_flow/db-engine-paradigms

# Include any dependencies generated for this target.
include CMakeFiles/benchmark_config.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/benchmark_config.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/benchmark_config.dir/flags.make

CMakeFiles/benchmark_config.dir/src/benchmarks/config.cpp.o: CMakeFiles/benchmark_config.dir/flags.make
CMakeFiles/benchmark_config.dir/src/benchmarks/config.cpp.o: src/benchmarks/config.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gurumurt/Phd/HeroDB/SQL_flow/db-engine-paradigms/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/benchmark_config.dir/src/benchmarks/config.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/benchmark_config.dir/src/benchmarks/config.cpp.o -c /home/gurumurt/Phd/HeroDB/SQL_flow/db-engine-paradigms/src/benchmarks/config.cpp

CMakeFiles/benchmark_config.dir/src/benchmarks/config.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/benchmark_config.dir/src/benchmarks/config.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gurumurt/Phd/HeroDB/SQL_flow/db-engine-paradigms/src/benchmarks/config.cpp > CMakeFiles/benchmark_config.dir/src/benchmarks/config.cpp.i

CMakeFiles/benchmark_config.dir/src/benchmarks/config.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/benchmark_config.dir/src/benchmarks/config.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gurumurt/Phd/HeroDB/SQL_flow/db-engine-paradigms/src/benchmarks/config.cpp -o CMakeFiles/benchmark_config.dir/src/benchmarks/config.cpp.s

CMakeFiles/benchmark_config.dir/src/benchmarks/config.cpp.o.requires:

.PHONY : CMakeFiles/benchmark_config.dir/src/benchmarks/config.cpp.o.requires

CMakeFiles/benchmark_config.dir/src/benchmarks/config.cpp.o.provides: CMakeFiles/benchmark_config.dir/src/benchmarks/config.cpp.o.requires
	$(MAKE) -f CMakeFiles/benchmark_config.dir/build.make CMakeFiles/benchmark_config.dir/src/benchmarks/config.cpp.o.provides.build
.PHONY : CMakeFiles/benchmark_config.dir/src/benchmarks/config.cpp.o.provides

CMakeFiles/benchmark_config.dir/src/benchmarks/config.cpp.o.provides.build: CMakeFiles/benchmark_config.dir/src/benchmarks/config.cpp.o


# Object files for target benchmark_config
benchmark_config_OBJECTS = \
"CMakeFiles/benchmark_config.dir/src/benchmarks/config.cpp.o"

# External object files for target benchmark_config
benchmark_config_EXTERNAL_OBJECTS =

libbenchmark_config.a: CMakeFiles/benchmark_config.dir/src/benchmarks/config.cpp.o
libbenchmark_config.a: CMakeFiles/benchmark_config.dir/build.make
libbenchmark_config.a: CMakeFiles/benchmark_config.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/gurumurt/Phd/HeroDB/SQL_flow/db-engine-paradigms/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libbenchmark_config.a"
	$(CMAKE_COMMAND) -P CMakeFiles/benchmark_config.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/benchmark_config.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/benchmark_config.dir/build: libbenchmark_config.a

.PHONY : CMakeFiles/benchmark_config.dir/build

CMakeFiles/benchmark_config.dir/requires: CMakeFiles/benchmark_config.dir/src/benchmarks/config.cpp.o.requires

.PHONY : CMakeFiles/benchmark_config.dir/requires

CMakeFiles/benchmark_config.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/benchmark_config.dir/cmake_clean.cmake
.PHONY : CMakeFiles/benchmark_config.dir/clean

CMakeFiles/benchmark_config.dir/depend:
	cd /home/gurumurt/Phd/HeroDB/SQL_flow/db-engine-paradigms && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gurumurt/Phd/HeroDB/SQL_flow/db-engine-paradigms /home/gurumurt/Phd/HeroDB/SQL_flow/db-engine-paradigms /home/gurumurt/Phd/HeroDB/SQL_flow/db-engine-paradigms /home/gurumurt/Phd/HeroDB/SQL_flow/db-engine-paradigms /home/gurumurt/Phd/HeroDB/SQL_flow/db-engine-paradigms/CMakeFiles/benchmark_config.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/benchmark_config.dir/depend

