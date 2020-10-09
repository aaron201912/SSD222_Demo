#!/bin/sh

export ARCH=arm
export CROSS_COMPILE=arm-eabi-
export PATH=~/toolchain/gcc-arm-8.2-2019.01-x86_64-arm-eabi/bin:$PATH

export SUPPORT_1024x600=1
export USE_MIPI=0

make clean;make

