#!/bin/sh
echo "start complier libevent............."
cd ../ThirdPart/libevent-master

cmake .
make -j8
make install

if [ $? -eq 0 ]; then
	echo "complier libevent OK.........."
else
	echo "complier libevent Error with ErrorCode="$?
	exit 1
fi

cp -rf ./libevent_core.a ./libevent_extra.a ./libevent.a  ../../Dependencies/libs/
rm -rf ../../Dependencies/linux_libevent/include
cp -rf ./include  ../../Dependencies/linux_libevent/
