#!/bin/sh
cmake ../GameServer/CMakeLists.txt
cd ../GameServer
make - j8
cp -f ./GameServer ../bin

