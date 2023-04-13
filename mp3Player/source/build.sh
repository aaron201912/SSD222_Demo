#!/bin/sh

#GCC_VERSION=9.1.0
GCC_VERSION=6.4.0
TARGET_NAME=Mp3Player

if [ "${GCC_VERSION}" = "9.1.0"  ];then
export PATH=/tools/toolchain/gcc-sigmastar-9.1.0-2020.07-x86_64_arm-linux-gnueabihf/bin:$PATH
export CROSS_COMPILE=arm-linux-gnueabihf-
export ARCH=arm
else
export PATH=/home/koda.xu/tmp/cus_xiaomi/sourcecode/project/cus_toolchain/gcc-linaro-6.4.0-2022.05-x86_64_arm-linux-gnueabihf/bin:$PATH
export CROSS_COMPILE=arm-linux-gnueabihf-
export ARCH=arm
fi

make clean -j32
make -j32

mkdir -p ./../runEnv/bin/${GCC_VERSION}
mkdir -p ./../runEnv/lib/${GCC_VERSION}

arm-linux-gnueabihf-strip --strip-unneeded ${TARGET_NAME}
cp ${TARGET_NAME} ./../runEnv/bin/${GCC_VERSION}
cp ./libmad/lib/${GCC_VERSION}/*.so* ./../runEnv/lib/${GCC_VERSION} -rfd
