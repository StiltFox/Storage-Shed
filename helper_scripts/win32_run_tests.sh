#!/bin/bash
rm -rf build
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/windows.toolchain -DCMAKE_INSTALL_PREFIX=/usr/i686-w64-mingw32/ -DCMAKE_BUILD_TYPE=Release -G Ninja ..
cmake --build .
wine test/Tests.exe
cd ..