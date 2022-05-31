#!/bin/bash

export PROCS=4

#export GCC_VER=5.3
export MY_CXX_STD=11
export MY_BOOST_VER=60

export GCC_VER=8.1
#export MY_CXX_STD=17
export MY_BOOST_VER=67

echo "Building with:"
echo "  GCC:   $GCC_VER"
echo "  STD:   $MY_CXX_STD"
echo "  BOOST: $MY_BOOST_VER"

export INSTALL_DIR=/usr/local/gcc-${GCC_VER}_boost-1.${MY_BOOST_VER}.0

export LIB_MODE=static
#export LIB_MODE=shared
export BOOST_VARIANT=debug,release
export CXX_PATH=/usr/local/gcc-${GCC_VER}/bin
export PATH=/usr/local/gcc-${GCC_VER}/bin:$PATH

cd /home/d3c572/downloads

sudo rm -rf boost_1_${MY_BOOST_VER}_0
tar xzf boost_1_${MY_BOOST_VER}_0.tar.gz

cd /home/d3c572/downloads/boost_1_${MY_BOOST_VER}_0
mkdir -p build

./bootstrap.sh --with-libraries=all --without-libraries=mpi --without-icu --prefix=$INSTALL_DIR
#exit

cp project-config.jam project-config.jam.orig
perl -i.bak -pne 's/^/#/ if ($. > 9 && $. < 16)' project-config.jam.orig
sed -e "17iusing gcc : ${GCC_VER} : /usr/local/gcc-${GCC_VER}/bin/g++-${GCC_VER} ;" project-config.jam.orig > project-config.jam

cat project-config.jam

echo "NOTE: In another terminal type the following:"
echo "tail -f ./build/build_boost.out"

echo sudo ./b2 --build-dir=build cxxflags="-std=c++${MY_CXX_STD}" -j$PROCS define=BOOST_SYSTEM_NO_DEPRECATED --layout=tagged --without-mpi --without-python --builddir=./build --build-type=complete toolset=gcc --prefix=$INSTALL_DIR/boost optimization=speed link=$LIB_MODE address-model=64 runtime-link=$LIB_MODE variant=${BOOST_VARIANT} threading=multi install toolset=gcc-${GCC_VER}

sudo ./b2 --build-dir=build cxxflags="-std=c++${MY_CXX_STD}" -j$PROCS define=BOOST_SYSTEM_NO_DEPRECATED --layout=tagged --without-mpi --without-python --builddir=./build --build-type=complete toolset=gcc --prefix=$INSTALL_DIR/boost optimization=speed link=$LIB_MODE address-model=64 runtime-link=$LIB_MODE variant=${BOOST_VARIANT} threading=multi install toolset=gcc-${GCC_VER} > ./build/build_boost.out 

echo "DONE"

#sudo ./bjam -j$PROCS --layout=tagged --without-mpi --without-python --builddir=./build --build-type=complete toolset=gcc --prefix=$INSTALL_DIR/boost optimization=speed link=$LIB_MODE address-model=64 runtime-link=$LIB_MODE variant=${BOOST_VARIANT} threading=multi install > ./build/build_boost.out 
