cmake_minimum_required(VERSION 2.8)
project(CtrBootManager)
enable_language(ASM)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${DKA_SUGGESTED_C_FLAGS} -O2 -DSFMT_MEXP=19937") # Use the devkitArm suggested flags. This is not mandatory.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DKA_SUGGESTED_CXX_FLAGS} -O2 -DSFMT_MEXP=19937")

# Note that you must copy the cmake folder and the DevkitArm3DS.cmake file in this directory
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake) # Add the cmake folder to the modules paths, so that we can use the tools and find_package for ctrulib
include(Tools3DS) # Include all the macros and tools needed for 3ds development.

find_package(CTRULIB REQUIRED) # Look for ctrulib

file(GLOB_RECURSE SOURCE_FILES
        source/*
)
add_executable(CtrBootManager ${SOURCE_FILES}) # Create the elf file

target_link_libraries(CtrBootManager ${LIBCTRU_LIBRARIES}) # Link ctrulib
target_include_directories(CtrBootManager PUBLIC ${LIBCTRU_INCLUDE_DIRS}) # Add the ctrulib include folder

target_include_directories(CtrBootManager PUBLIC ${DEVKITPRO}/portlibs/armv6k/include)
#target_link_libraries(CtrBootManager ${DEVKITPRO}/portlibs/armv6k/lib/libpng.a)
target_link_libraries(CtrBootManager ${DEVKITPRO}/portlibs/armv6k/lib/libz.a)
target_link_libraries(CtrBootManager ${DEVKITPRO}/portlibs/armv6k/lib/libconfig.a)

target_include_directories(CtrBootManager PUBLIC source source/CakeBrah/include)
target_include_directories(CtrBootManager PUBLIC source/CakeBrah/source/libkhax)

target_embed_file(CtrBootManager data/font.bin)

add_3dsx_target(CtrBootManager) # Generate the .3dsx and .smdh from the CtrBootManager target
add_netload_target(CtrBootManager_3dslink CtrBootManager)

add_citra_target(CtrBootManager_Citra CtrBootManager)
set_target_properties(CtrBootManager PROPERTIES COMPILE_FLAGS "-DCITRA")
