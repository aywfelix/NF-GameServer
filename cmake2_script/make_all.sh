#!/bin/bash

WorkDir=$(cd $(dirname $0); pwd)
cd $WorkDir/../

if [ ! -d bin  ];then
  mkdir bin
  chmod 777 bin
fi
cd $WorkDir
Servers=("Common" "ChatServer")
gen_servers_cmake(){
    for str in ${Servers[@]};do
        echo $str
        python3 gen_cmake.py $str $1
    done 
}

make_all_servers(){
    for str in ${Servers[@]};do
        cd ../$str && pwd
        if [ ! -d build ];then
            mkdir build
            chmod 777 build
        fi
        cd build
        cmake ..
        make $1
        cd ../
    done 
}

gen_servers_cmake "debug"
make_all_servers "-j8"
