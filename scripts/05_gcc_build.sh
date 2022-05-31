#!/bin/bash

export MY_TOOL=gcc
export MY_GCC_VER=8.1
export MY_TOOL_VER=8.1.0
#export MY_GCC_VER=5.3
#export MY_TOOL_VER=5.3.0

#rm -rf ${MY_TOOL}-${MY_TOOL_VER} > /dev/null 2>&1
#tar xzf ${MY_TOOL}-${MY_TOOL_VER}.tar.gz
cd ${MY_TOOL}-${MY_TOOL_VER}
#./contrib/download_prerequisites
mkdir -p build && cd build
../configure --prefix=/usr/local/gcc-${MY_GCC_VER}    \
             --disable-multilib                       \
             --enable-checking=release                \
             --enable-languages=c,c++                 \
             --program-suffix=-${MY_GCC_VER}
make -j 4
sudo make install
