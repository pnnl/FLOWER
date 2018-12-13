#!/bin/bash

echo ${CMAKE_SYSTEM_VERSION}

exit 0

base_path=`dirname $0`
export MAKEFLAGS="-j 4"
export CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE=Release -DENABLE_TESTS=ON"
export CMAKECMD="cmake"
export CTESTCMD="ctest"
export CPACKCMD="cpack"
#  NOTE: Hack to get around missing cmake when cmake3 is installed
if [[ -z "`which cmake 2> /dev/null | grep -v cmake3`" ]]; then
  if [[ ! -z "`which cmake3 2> /dev/null`" ]]; then
    echo "NOTE: Aliasing cmake to cmake3"
    echo "NOTE: Aliasing ctest to ctest3"
    echo "NOTE: Aliasing cpack to cpack3"
    export CMAKECMD="cmake3"
    export CTESTCMD="ctest3"
    export CPACKCMD="cpack3"
  fi
fi

cd $base_path                || exit 1
mkdir -p build               || exit 1
cd build                     || exit 1
$CMAKECMD $CMAKE_OPTIONS ..  || exit 1
$CMAKECMD --build .          || exit 1
$CTESTCMD
echo ""
echo "  NOTE:"
echo "         There will probably be 3 to 5 errors for tests"
echo "         with sudo in the name"
echo ""
$CPACKCMD                    || exit 1
