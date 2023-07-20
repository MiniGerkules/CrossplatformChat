#!/usr/bin/env zsh

test -d ./build && rm -rf build
mkdir build

conan install . --output-folder=build --build=missing

cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug -G Xcode
