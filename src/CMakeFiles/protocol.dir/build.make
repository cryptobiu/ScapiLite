# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_SOURCE_DIR = /home/dudi/ScapiLite/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dudi/ScapiLite/src

# Include any dependencies generated for this target.
include CMakeFiles/protocol.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/protocol.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/protocol.dir/flags.make

CMakeFiles/protocol.dir/Prg.cpp.o: CMakeFiles/protocol.dir/flags.make
CMakeFiles/protocol.dir/Prg.cpp.o: Prg.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dudi/ScapiLite/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/protocol.dir/Prg.cpp.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/protocol.dir/Prg.cpp.o -c /home/dudi/ScapiLite/src/Prg.cpp

CMakeFiles/protocol.dir/Prg.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protocol.dir/Prg.cpp.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dudi/ScapiLite/src/Prg.cpp > CMakeFiles/protocol.dir/Prg.cpp.i

CMakeFiles/protocol.dir/Prg.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protocol.dir/Prg.cpp.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dudi/ScapiLite/src/Prg.cpp -o CMakeFiles/protocol.dir/Prg.cpp.s

CMakeFiles/protocol.dir/Prg.cpp.o.requires:

.PHONY : CMakeFiles/protocol.dir/Prg.cpp.o.requires

CMakeFiles/protocol.dir/Prg.cpp.o.provides: CMakeFiles/protocol.dir/Prg.cpp.o.requires
	$(MAKE) -f CMakeFiles/protocol.dir/build.make CMakeFiles/protocol.dir/Prg.cpp.o.provides.build
.PHONY : CMakeFiles/protocol.dir/Prg.cpp.o.provides

CMakeFiles/protocol.dir/Prg.cpp.o.provides.build: CMakeFiles/protocol.dir/Prg.cpp.o


CMakeFiles/protocol.dir/aes.cpp.o: CMakeFiles/protocol.dir/flags.make
CMakeFiles/protocol.dir/aes.cpp.o: aes.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dudi/ScapiLite/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/protocol.dir/aes.cpp.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/protocol.dir/aes.cpp.o -c /home/dudi/ScapiLite/src/aes.cpp

CMakeFiles/protocol.dir/aes.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protocol.dir/aes.cpp.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dudi/ScapiLite/src/aes.cpp > CMakeFiles/protocol.dir/aes.cpp.i

CMakeFiles/protocol.dir/aes.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protocol.dir/aes.cpp.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dudi/ScapiLite/src/aes.cpp -o CMakeFiles/protocol.dir/aes.cpp.s

CMakeFiles/protocol.dir/aes.cpp.o.requires:

.PHONY : CMakeFiles/protocol.dir/aes.cpp.o.requires

CMakeFiles/protocol.dir/aes.cpp.o.provides: CMakeFiles/protocol.dir/aes.cpp.o.requires
	$(MAKE) -f CMakeFiles/protocol.dir/build.make CMakeFiles/protocol.dir/aes.cpp.o.provides.build
.PHONY : CMakeFiles/protocol.dir/aes.cpp.o.provides

CMakeFiles/protocol.dir/aes.cpp.o.provides.build: CMakeFiles/protocol.dir/aes.cpp.o


CMakeFiles/protocol.dir/Common.cpp.o: CMakeFiles/protocol.dir/flags.make
CMakeFiles/protocol.dir/Common.cpp.o: Common.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dudi/ScapiLite/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/protocol.dir/Common.cpp.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/protocol.dir/Common.cpp.o -c /home/dudi/ScapiLite/src/Common.cpp

CMakeFiles/protocol.dir/Common.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protocol.dir/Common.cpp.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dudi/ScapiLite/src/Common.cpp > CMakeFiles/protocol.dir/Common.cpp.i

CMakeFiles/protocol.dir/Common.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protocol.dir/Common.cpp.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dudi/ScapiLite/src/Common.cpp -o CMakeFiles/protocol.dir/Common.cpp.s

CMakeFiles/protocol.dir/Common.cpp.o.requires:

.PHONY : CMakeFiles/protocol.dir/Common.cpp.o.requires

CMakeFiles/protocol.dir/Common.cpp.o.provides: CMakeFiles/protocol.dir/Common.cpp.o.requires
	$(MAKE) -f CMakeFiles/protocol.dir/build.make CMakeFiles/protocol.dir/Common.cpp.o.provides.build
