#!/bin/sh
./clean.sh &&
cd build/ &&
cmake -DCMAKE_TOOLCHAIN_FILE=../DevkitArm3DS.cmake ../ &&
cmake -DCMAKE_TOOLCHAIN_FILE=../DevkitArm3DS.cmake ../ &&
cmake --build . --target CtrBootManager_a9lh
