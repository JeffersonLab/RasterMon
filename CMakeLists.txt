# CMakeLists.txt
#
# Build files for the RasterMon program.
#
# Author: Maurik Holtrop
#
cmake_minimum_required(VERSION 3.5)
project(RasterMon)

#

find_package(EvioTool CONFIG)  # EvioTool will also find fEvio.

if(${EvioTool_FOUND})
    include($ENV{HOME}/cmake/cmake_debug_properties.cmake)
    print_target_properties(EvioTool)

    get_target_property(EvioTool_DIR EvioTool INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "EvioTool found at ${EvioTool_DIR}")
else()
    message(STATUS "EvioTool not found. Will build locally.")
    add_subdirectory(EvioTool)
endif()

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
# This *must* come after EvioTool, because EvioTool imports ROOT without the Gui component.

find_package(ROOT REQUIRED COMPONENTS Core Gui)
message(STATUS "ROOT found at: ${ROOT_DIR}")

set(default_build_type "RELEASE")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
                 "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

# For dictionary compile step to find include files.
# The target properties for include directories need to be explicitly added to the include_directories.
include_directories(${PROJECT_SOURCE_DIR})

ROOT_GENERATE_DICTIONARY(RasterLib_dict RasterLib.h DEPENDENCIES EvioTool LINKDEF RasterLibLinkDef.h)

set(SOURCE_FILES
        RasterMonGui.cxx
        RasterHists.cxx
        RasterEvioTool.cxx
        RasterMonEventInfo.cxx
        )

add_library(RasterLib SHARED ${SOURCE_FILES} RasterLib_dict.cxx)
target_link_libraries(RasterLib ${ROOT_LIBRARIES} EvioTool evio)
add_executable(RasterMon RasterMon.cxx)
target_link_libraries(RasterMon RasterLib )

add_executable(test test_evio.cpp)
target_link_libraries(test RasterLib)
add_executable(test_destructor test_destructor.cpp)
target_link_libraries(test_destructor RasterLib)


install(TARGETS RasterLib  LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(FILES ${PROJECT_BINARY_DIR}/libRasterLib_dict_rdict.pcm  DESTINATION lib)
install(TARGETS RasterMon DESTINATION bin)