.PHONY : CMakeFiles/protocol.dir/Common.cpp.o.provides

CMakeFiles/protocol.dir/Common.cpp.o.provides.build: CMakeFiles/protocol.dir/Common.cpp.o


CMakeFiles/protocol.dir/ArithmeticCircuit.cpp.o: CMakeFiles/protocol.dir/flags.make
CMakeFiles/protocol.dir/ArithmeticCircuit.cpp.o: ArithmeticCircuit.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dudi/ScapiLite/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/protocol.dir/ArithmeticCircuit.cpp.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/protocol.dir/ArithmeticCircuit.cpp.o -c /home/dudi/ScapiLite/src/ArithmeticCircuit.cpp

CMakeFiles/protocol.dir/ArithmeticCircuit.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protocol.dir/ArithmeticCircuit.cpp.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dudi/ScapiLite/src/ArithmeticCircuit.cpp > CMakeFiles/protocol.dir/ArithmeticCircuit.cpp.i

CMakeFiles/protocol.dir/ArithmeticCircuit.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protocol.dir/ArithmeticCircuit.cpp.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dudi/ScapiLite/src/ArithmeticCircuit.cpp -o CMakeFiles/protocol.dir/ArithmeticCircuit.cpp.s

CMakeFiles/protocol.dir/ArithmeticCircuit.cpp.o.requires:

.PHONY : CMakeFiles/protocol.dir/ArithmeticCircuit.cpp.o.requires

CMakeFiles/protocol.dir/ArithmeticCircuit.cpp.o.provides: CMakeFiles/protocol.dir/ArithmeticCircuit.cpp.o.requires
	$(MAKE) -f CMakeFiles/protocol.dir/build.make CMakeFiles/protocol.dir/ArithmeticCircuit.cpp.o.provides.build
.PHONY : CMakeFiles/protocol.dir/ArithmeticCircuit.cpp.o.provides

CMakeFiles/protocol.dir/ArithmeticCircuit.cpp.o.provides.build: CMakeFiles/protocol.dir/ArithmeticCircuit.cpp.o


CMakeFiles/protocol.dir/CommBF.cpp.o: CMakeFiles/protocol.dir/flags.make
CMakeFiles/protocol.dir/CommBF.cpp.o: CommBF.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dudi/ScapiLite/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/protocol.dir/CommBF.cpp.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/protocol.dir/CommBF.cpp.o -c /home/dudi/ScapiLite/src/CommBF.cpp

CMakeFiles/protocol.dir/CommBF.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protocol.dir/CommBF.cpp.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dudi/ScapiLite/src/CommBF.cpp > CMakeFiles/protocol.dir/CommBF.cpp.i

CMakeFiles/protocol.dir/CommBF.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protocol.dir/CommBF.cpp.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dudi/ScapiLite/src/CommBF.cpp -o CMakeFiles/protocol.dir/CommBF.cpp.s

CMakeFiles/protocol.dir/CommBF.cpp.o.requires:

.PHONY : CMakeFiles/protocol.dir/CommBF.cpp.o.requires

CMakeFiles/protocol.dir/CommBF.cpp.o.provides: CMakeFiles/protocol.dir/CommBF.cpp.o.requires
	$(MAKE) -f CMakeFiles/protocol.dir/build.make CMakeFiles/protocol.dir/CommBF.cpp.o.provides.build
.PHONY : CMakeFiles/protocol.dir/CommBF.cpp.o.provides

CMakeFiles/protocol.dir/CommBF.cpp.o.provides.build: CMakeFiles/protocol.dir/CommBF.cpp.o


CMakeFiles/protocol.dir/MPCCommunicationBF.cpp.o: CMakeFiles/protocol.dir/flags.make
CMakeFiles/protocol.dir/MPCCommunicationBF.cpp.o: MPCCommunicationBF.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dudi/ScapiLite/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/protocol.dir/MPCCommunicationBF.cpp.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/protocol.dir/MPCCommunicationBF.cpp.o -c /home/dudi/ScapiLite/src/MPCCommunicationBF.cpp

CMakeFiles/protocol.dir/MPCCommunicationBF.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protocol.dir/MPCCommunicationBF.cpp.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dudi/ScapiLite/src/MPCCommunicationBF.cpp > CMakeFiles/protocol.dir/MPCCommunicationBF.cpp.i

