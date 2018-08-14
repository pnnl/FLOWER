#!/bin/bash

. $HOME/.bashrc

if [ ! -f /etc/os-release ]
then 
  release_value=`grep release /etc/redhat-release | awk -F'release ' '{print $2} ' | awk -F'.' '{print $1}'`
else
  release_value=`grep ^VERSION_ID= /etc/os-release | sed -e 's/"//g' | awk -F'=' '{print $2}' | awk -F'.' '{print $1}'`
fi
#echo release value - $release_value

case $release_value in 
  6) export CPPFLAGS=-D_GLIBCXX_USE_CXX11_ABI=0
     BOOST_HOME=/usr/local/boost_1_60_0_shared64_debug_multi
     PREFIX=$HOME/dev/flower
     ncpu=`grep -c ^processor /proc/cpuinfo`
     make distclean
     rm -rf autom4te.cache
     rm -f  config.log config.status Makefile
     rm -f  bin/*
     autoreconf --install --force
     ./configure --enable-maintainer-mode --prefix=$PREFIX --with-boost=$BOOST_HOME
     make
     ;;
  7) export CPPFLAGS=-D_GLIBCXX_USE_CXX11_ABI=0
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
     ;;
  16) export CPPFLAGS=-lboost_system 
      PREFIX=$HOME/dev/flower
      ncpu=`grep -c ^processor /proc/cpuinfo`
      make distclean
      rm -rf autom4te.cache
      rm -f  config.log config.status Makefile
      rm -f  bin/*
      autoreconf --install --force
      automake --add-missing
      ./configure --enable-maintainer-mode --prefix=$PREFIX
      make
      #-j $ncpu
     ;;
esac

exit
