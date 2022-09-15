# CMakeLists.txt
#
# Build files for the RasterMon program.
#
# Author: Maurik Holtrop
#
cmake_minimum_required(VERSION 3.5)
project(RasterMon VERSION 0.1 LANGUAGES CXX)
set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY True)

set(CMAKE_CXX_STANDARD 17)


# include(cmake/FindElog.cmake)   ## ELOG is *dead* for C++17 code. Blame OPS for being 11 years behind in C++.

if(${ELOG_FOUND})
    message(STATUS "The elog library was found at ${ELOG_LIBRARIES} + ${ELOG_INCLUDE_DIR}")
    set(CMAKE_CXX_FLAGS "-Wno-deprecated-declarations -fpermissive")
endif()

# find_package(EvioTool CONFIG)  # EvioTool will also find fEvio.

if(${EvioTool_FOUND})
    message(STATUS "Found an existing installation of EvioTool. Will use that.")
    get_target_property(EvioTool_DIR EvioTool INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "EvioTool found at ${EvioTool_DIR}")
    get_target_property(evio_DIR evio INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "evio found at ${evio_DIR}")
    get_target_property(et_DIR et INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "et found at ${et_DIR}")
    #
    # The RootMacros.cmake is broken. It adds the INCLUDE_DIRECTORIES for a target, not the INTERFACE_INCLUDE_DIRECTORIES
    #
    set_target_properties(EvioTool PROPERTIES INCLUDE_DIRECTORIES ${EvioTool_DIR})
    set_target_properties(evio PROPERTIES INCLUDE_DIRECTORIES ${evio_DIR})
    set_target_properties(et PROPERTIES INCLUDE_DIRECTORIES ${et_DIR})

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
#
# Some items need control.
#
if (NOT DEFINED DEFAULT_HISTOGRAM_PATH)
    set(DEFAULT_HISTOGRAM_PATH "/home/clasrun/raster")
endif()
message(STATUS "DEFAULT_HISTOGRAM_PATH set to ${DEFAULT_HISTOGRAM_PATH}")
if (NOT DEFINED CLI_LOGENTRY_PROGRAM)
    set(CLI_LOGENTRY_PROGRAM "/home/clasrun/RasterMon/logentry")
endif()
message(STATUS "CLI_LOGENTRY_PROGRAM set to ${CLI_LOGENTRY_PROGRAM}")

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

# For dictionary compile step to find include files.
# The target properties for include directories need to be explicitly added to the include_directories.
include_directories(${PROJECT_SOURCE_DIR})

if(${ELOG_FOUND})
    include_directories(${ELOG_INCLUDE_DIR})
endif()

#
# "Proper" ROOT location for the ROOT_GENERATE_DICTIONARY is *after* the add_library(...) statement.
# For some very odd reason (um, bug?) this does not allow root_cling to parse the define of the
# DEFAULT_HISTOGRAM_PATH, stripping the quotes of the path.
#
ROOT_GENERATE_DICTIONARY(RasterLib_dict RasterLib.h DEPENDENCIES EvioTool evio et LINKDEF RasterLibLinkDef.h MODULE RasterLib)
list(APPEND ADDITIONAL_CLEAN_FILES RasterLib_dict.cxx)


## TODO: Install the header files.

set(SOURCE_FILES
        RasterMonGui.cxx
        RasterHists.cxx
        RasterEvioTool.cxx
        RasterMonEventInfo.cxx
        RasterLogBookEntry.cxx
        RasterMonConfigPanel.cxx
        RasterMonDefaultConfig.cxx
        RasterMonConfigInfo.cxx
        RasterScalerBank.cxx
        RasterHelicity.cxx)

add_library(RasterLib SHARED ${SOURCE_FILES} RasterLib_dict.cxx)

target_compile_definitions(RasterLib PRIVATE "DEFAULT_HISTOGRAM_PATH=\"${DEFAULT_HISTOGRAM_PATH}\"")
target_compile_definitions(RasterLib PRIVATE "CLI_LOGENTRY_PROGRAM=\"${CLI_LOGENTRY_PROGRAM}\"")

if(${ELOG_FOUND})
    message(STATUS "Setting HAS_LOGBOOK=1")

    set_source_files_properties(ELogBookShim.cxx PROPERTIES COMPILE_FLAGS --std=c++11)
    add_library(ELogBookShim STATIC ELogBookShim.cxx )
    target_link_libraries(ELogBookShim elog)
    install(TARGETS ELogBookShim LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

    target_compile_definitions(RasterLib PRIVATE "HAS_LOGBOOK=1")
    target_link_libraries(RasterLib ${ROOT_LIBRARIES} EvioTool evio et ELogBookShim elog)
else()
    message(STATUS "WARNING: Direct logbook entry will not be enabled. Code will still work using cli logentry.")
    target_link_libraries(RasterLib ${ROOT_LIBRARIES} EvioTool evio et)
endif()

add_executable(RasterMon RasterMon.cxx)
target_link_libraries(RasterMon RasterLib )

add_executable(parse_test ParseTest.cxx)
target_link_libraries(parse_test RasterLib)

install(TARGETS RasterLib  LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(FILES ${PROJECT_BINARY_DIR}/libRasterLib_rdict.pcm  DESTINATION lib)
install(TARGETS RasterMon DESTINATION bin)
