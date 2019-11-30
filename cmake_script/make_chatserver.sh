#!/bin/sh
cmake ../ChatServer/CMakeLists.txt
cd ../ChatServer
make - j8
cp -f ./ChatServer ../bin
