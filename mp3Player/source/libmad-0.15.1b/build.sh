#!/bin/bash
current_path=$(cd "$(dirname $0)";pwd)
rm -rf host
mkdir -p $current_path/host
install_path=$current_path/host
export CC=arm-linux-gnueabihf-gcc
export CXX=arm-linux-gnueabihf-g++
./configure --prefix=$install_path --enable-shared --host=arm-linux
make clean
make
make install
cp -rf $install_path/* ../libmad/
