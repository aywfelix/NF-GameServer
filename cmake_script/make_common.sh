#!/bin/sh
cmake ../Common/CMakeLists.txt
cd ../Common/
make -j8
cp -f ./libCommon.a ../Dependencies/libs
