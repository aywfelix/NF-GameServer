#!/bin/sh
cmake ../DBServer/CMakeLists.txt
cd ../DBServer
make - j8
cp -f ./DBServer ../bin
