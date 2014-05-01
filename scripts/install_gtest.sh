#!/bin/sh

apt-get install libgtest-dev cmake
cd /usr/src/gtest
cmake CMakeLists.txt
make
cp *.a /usr/lib

