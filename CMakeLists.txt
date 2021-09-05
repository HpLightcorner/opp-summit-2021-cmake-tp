cmake_minimum_required(VERSION 3.20)

#
# Boilerplate CMake Commands
cmake_policy(VERSION 3.20)
cmake_policy(SET CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
# Options
option(BUILD_INET "Enable example build of INET" OFF)

#
# The Project name
project(tictoc)

#
# Include CMake-Extensions as needed
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/omnetpp)

#
# If using LLVM (Clang), force-switch to lld (speedup linking)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Setting linker to lld.")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fuse-ld=lld")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=lld")
endif()

#
# Ensure standalone debug symbols for debugging OMNeT++, INET...
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    message(STATUS "Building for standalone debugging.")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstandalone-debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstandalone-debug")
endif()

#
# Create the debug/run target and launch.json settings vor VS-Code
set_property(GLOBAL PROPERTY VSCODE_ADDON TRUE)
set(VSCODE_DEBUG_CONFIG GDB)

# 
# find OMNeT++ (on Windows, make sure to select corresponding toolchains)
find_package(OmnetPP 6.0 REQUIRED)
include(OmnetppHelpers)

# ======================================================================
# Build the overall src folder as an OMNeT++ target
# Some minimal rules apply to work for this example:
# - C++ files must end with .cc
# - Messages files must end with .msg
# - Files must be placed inside a src folder and meaningful sub-folders (e.g. src/tictoc)
# - Include statements for generated messages must be relative to the source-root (e.g. #include "tictoc/my-message_m.h")
# ======================================================================
add_opp_target(TARGET ${PROJECT_NAME} ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Add the run/debug configuration
add_opp_run(run-${PROJECT_NAME}
    WORKING_DIRECTORY ./run/
    DEPENDENCY ${PROJECT_NAME}
)

#
# Link with Simulation-DLL
target_link_libraries(${PROJECT_NAME} PUBLIC 
    OmnetPP::sim 
)

# ======================================================================
# Explicitly build TicToc example version 18 (with generating messages)
# and adding a CMake based dependency (spdlog)
#
# using .cpp as file extensions such that the file gets ignored by add_opp_target
# ======================================================================

#
# The actual MQTT-RT Server linkes as a DLL to run within OMNeT++
add_library(${PROJECT_NAME}_18 SHARED 
    ./src/tictoc/txc18_spdlog.cpp
)

#
# Set the corresponding NED-Folder property
set_target_properties(${PROJECT_NAME}_18 PROPERTIES NED_FOLDERS ./src)

#
# Generate messages and add them to the target
generate_opp_message(./src/tictoc/tictoc18.msg TARGET ${PROJECT_NAME}_18)

#
# Add CMake based dependency (spdlog)
add_subdirectory(externals/spdlog)

#
# Link with Simulation-DLL
target_link_libraries(${PROJECT_NAME}_18 PUBLIC 
    OmnetPP::sim 
    spdlog::spdlog
)

add_opp_run(run-${PROJECT_NAME}-18
    WORKING_DIRECTORY ./run/
    DEPENDENCY ${PROJECT_NAME}_18
)

# ======================================================================
# For completeness, build and link a very complex dependency - INET
# ======================================================================
if (BUILD_INET)

find_package(Python COMPONENTS Interpreter)

find_path(INET_DIR NAMES src/inet/package.ned PATHS externals/inet DOC "INET root directory")
set(INET_GEN_DIR ${PROJECT_BINARY_DIR}/INET_gen)
file(MAKE_DIRECTORY ${INET_GEN_DIR}/inet)

# collect INET's opp_makemake options
execute_process(COMMAND ${Python_EXECUTABLE} ${OMNETPP_FEATURETOOL} options -l -f
    WORKING_DIRECTORY ${INET_DIR}
    OUTPUT_VARIABLE INET_OPP_MAKEMAKE
    COMMAND_ECHO STDOUT 
    ECHO_OUTPUT_VARIABLE
)
separate_arguments(INET_OPP_MAKEMAKE NATIVE_COMMAND ${INET_OPP_MAKEMAKE})
message(DEBUG "INET-OPP MakeMake: " ${INET_OPP_MAKEMAKE})

# generate INET's features and opp_defines header
set(INET_GEN_DIR ${PROJECT_BINARY_DIR}/INET_gen)
file(MAKE_DIRECTORY ${INET_GEN_DIR}/inet)

add_custom_command(OUTPUT ${INET_GEN_DIR}/inet/features.h
    COMMAND ${Python_EXECUTABLE} ${OMNETPP_FEATURETOOL} defines > ${INET_GEN_DIR}/inet/features.h
    DEPENDS ${INET_DIR}/.oppfeaturestate
    WORKING_DIRECTORY ${INET_DIR}
    COMMENT "Generate INET features.h"
)

add_custom_command(OUTPUT ${INET_GEN_DIR}/inet/opp_defines.h
    COMMAND ${CMAKE_COMMAND} -E touch ${INET_GEN_DIR}/inet/opp_defines.h
    COMMENT "Generate INET opp_defines.h"
)

add_opp_target(TARGET INET ROOT_DIR ${INET_DIR}
    DEPENDS ${INET_GEN_DIR}/inet/features.h ${INET_GEN_DIR}/inet/opp_defines.h
    OPP_MAKEMAKE ${INET_OPP_MAKEMAKE}
)

target_compile_definitions(INET PUBLIC INET_EXPORT)

target_link_libraries(INET PUBLIC 
    OmnetPP::common
    OmnetPP::envir
    OmnetPP::eventlog
    OmnetPP::layout
    OmnetPP::main
    OmnetPP::nedxml
    OmnetPP::scave
    OmnetPP::sim
)

if(WIN32)
    target_link_libraries(INET PUBLIC 
        ws2_32
    )
endif()

endif()