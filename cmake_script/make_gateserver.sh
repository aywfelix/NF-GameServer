#!/bin/sh
cmake ../GateServer/CMakeLists.txt
cd ../GateServer
make - j8
cp -f ./GateServer ../bin

