#!/bin/bash

. $HOME/.bashrc

export CPPFLAGS=-D_GLIBCXX_USE_CXX11_ABI=0

#BOOST_HOME=/usr/local/boost_1_60_0_shared64_debug_multi
BOOST_HOME=/usr/local/boost
PREFIX=$HOME/dev/flower
ncpu=`grep -c ^processor /proc/cpuinfo`
make distclean
rm -rf autom4te.cache
rm -f  config.log config.status Makefile
rm -f  bin/*
autoreconf --install --force
./configure --enable-maintainer-mode --prefix=$PREFIX --with-boost=$BOOST_HOME
make
#-j $ncpu
