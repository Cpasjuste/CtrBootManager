# - Try to find ctrulib
# Once done this will define
#  LIBCTRU_FOUND - System has ctrulib
#  LIBCTRU_INCLUDE_DIRS - The ctrulib include directories
#  LIBCTRU_LIBRARIES - The libraries needed to use ctrulib
#
# It also adds an imported target named `3ds::ctrulib`.
# Linking it is the same as target_link_libraries(target ${LIBCTRU_LIBRARIES}) and target_include_directories(target ${LIBCTRU_INCLUDE_DIRS})

# DevkitPro paths are broken on windows, so we have to fix those
macro(msys_to_cmake_path MsysPath ResultingPath)
    string(REGEX REPLACE "^/([a-zA-Z])/" "\\1:/" ${ResultingPath} "${MsysPath}")
endmacro()

if(NOT DEVKITPRO)
    msys_to_cmake_path("$ENV{DEVKITPRO}" DEVKITPRO)
endif()

set(LIBCTRSHELL_PATHS $ENV{LIBCTRSHELL} libctrshell ${DEVKITPRO}/libctrshell)

find_path(LIBCTRSHELL_INCLUDE_DIR ctr_shell.h
          PATHS ${LIBCTRSHELL_PATHS}
          PATH_SUFFIXES include libctrshell/include )

find_library(LIBCTRSHELL_LIBRARY NAMES libctrshell libctrshell.a
          PATHS ${LIBCTRSHELL_PATHS}
          PATH_SUFFIXES lib libctrshell/lib )

set(LIBCTRSHELL_LIBRARIES ${LIBCTRSHELL_LIBRARY} )
set(LIBCTRSHELL_INCLUDE_DIRS ${LIBCTRSHELL_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBCTRU_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LIBCTRSHELL  DEFAULT_MSG
        LIBCTRSHELL_LIBRARY LIBCTRSHELL_INCLUDE_DIR)

mark_as_advanced(LIBCTRSHELL_INCLUDE_DIR LIBCTRSHELL_LIBRARY )
if(LIBCTRSHELL_FOUND)
    set(LIBCTRSHELL ${LIBCTRSHELL_INCLUDE_DIR}/..)
    message(STATUS "setting LIBCTRSHELL to ${LIBCTRSHELL}")

    add_library(3ds::libctrshell STATIC IMPORTED GLOBAL)
    set_target_properties(3ds::libctrshell PROPERTIES
        IMPORTED_LOCATION "${LIBCTRSHELL_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${LIBCTRSHELL_INCLUDE_DIR}"
    )
endif()
