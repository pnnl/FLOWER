#!/bin/bash

if [[ $EUID -ne 0 ]]; then
  echo "################################################"
  echo "#   ERROR: This script must be run as root"
  echo "################################################"
  exit 1
fi

export TOP_DIR="/usr/local"
export DOWNLOAD_DIR="$TOP_DIR/download"
export MY_CXX_STD="11"
export MY_TOOL="gcc"
export MY_GCC_VER="8.2"
export MY_BOOST_DOT_VER="1.67.0"
export MY_BOOST_FULL_VER="1_67_0"
export PROCS="4"
export SED="sed"
export URL="https://sourceforge.net"
export URLPATH="projects/boost/files/boost/${MY_BOOST_DOT_VER}"
export URLFILENAME="boost_${MY_BOOST_FULL_VER}.tar.gz"
export INSTALL_DIR=$TOP_DIR/gcc-${MY_GCC_VER}_boost-${MY_BOOST_DOT_VER}

echo "Building with:"
echo "  GCC:   $MY_GCC_VER"
echo "  STD:   $MY_CXX_STD"
echo "  BOOST: $MY_BOOST_DOT_VER"

export LIB_MODE=static
export BOOST_VARIANT=debug,release
export CXX_PATH=$TOP_DIR/gcc-${MY_GCC_VER}/bin
export CXX_COMPILER=$CXX_PATH/gcc-${MY_GCC_VER}
export PATH=$CXX_PATH:$PATH

if [ ! -d "${DOWNLOAD_DIR}" ]; then
  echo "################################################"
  echo "#         CREATING ${DOWNLOAD_DIR}"
  echo "################################################"
  mkdir -p "$DOWNLOAD_DIR"                                 || exit 1
  cd "$DOWNLOAD_DIR"                                       || exit 1
fi

if [ ! -f "${URLFILENAME}" ]; then
  echo "################################################"
  echo "#         DOWNLOADING ${URLFILENAME}"
  echo "################################################"
  cd "${DOWNLOAD_DIR}"                                     || exit 1
  echo "wget -q ${URL}/${URLPATH}/${URLFILENAME}"
        wget -q ${URL}/${URLPATH}/${URLFILENAME}           || exit 1
fi

if [ ! -d "boost_${MY_BOOST_FULL_VER}" ]; then
  echo "################################################"
  echo "#         UNPACKING boost_${MY_BOOST_FULL_VER}"
  echo "################################################"
  cd "$DOWNLOAD_DIR"                                       || exit 1
  tar xzf boost_${MY_BOOST_FULL_VER}.tar.gz                || exit 1
fi

echo "cd  $DOWNLOAD_DIR/boost_${MY_BOOST_FULL_VER}"
      cd "$DOWNLOAD_DIR/boost_${MY_BOOST_FULL_VER}"        || exit 1
echo "mkdir -p build"
      mkdir -p build                                       || exit 1

./bootstrap.sh                 \
    --with-libraries=all       \
    --without-libraries=mpi    \
    --without-icu              \
    --prefix=$INSTALL_DIR                                  || exit 1

cp project-config.jam project-config.jam.orig              || exit 1
#
#  NOTE: Comment out lines 10-15 in project-config.jam
#
perl -i.bak -pne 's/^/#/ if ($. > 9 && $. < 16)' project-config.jam.orig  || exit 1
#
#  NOTE: Insert the gcc version on line 17 in project-config.jam
#
sed -e "17iusing gcc : ${MY_GCC_VER} : ${CXX_COMPILER} ;" project-config.jam.orig > project-config.jam

#cat project-config.jam

echo "NOTE: In another terminal type the following and wait until you see DONE below:"
echo "tail -f ./build/build_boost.out"

echo ./b2 --build-dir=build cxxflags="-std=c++${MY_CXX_STD}" -j$PROCS define=BOOST_SYSTEM_NO_DEPRECATED --layout=tagged --without-mpi --without-python --builddir=./build --build-type=complete toolset=gcc --prefix=$INSTALL_DIR/boost optimization=speed link=$LIB_MODE address-model=64 runtime-link=$LIB_MODE variant=${BOOST_VARIANT} threading=multi install toolset=gcc-${MY_GCC_VER}

     ./b2 --build-dir=build cxxflags="-std=c++${MY_CXX_STD}" -j$PROCS define=BOOST_SYSTEM_NO_DEPRECATED --layout=tagged --without-mpi --without-python --builddir=./build --build-type=complete toolset=gcc --prefix=$INSTALL_DIR/boost optimization=speed link=$LIB_MODE address-model=64 runtime-link=$LIB_MODE variant=${BOOST_VARIANT} threading=multi install toolset=gcc-${MY_GCC_VER} > ./build/build_boost.out 

echo "DONE"

exit 0
