#!/bin/sh
cmake ../MasterServer/CMakeLists.txt
cd ../MasterServer
make - j8
cp -f ./MasterServer ../bin
