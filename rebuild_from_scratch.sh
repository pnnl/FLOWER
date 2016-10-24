#!/bin/bash

. $HOME/.bashrc

#_GLIBCXX_USE_CXX11_ABI=0
export CPPFLAGS=-D_GLIBCXX_USE_CXX11_ABI=0

BOOST_HOME=/usr/local/boost_1_60_0_shared64_debug_multi
PREFIX=$HOME/dev/flower
ncpu=`grep processor /proc/cpuinfo | wc -l`
make distclean
rm -rf autom4te.cache
rm -f  config.log config.status Makefile
rm -f  bin/*
autoreconf --install --force
./configure --enable-maintainer-mode --prefix=$PREFIX --with-boost=$BOOST_HOME
#./configure --prefix=$PREFIX --with-boost=$BOOST_HOME
#make -j $ncpu && make -j $ncpu release && make -j $ncpu check && make -j $ncpu install
make -j $ncpu
#make
#&& make install
#cd src/test
#make
#make -j $ncpu && make check
