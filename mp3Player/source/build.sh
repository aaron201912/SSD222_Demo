#!/bin/sh

#GCC_VERSION=9.1.0
GCC_VERSION=6.4.0
TARGET=Mp3Player
BUILD_STATIC=1

if [ "${GCC_VERSION}" = "9.1.0"  ];then
export PATH=/tools/toolchain/gcc-sigmastar-9.1.0-2020.07-x86_64_arm-linux-gnueabihf/bin:$PATH
export CROSS_COMPILE=arm-linux-gnueabihf-
export ARCH=arm
else
export PATH=/home/koda.xu/tmp/cus_xiaomi/sourcecode/project/cus_toolchain/gcc-linaro-6.4.0-2022.05-x86_64_arm-linux-gnueabihf/bin:$PATH
export CROSS_COMPILE=arm-linux-gnueabihf-
export ARCH=arm
fi

if [ "${BUILD_STATIC}" = "1" ];then
TARGET=Mp3Player_static
fi

make clean -j32
make ENABLE_STATIC=${BUILD_STATIC} TARGET_NAME=${TARGET} -j32

mkdir -p ./../runEnv/bin/${GCC_VERSION}
mkdir -p ./../runEnv/lib/${GCC_VERSION}

arm-linux-gnueabihf-strip --strip-unneeded ${TARGET}

cp ${TARGET} ./../runEnv/bin/${GCC_VERSION}

if [ "${ENABLE_STATIC}" = "1" ];then
    echo "build static"
else
    echo "build dynamic"
    cp ./libmad/lib/${GCC_VERSION}/dynamic/* ./../runEnv/lib/${GCC_VERSION} -rfd
fi
