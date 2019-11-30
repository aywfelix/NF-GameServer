#!/bin/sh
cmake ../ThirdPart/navigation/CMakeLists.txt
cd ../ThirdPart/navigation/
make -j8

cp -f ./libNavMesh.a ../../Dependencies/libs
