#!/bin/sh

#GCC_VERSION=9.1.0
#GCC_VERSION=6.4.0
GCC_VERSION=8.2.1

if [ "${GCC_VERSION}" = "6.4.0" ];then
export PATH=/home/koda.xu/tmp/cus_xiaomi/sourcecode/project/cus_toolchain/gcc-linaro-6.4.0-2022.05-x86_64_arm-linux-gnueabihf/bin:$PATH
export CROSS_COMPILE=arm-linux-gnueabihf-
export ARCH=arm
elif [ "${GCC_VERSION}" = "8.2.1"  ];then
export PATH=/tools/toolchain/gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf/bin/:$PATH
export CROSS_COMPILE=arm-linux-gnueabihf-
export ARCH=arm
else
export PATH=/tools/toolchain/gcc-sigmastar-9.1.0-2020.07-x86_64_arm-linux-gnueabihf/bin:$PATH
export CROSS_COMPILE=arm-linux-gnueabihf-
export ARCH=arm
fi

CURRENT_PATH=$(pwd)
OUTPUT_DIR=${CURRENT_PATH}/host/${GCC_VERSION}

# -fforce-mem was removed in gcc 4.3
sed -i '/-fforce-mem/d' configure

# add enable-speed to fix the errors below:
# Error: selected processor does not support `rsc ip,ip,#0' in Thumb mode
# Error: selected processor does not support `rsc r9,r9,#0' in Thumb mode
# Error: selected processor does not support `rsc r8,r8,#0' in Thumb mode
# Error: selected processor does not support `rsc ip,ip,#0' in Thumb mode

chmod 755 configure

./configure \
    --host=arm-linux  \
    CC=${CROSS_COMPILE}gcc  \
    --prefix=${OUTPUT_DIR}                \
    --enable-speed          \
    --enable-debugging     \
    --enable-fpm=arm        \
    --enable-experimental

make clean -j32
make -j32
make install
#chmod 755 ${OUTPUT_DIR}/lib/*.so*
#${CROSS_COMPILE}strip --strip-unneeded ${OUTPUT_DIR}/lib/*.so*
#${CROSS_COMPILE}strip --strip-unneeded ${OUTPUT_DIR}/lib/*.a

cp ${OUTPUT_DIR}/include ./../libmad -r
mkdir -p ./../libmad/lib/${GCC_VERSION}/dynamic
mkdir -p ./../libmad/lib/${GCC_VERSION}/static
cp ${OUTPUT_DIR}/lib/*.so* ./../libmad/lib/${GCC_VERSION}/dynamic -rfd
cp ${OUTPUT_DIR}/lib/*.a ./../libmad/lib/${GCC_VERSION}/static -rfd
