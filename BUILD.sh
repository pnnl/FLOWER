#!/bin/bash

base_path=`dirname $0`
export MAKEFLAGS="-j 4"

cd $base_path        || exit 1
mkdir -p build       || exit 1
cd build             || exit 1
cmake ..             || exit 1
cmake --build .      || exit 1
ctest                || exit 1
