#!/bin/sh
cmake ../WorldServer/CMakeLists.txt
cd ../WorldServer
make - j8
cp -f ./WorldServer ../bin
