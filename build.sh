#!/usr/bin/bash

ROOTDIRECTORY=$PWD

# COMPILE MODULES IF NECESSARY
if [ ! -f libs/libglfw3.a ]; then
    cd modules/glfw
    if [ ! -d build ]; then
        mkdir build
    fi
    cmake -S . -B build -D BUILD_SHARED_LIBS=OFF -D GLFW_BUILD_EXAMPLES=OFF -D GLFW_BUILD_TESTS=OFF -D GLFW_BUILD_DOCS=OFF

    cd build
    make

    cd $ROOTDIRECTORY
    if [ ! -d libs ]; then
        mkdir libs
    fi
    cp modules/glfw/build/src/libglfw3.a libs/libglfw3.a
fi

# COMPILE MAIN PROGRAM
if [ ! -d build ]; then
    mkdir build
fi

cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
mv Plotty ../Plotty
