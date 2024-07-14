#!/usr/bin/bash

cd modules/glfw
mkdir build
cmake -S . -B build -D BUILD_SHARED_LIBS=OFF -D GLFW_BUILD_EXAMPLES=OFF -D GLFW_BUILD_TESTS=OFF -D GLFW_BUILD_DOCS=OFF

cd build
make -j4

cd ../../..
echo $PWD
cp modules/glfw/build/src/libglfw3.a libs/libglfw3.a

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
mv Plotty ../Plotty