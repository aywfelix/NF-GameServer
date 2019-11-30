#!/bin/sh
cmake ../LoginServer/CMakeLists.txt
cd ../LoginServer
make - j8
cp -f ./LoginServer ../bin

