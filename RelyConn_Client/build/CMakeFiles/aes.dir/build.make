# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/rick/Desktop/repos/RelyConn/RelyConn_Client

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/rick/Desktop/repos/RelyConn/RelyConn_Client/build

# Include any dependencies generated for this target.
include CMakeFiles/aes.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/aes.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/aes.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/aes.dir/flags.make

CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/aes.c.o: CMakeFiles/aes.dir/flags.make
CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/aes.c.o: /home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/aes.c
CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/aes.c.o: CMakeFiles/aes.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rick/Desktop/repos/RelyConn/RelyConn_Client/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/aes.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/aes.c.o -MF CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/aes.c.o.d -o CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/aes.c.o -c /home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/aes.c

CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/aes.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/aes.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/aes.c > CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/aes.c.i

CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/aes.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/aes.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/aes.c -o CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/aes.c.s

CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/test.c.o: CMakeFiles/aes.dir/flags.make
CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/test.c.o: /home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/test.c
CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/test.c.o: CMakeFiles/aes.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rick/Desktop/repos/RelyConn/RelyConn_Client/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/test.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/test.c.o -MF CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/test.c.o.d -o CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/test.c.o -c /home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/test.c

CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/test.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/test.c > CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/test.c.i

CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/test.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/test.c -o CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/test.c.s

# Object files for target aes
aes_OBJECTS = \
"CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/aes.c.o" \
"CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/test.c.o"

# External object files for target aes
aes_EXTERNAL_OBJECTS =

libaes.a: CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/aes.c.o
libaes.a: CMakeFiles/aes.dir/home/rick/Desktop/repos/RelyConn/externals/tiny-AES-c-master/test.c.o
libaes.a: CMakeFiles/aes.dir/build.make
libaes.a: CMakeFiles/aes.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/rick/Desktop/repos/RelyConn/RelyConn_Client/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C static library libaes.a"
	$(CMAKE_COMMAND) -P CMakeFiles/aes.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/aes.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/aes.dir/build: libaes.a
.PHONY : CMakeFiles/aes.dir/build

CMakeFiles/aes.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/aes.dir/cmake_clean.cmake
.PHONY : CMakeFiles/aes.dir/clean

CMakeFiles/aes.dir/depend:
	cd /home/rick/Desktop/repos/RelyConn/RelyConn_Client/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rick/Desktop/repos/RelyConn/RelyConn_Client /home/rick/Desktop/repos/RelyConn/RelyConn_Client /home/rick/Desktop/repos/RelyConn/RelyConn_Client/build /home/rick/Desktop/repos/RelyConn/RelyConn_Client/build /home/rick/Desktop/repos/RelyConn/RelyConn_Client/build/CMakeFiles/aes.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/aes.dir/depend

