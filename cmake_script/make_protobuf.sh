#!/bin/sh
echo "start complier protobuf............."
cd ../ThirdPart/protobuf-master/cmake

cmake -Dprotobuf_BUILD_TESTS=OFF

cmake .
make -j8
make install

if [ $? -eq 0 ]; then
	echo "complier protobuf OK.........."
else
	echo "complier protobuf Error with ErrorCode="$?
	exit 1
fi

cp -rf libprotobuf-lite.a libprotoc.a libprotobuf.a  ../../../Dependencies/libs/

rm -rf ../../../Dependencies/protobuf/src/google
cp -rf ../src/google ../../../Dependencies/protobuf/src/