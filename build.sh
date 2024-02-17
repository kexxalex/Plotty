#!/usr/bin/sh

ROOTDIR=$(pwd)

mkdir libs


cd external/glfw
cmake -S . -DGLFW_BUILD_DOCS=OFF -B build
cd build
make
cp src/libglfw3.a $ROOTDIR/libs/libglfw3.a

cd $ROOTDIR
clear

cd external/glew/auto
make
cd ..
make
cp lib/libGLEW.a $ROOTDIR/libs/libGLEW.a

cd $ROOTDIR
clear

cmake . -B build
cd build
make
