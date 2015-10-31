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

set(CTRULIB_PATHS $ENV{CTRULIB} libctru ctrulib ${DEVKITPRO}/libctru ${DEVKITPRO}/ctrulib)

find_library(LIBKAIRY_LIBRARY NAMES kairy libkairy.a
          PATHS ${CTRULIB_PATHS}
          PATH_SUFFIXES lib libctru/lib )

set(LIBKAIRY_LIBRARIES ${LIBKAIRY_LIBRARY} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBCTRU_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LIBKAIRY  DEFAULT_MSG LIBKAIRY_LIBRARY)

mark_as_advanced(LIBKAIRY_LIBRARY )
if(LIBKAIRY_FOUND)
    set(LIBKAIRY)
    message(STATUS "setting LIBKAIRY to ${LIBKAIRY}")

    add_library(3ds::libkairy STATIC IMPORTED GLOBAL)
    set_target_properties(3ds::libkairy PROPERTIES
        IMPORTED_LOCATION "${LIBKAIRY_LIBRARY}"
    )
endif()
