# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.11

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/home/sul/DemoLocal/Full dynamic connectivity"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/sul/DemoLocal/Full dynamic connectivity/cmake-build-debug"

# Include any dependencies generated for this target.
include CMakeFiles/Dzen.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Dzen.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Dzen.dir/flags.make

CMakeFiles/Dzen.dir/main.cpp.o: CMakeFiles/Dzen.dir/flags.make
CMakeFiles/Dzen.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/sul/DemoLocal/Full dynamic connectivity/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Dzen.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Dzen.dir/main.cpp.o -c "/home/sul/DemoLocal/Full dynamic connectivity/main.cpp"

CMakeFiles/Dzen.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Dzen.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/sul/DemoLocal/Full dynamic connectivity/main.cpp" > CMakeFiles/Dzen.dir/main.cpp.i

CMakeFiles/Dzen.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Dzen.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/sul/DemoLocal/Full dynamic connectivity/main.cpp" -o CMakeFiles/Dzen.dir/main.cpp.s

CMakeFiles/Dzen.dir/dynamic_graph.cpp.o: CMakeFiles/Dzen.dir/flags.make
CMakeFiles/Dzen.dir/dynamic_graph.cpp.o: ../dynamic_graph.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/sul/DemoLocal/Full dynamic connectivity/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/Dzen.dir/dynamic_graph.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Dzen.dir/dynamic_graph.cpp.o -c "/home/sul/DemoLocal/Full dynamic connectivity/dynamic_graph.cpp"

CMakeFiles/Dzen.dir/dynamic_graph.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Dzen.dir/dynamic_graph.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/sul/DemoLocal/Full dynamic connectivity/dynamic_graph.cpp" > CMakeFiles/Dzen.dir/dynamic_graph.cpp.i

CMakeFiles/Dzen.dir/dynamic_graph.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Dzen.dir/dynamic_graph.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/sul/DemoLocal/Full dynamic connectivity/dynamic_graph.cpp" -o CMakeFiles/Dzen.dir/dynamic_graph.cpp.s

CMakeFiles/Dzen.dir/et_forest.cpp.o: CMakeFiles/Dzen.dir/flags.make
CMakeFiles/Dzen.dir/et_forest.cpp.o: ../et_forest.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/sul/DemoLocal/Full dynamic connectivity/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/Dzen.dir/et_forest.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Dzen.dir/et_forest.cpp.o -c "/home/sul/DemoLocal/Full dynamic connectivity/et_forest.cpp"

CMakeFiles/Dzen.dir/et_forest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Dzen.dir/et_forest.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/sul/DemoLocal/Full dynamic connectivity/et_forest.cpp" > CMakeFiles/Dzen.dir/et_forest.cpp.i

CMakeFiles/Dzen.dir/et_forest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Dzen.dir/et_forest.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/sul/DemoLocal/Full dynamic connectivity/et_forest.cpp" -o CMakeFiles/Dzen.dir/et_forest.cpp.s

# Object files for target Dzen
Dzen_OBJECTS = \
"CMakeFiles/Dzen.dir/main.cpp.o" \
"CMakeFiles/Dzen.dir/dynamic_graph.cpp.o" \
"CMakeFiles/Dzen.dir/et_forest.cpp.o"

# External object files for target Dzen
Dzen_EXTERNAL_OBJECTS =

Dzen: CMakeFiles/Dzen.dir/main.cpp.o
Dzen: CMakeFiles/Dzen.dir/dynamic_graph.cpp.o
Dzen: CMakeFiles/Dzen.dir/et_forest.cpp.o
Dzen: CMakeFiles/Dzen.dir/build.make
Dzen: CMakeFiles/Dzen.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/home/sul/DemoLocal/Full dynamic connectivity/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable Dzen"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Dzen.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Dzen.dir/build: Dzen

.PHONY : CMakeFiles/Dzen.dir/build

CMakeFiles/Dzen.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Dzen.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Dzen.dir/clean

CMakeFiles/Dzen.dir/depend:
	cd "/home/sul/DemoLocal/Full dynamic connectivity/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/sul/DemoLocal/Full dynamic connectivity" "/home/sul/DemoLocal/Full dynamic connectivity" "/home/sul/DemoLocal/Full dynamic connectivity/cmake-build-debug" "/home/sul/DemoLocal/Full dynamic connectivity/cmake-build-debug" "/home/sul/DemoLocal/Full dynamic connectivity/cmake-build-debug/CMakeFiles/Dzen.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/Dzen.dir/depend
