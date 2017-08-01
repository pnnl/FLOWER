#!/bin/bash

#  NOTE: Later versions may have new or different build and installation instructions.
#  we are using the bash shell on Linux
#  we will build the libraries in BUILD_DIR, /var/tmp
#  we will install the libraries in INSTALL_DIR, /usr/local
#  we are building the 64-bit versions of the libaries
#  we are building "static" (LIB_MODE) version of the libraries
#  we are building "release" (BOOST_VARIANT) versions of the libaries

export CXX_PATH=/usr/local/gcc
export INSTALL_DIR=/usr/local
#export LIB_MODE="static"              # Change to "shared" for shared libraries
export LIB_MODE="shared"              # Change to "shared" for shared libraries
#export BOOST_VARIANT="release"        # Change to "debug"  for debug  libraries
export BOOST_VARIANT="debug"          # Change to "debug"  for debug  libraries


export VERSION=1.60.0
export SVERSION=`echo $VERSION | sed -e "s/\./_/g"`
export PROCS=`grep -c ^processor /proc/cpuinfo` || 1

cd /usr/local


if [ ! -f "boost_$SVERSION.tar.gz" ]; then
  echo "################################################"
  echo "#         DOWNLOADING boost_$SVERSION"
  echo "################################################"
  echo "wget --progress=bar:force 'https://sourceforge.net/projects/boost/files/boost/1.60.0/boost_1_60_0.tar.gz'"
        wget --progress=bar:force 'https://sourceforge.net/projects/boost/files/boost/1.60.0/boost_1_60_0.tar.gz'
fi


if [ ! -d "boost_$SVERSION" ]; then
  if [ ! -f "boost_$SVERSION.tar.gz" ]; then
    echo "################################################"
    echo "#         ERROR MISSING boost_$SVERSION.tar.gz"
    echo "################################################"
  else
    echo "################################################"
    echo "#         UNPACKING boost_$SVERSION"
    echo "################################################"
    echo "tar xzf boost_$SVERSION.tar.gz"
          tar xzf boost_$SVERSION.tar.gz  || exit 1
  fi
fi


echo "cd boost_$SVERSION"
      cd boost_$SVERSION  || exit 1


echo "./bootstrap.sh --with-libraries=all -without-libraries=mpi --without-icu"
      ./bootstrap.sh --with-libraries=all -without-libraries=mpi --without-icu    || exit 1


echo "./bjam -j$PROCS --layout=tagged --without-mpi --without-python --builddir=../build_boost --build-type=co
mplete toolset=gcc --prefix=$INSTALL_DIR/boost optimization=speed link=$LIB_MODE address-model=64 runtime-link
=$LIB_MODE variant=${BOOST_VARIANT} threading=multi install"
      ./bjam -j$PROCS --layout=tagged --without-mpi --without-python --builddir=../build_boost --build-type=complete toolset=gcc --prefix=$INSTALL_DIR/boost optimization=speed link=$LIB_MODE address-model=64 runtime-link=$LIB_MODE variant=${BOOST_VARIANT} threading=multi install > build_boost.out 2>&1

exit 0
