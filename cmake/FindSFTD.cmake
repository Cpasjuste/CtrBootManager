# - Try to find sf2d
# Once done this will define
#  LIBSF2D_FOUND - System has sf2d
#  LIBSF2D_INCLUDE_DIRS - The sf2d include directories
#  LIBSF2D_LIBRARIES - The libraries needed to use sf2d
#
# It also adds an imported target named `3ds::sf2d`.
# Linking it is the same as target_link_libraries(target ${LIBSF2D_LIBRARIES}) and target_include_directories(target ${LIBSF2D_INCLUDE_DIRS})

# DevkitPro paths are broken on windows, so we have to fix those
macro(msys_to_cmake_path MsysPath ResultingPath)
    string(REGEX REPLACE "^/([a-zA-Z])/" "\\1:/" ${ResultingPath} "${MsysPath}")
endmacro()

if(NOT DEVKITPRO)
    msys_to_cmake_path("$ENV{DEVKITPRO}" DEVKITPRO)
endif()

#set(CTRULIB_PATHS $ENV{CTRULIB} libctru ctrulib ${DEVKITPRO}/libctru ${DEVKITPRO}/ctrulib)

find_path(LIBSFTD_INCLUDE_DIR sftd.h
        PATHS ${CTRULIB_PATHS}
        PATH_SUFFIXES include libctru/include)

find_library(LIBSFTD_LIBRARY NAMES sftd libsftd.a
        PATHS ${CTRULIB_PATHS}
        PATH_SUFFIXES lib libctru/lib)

set(LIBSFTD_LIBRARIES ${LIBSFTD_LIBRARY} )
set(LIBSFTD_INCLUDE_DIRS ${LIBSFTD_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBSF2D_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(SFTD  DEFAULT_MSG
        LIBSFTD_LIBRARY LIBSFTD_INCLUDE_DIR)

mark_as_advanced(LIBSFTD_INCLUDE_DIR LIBSFTD_LIBRARY )
if(SFTD_FOUND)
    set(SFTD ${LIBSFTD_INCLUDE_DIR}/..)
    message(STATUS "setting SFTD to ${SFTD}")

    add_library(3ds::sftd STATIC IMPORTED GLOBAL)
    set_target_properties(3ds::sftd PROPERTIES
            IMPORTED_LOCATION "${LIBSFTD_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${LIBSFTD_INCLUDE_DIR}"
            )
endif()