CMakeFiles/protocol.dir/MPCCommunicationBF.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protocol.dir/MPCCommunicationBF.cpp.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dudi/ScapiLite/src/MPCCommunicationBF.cpp -o CMakeFiles/protocol.dir/MPCCommunicationBF.cpp.s

CMakeFiles/protocol.dir/MPCCommunicationBF.cpp.o.requires:

.PHONY : CMakeFiles/protocol.dir/MPCCommunicationBF.cpp.o.requires

CMakeFiles/protocol.dir/MPCCommunicationBF.cpp.o.provides: CMakeFiles/protocol.dir/MPCCommunicationBF.cpp.o.requires
	$(MAKE) -f CMakeFiles/protocol.dir/build.make CMakeFiles/protocol.dir/MPCCommunicationBF.cpp.o.provides.build
.PHONY : CMakeFiles/protocol.dir/MPCCommunicationBF.cpp.o.provides

CMakeFiles/protocol.dir/MPCCommunicationBF.cpp.o.provides.build: CMakeFiles/protocol.dir/MPCCommunicationBF.cpp.o


CMakeFiles/protocol.dir/Protocol.cpp.o: CMakeFiles/protocol.dir/flags.make
CMakeFiles/protocol.dir/Protocol.cpp.o: Protocol.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dudi/ScapiLite/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/protocol.dir/Protocol.cpp.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/protocol.dir/Protocol.cpp.o -c /home/dudi/ScapiLite/src/Protocol.cpp

CMakeFiles/protocol.dir/Protocol.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protocol.dir/Protocol.cpp.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dudi/ScapiLite/src/Protocol.cpp > CMakeFiles/protocol.dir/Protocol.cpp.i

CMakeFiles/protocol.dir/Protocol.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protocol.dir/Protocol.cpp.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dudi/ScapiLite/src/Protocol.cpp -o CMakeFiles/protocol.dir/Protocol.cpp.s

CMakeFiles/protocol.dir/Protocol.cpp.o.requires:

.PHONY : CMakeFiles/protocol.dir/Protocol.cpp.o.requires

CMakeFiles/protocol.dir/Protocol.cpp.o.provides: CMakeFiles/protocol.dir/Protocol.cpp.o.requires
	$(MAKE) -f CMakeFiles/protocol.dir/build.make CMakeFiles/protocol.dir/Protocol.cpp.o.provides.build
.PHONY : CMakeFiles/protocol.dir/Protocol.cpp.o.provides

CMakeFiles/protocol.dir/Protocol.cpp.o.provides.build: CMakeFiles/protocol.dir/Protocol.cpp.o


CMakeFiles/protocol.dir/ConfigFile.cpp.o: CMakeFiles/protocol.dir/flags.make
CMakeFiles/protocol.dir/ConfigFile.cpp.o: ConfigFile.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dudi/ScapiLite/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/protocol.dir/ConfigFile.cpp.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/protocol.dir/ConfigFile.cpp.o -c /home/dudi/ScapiLite/src/ConfigFile.cpp

CMakeFiles/protocol.dir/ConfigFile.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protocol.dir/ConfigFile.cpp.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dudi/ScapiLite/src/ConfigFile.cpp > CMakeFiles/protocol.dir/ConfigFile.cpp.i

CMakeFiles/protocol.dir/ConfigFile.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protocol.dir/ConfigFile.cpp.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dudi/ScapiLite/src/ConfigFile.cpp -o CMakeFiles/protocol.dir/ConfigFile.cpp.s

CMakeFiles/protocol.dir/ConfigFile.cpp.o.requires:

.PHONY : CMakeFiles/protocol.dir/ConfigFile.cpp.o.requires

CMakeFiles/protocol.dir/ConfigFile.cpp.o.provides: CMakeFiles/protocol.dir/ConfigFile.cpp.o.requires
	$(MAKE) -f CMakeFiles/protocol.dir/build.make CMakeFiles/protocol.dir/ConfigFile.cpp.o.provides.build
.PHONY : CMakeFiles/protocol.dir/ConfigFile.cpp.o.provides

CMakeFiles/protocol.dir/ConfigFile.cpp.o.provides.build: CMakeFiles/protocol.dir/ConfigFile.cpp.o


