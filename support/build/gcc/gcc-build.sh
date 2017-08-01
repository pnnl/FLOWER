#!/bin/bash

export GCC_VERSION=5.3.0
export PROCS=`grep -c ^processor /proc/cpuinfo` || 1
export SED=sed


cd /usr/local

if [ ! -f "gcc-$GCC_VERSION.tar.gz" ]; then
  echo "################################################"
  echo "#         DOWNLOADING gcc_$GCC_VERSION"
  echo "################################################"
  wget -q http://www.netgull.com/gcc/releases/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.gz
fi

if [ ! -d "gcc-$GCC_VERSION" ]; then
  echo "################################################"
  echo "#         UNPACKING gcc_$GCC_VERSION"
  echo "################################################"
  tar xzf gcc-$GCC_VERSION.tar.gz
fi

cd gcc-$GCC_VERSION                                        || exit 1

echo "################################################"
echo "#         DOWNLOADING Prerequisites"
echo "################################################"
#  NOTE:  This installs the mpc, mpfr, gmp, and zlib prereqs if needed
./contrib/download_prerequisites                           || exit 1
cd ..


#  NOTE: This creates a build directory so we don't overwrite the original source
mkdir -p gcc_$GCC_VERSION-build                            || exit 1
cd       gcc_$GCC_VERSION-build                            || exit 1


echo "################################################"
echo "#         CONFIGURE"
echo "################################################"
../gcc-$GCC_VERSION/configure                              \
    --prefix=/usr/local/gcc                                \
    --libdir=/usr/local/gcc/lib                            \
    --enable-shared                                        \
    --enable-threads=posix                                 \
    --enable-clocale=gnu                                   \
    --disable-bootstrap                                    \
    --disable-multilib                                     \
    --with-default-libstdcxx-abi=gcc4-compatible           \
    --enable-languages=c,c++                               || (cat config.log && exit 1)

echo "################################################"
echo "#         MAKE"
echo "################################################"
ulimit -s 32768                                            || exit 1
make -j $PROCS                                             || exit 1

make install                                               || exit 1
ldconfig

exit 0
