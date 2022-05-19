#
# Try to find the elusive, nearly non existent and poorly supported elog C++ API.
#
if(NOT ELOG_INCLUDE_DIR)
    find_path(ELOG_INCLUDE_DIR NAME elog.h HINTS /apps/acctools/cs/certified/apps/eloglib/2.5/inc eloglib/include elog)
endif()

if(NOT ELOG_LIBRARY)
    find_library(ELOG_LIBRARY NAMES libelog.a HINTS /apps/acctools/cs/certified/apps/eloglib/2.5/lib/rhel-7-x86_64 eloglib/lib elog)
endif()

mark_as_advanced(ELOG_INCLUDE_DIR ELOG_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Elog FOUND_VAR ELOG_FOUND
        REQUIRED_VARS ELOG_INCLUDE_DIR ELOG_LIBRARY)

if(ELOG_FOUND)
    set(ELOG_INCLUDE_DIRS ${VDT_INCLUDE_DIR})

    if(NOT ELOG_LIBRARIES)
        set(ELOG_LIBRARIES ${ELOG_LIBRARY})
    endif()

    if(NOT TARGET ELOG::ELOG)
        add_library(ELOG::ELOG UNKNOWN IMPORTED)

        set_target_properties(ELOG::ELOG
                PROPERTIES
                IMPORTED_LOCATION "${ELOG_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${ELOG_INCLUDE_DIRS}"
                )
    endif()
endif()