CMakeFiles/protocol.dir/TemplateField.cpp.o: CMakeFiles/protocol.dir/flags.make
CMakeFiles/protocol.dir/TemplateField.cpp.o: TemplateField.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dudi/ScapiLite/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/protocol.dir/TemplateField.cpp.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/protocol.dir/TemplateField.cpp.o -c /home/dudi/ScapiLite/src/TemplateField.cpp

CMakeFiles/protocol.dir/TemplateField.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protocol.dir/TemplateField.cpp.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dudi/ScapiLite/src/TemplateField.cpp > CMakeFiles/protocol.dir/TemplateField.cpp.i

CMakeFiles/protocol.dir/TemplateField.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protocol.dir/TemplateField.cpp.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dudi/ScapiLite/src/TemplateField.cpp -o CMakeFiles/protocol.dir/TemplateField.cpp.s

CMakeFiles/protocol.dir/TemplateField.cpp.o.requires:

.PHONY : CMakeFiles/protocol.dir/TemplateField.cpp.o.requires

CMakeFiles/protocol.dir/TemplateField.cpp.o.provides: CMakeFiles/protocol.dir/TemplateField.cpp.o.requires
	$(MAKE) -f CMakeFiles/protocol.dir/build.make CMakeFiles/protocol.dir/TemplateField.cpp.o.provides.build
.PHONY : CMakeFiles/protocol.dir/TemplateField.cpp.o.provides

CMakeFiles/protocol.dir/TemplateField.cpp.o.provides.build: CMakeFiles/protocol.dir/TemplateField.cpp.o


CMakeFiles/protocol.dir/GF2_8LookupTable.cpp.o: CMakeFiles/protocol.dir/flags.make
CMakeFiles/protocol.dir/GF2_8LookupTable.cpp.o: GF2_8LookupTable.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dudi/ScapiLite/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/protocol.dir/GF2_8LookupTable.cpp.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/protocol.dir/GF2_8LookupTable.cpp.o -c /home/dudi/ScapiLite/src/GF2_8LookupTable.cpp

CMakeFiles/protocol.dir/GF2_8LookupTable.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protocol.dir/GF2_8LookupTable.cpp.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dudi/ScapiLite/src/GF2_8LookupTable.cpp > CMakeFiles/protocol.dir/GF2_8LookupTable.cpp.i

CMakeFiles/protocol.dir/GF2_8LookupTable.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protocol.dir/GF2_8LookupTable.cpp.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dudi/ScapiLite/src/GF2_8LookupTable.cpp -o CMakeFiles/protocol.dir/GF2_8LookupTable.cpp.s

CMakeFiles/protocol.dir/GF2_8LookupTable.cpp.o.requires:

.PHONY : CMakeFiles/protocol.dir/GF2_8LookupTable.cpp.o.requires

CMakeFiles/protocol.dir/GF2_8LookupTable.cpp.o.provides: CMakeFiles/protocol.dir/GF2_8LookupTable.cpp.o.requires
	$(MAKE) -f CMakeFiles/protocol.dir/build.make CMakeFiles/protocol.dir/GF2_8LookupTable.cpp.o.provides.build
.PHONY : CMakeFiles/protocol.dir/GF2_8LookupTable.cpp.o.provides

CMakeFiles/protocol.dir/GF2_8LookupTable.cpp.o.provides.build: CMakeFiles/protocol.dir/GF2_8LookupTable.cpp.o


CMakeFiles/protocol.dir/main.cpp.o: CMakeFiles/protocol.dir/flags.make
CMakeFiles/protocol.dir/main.cpp.o: main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dudi/ScapiLite/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/protocol.dir/main.cpp.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/protocol.dir/main.cpp.o -c /home/dudi/ScapiLite/src/main.cpp

CMakeFiles/protocol.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protocol.dir/main.cpp.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dudi/ScapiLite/src/main.cpp > CMakeFiles/protocol.dir/main.cpp.i

CMakeFiles/protocol.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protocol.dir/main.cpp.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dudi/ScapiLite/src/main.cpp -o CMakeFiles/protocol.dir/main.cpp.s

CMakeFiles/protocol.dir/main.cpp.o.requires:

.PHONY : CMakeFiles/protocol.dir/main.cpp.o.requires

CMakeFiles/protocol.dir/main.cpp.o.provides: CMakeFiles/protocol.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/protocol.dir/build.make CMakeFiles/protocol.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/protocol.dir/main.cpp.o.provides

CMakeFiles/protocol.dir/main.cpp.o.provides.build: CMakeFiles/protocol.dir/main.cpp.o


# Object files for target protocol
protocol_OBJECTS = \
"CMakeFiles/protocol.dir/Prg.cpp.o" \
"CMakeFiles/protocol.dir/aes.cpp.o" \
"CMakeFiles/protocol.dir/Common.cpp.o" \
"CMakeFiles/protocol.dir/ArithmeticCircuit.cpp.o" \
"CMakeFiles/protocol.dir/CommBF.cpp.o" \
"CMakeFiles/protocol.dir/MPCCommunicationBF.cpp.o" \
"CMakeFiles/protocol.dir/Protocol.cpp.o" \
"CMakeFiles/protocol.dir/ConfigFile.cpp.o" \
"CMakeFiles/protocol.dir/TemplateField.cpp.o" \
"CMakeFiles/protocol.dir/GF2_8LookupTable.cpp.o" \
"CMakeFiles/protocol.dir/main.cpp.o"

# External object files for target protocol
protocol_EXTERNAL_OBJECTS =

protocol: CMakeFiles/protocol.dir/Prg.cpp.o
protocol: CMakeFiles/protocol.dir/aes.cpp.o
protocol: CMakeFiles/protocol.dir/Common.cpp.o
protocol: CMakeFiles/protocol.dir/ArithmeticCircuit.cpp.o
protocol: CMakeFiles/protocol.dir/CommBF.cpp.o
protocol: CMakeFiles/protocol.dir/MPCCommunicationBF.cpp.o
protocol: CMakeFiles/protocol.dir/Protocol.cpp.o
protocol: CMakeFiles/protocol.dir/ConfigFile.cpp.o
protocol: CMakeFiles/protocol.dir/TemplateField.cpp.o
protocol: CMakeFiles/protocol.dir/GF2_8LookupTable.cpp.o
protocol: CMakeFiles/protocol.dir/main.cpp.o
protocol: CMakeFiles/protocol.dir/build.make
protocol: CMakeFiles/protocol.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/dudi/ScapiLite/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Linking CXX executable protocol"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/protocol.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/protocol.dir/build: protocol

.PHONY : CMakeFiles/protocol.dir/build

CMakeFiles/protocol.dir/requires: CMakeFiles/protocol.dir/Prg.cpp.o.requires
CMakeFiles/protocol.dir/requires: CMakeFiles/protocol.dir/aes.cpp.o.requires
CMakeFiles/protocol.dir/requires: CMakeFiles/protocol.dir/Common.cpp.o.requires
CMakeFiles/protocol.dir/requires: CMakeFiles/protocol.dir/ArithmeticCircuit.cpp.o.requires
CMakeFiles/protocol.dir/requires: CMakeFiles/protocol.dir/CommBF.cpp.o.requires
CMakeFiles/protocol.dir/requires: CMakeFiles/protocol.dir/MPCCommunicationBF.cpp.o.requires
CMakeFiles/protocol.dir/requires: CMakeFiles/protocol.dir/Protocol.cpp.o.requires
CMakeFiles/protocol.dir/requires: CMakeFiles/protocol.dir/ConfigFile.cpp.o.requires
CMakeFiles/protocol.dir/requires: CMakeFiles/protocol.dir/TemplateField.cpp.o.requires
CMakeFiles/protocol.dir/requires: CMakeFiles/protocol.dir/GF2_8LookupTable.cpp.o.requires
CMakeFiles/protocol.dir/requires: CMakeFiles/protocol.dir/main.cpp.o.requires

.PHONY : CMakeFiles/protocol.dir/requires

CMakeFiles/protocol.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/protocol.dir/cmake_clean.cmake
.PHONY : CMakeFiles/protocol.dir/clean

CMakeFiles/protocol.dir/depend:
	cd /home/dudi/ScapiLite/src && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dudi/ScapiLite/src /home/dudi/ScapiLite/src /home/dudi/ScapiLite/src /home/dudi/ScapiLite/src /home/dudi/ScapiLite/src/CMakeFiles/protocol.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/protocol.dir/depend

