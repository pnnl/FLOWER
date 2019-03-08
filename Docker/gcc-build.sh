#!/bin/bash  

if [[ $EUID -ne 0 ]]; then
  echo "################################################"  
  echo "#   ERROR: This script must be run as root" 
  echo "################################################"  
  exit 1
fi

export TOP_DIR=/usr/local
export DOWNLOAD_DIR=$TOP_DIR/download
export MY_TOOL=gcc
export MY_GCC_VER=8.2
export MY_TOOL_VER=8.2.0
export PROCS=2
export URL="http://www.netgull.com"
export URLPATH="${MY_TOOL}/releases/${MY_TOOL}-${MY_TOOL_VER}"
export URLFILENAME="${MY_TOOL}-${MY_TOOL_VER}.tar.gz"
  
if [ ! -d "${DOWNLOAD_DIR}" ]; then  
  echo "################################################"  
  echo "#         CREATING ${DOWNLOAD_DIR}"  
  echo "################################################"
  mkdir -p "$DOWNLOAD_DIR"                                 || exit 1
  cd "$DOWNLOAD_DIR"                                       || exit 1
fi  
  
if [ ! -f "$DOWNLOAD_DIR/${MY_TOOL}-${MY_TOOL_VER}.tar.gz" ]; then  
  echo "################################################"  
  echo "#         DOWNLOADING ${MY_TOOL}_${MY_TOOL_VER}"  
  echo "################################################"  
  cd "$DOWNLOAD_DIR"                                       || exit 1
  wget -q "${URL}/${URLPATH}/${URLFILENAME}"               || exit 1
fi  
  
if [ ! -d "$DOWNLOAD_DIR/${MY_TOOL}-${MY_TOOL_VER}" ]; then  
  echo "################################################"  
  echo "#         UNPACKING ${MY_TOOL}_${MY_TOOL_VER}"  
  echo "################################################"  
  cd "$DOWNLOAD_DIR"                                       || exit 1
  tar xzf "${MY_TOOL}-${MY_TOOL_VER}".tar.gz               || exit 1
fi  
  
cd "$DOWNLOAD_DIR/${MY_TOOL}-${MY_TOOL_VER}"               || exit 1  
  
echo "################################################"  
echo "#         DOWNLOADING Prerequisites"  
echo "################################################"  
#  NOTE:  This installs the mpc, mpfr, gmp, and zlib prereqs if needed  
./contrib/download_prerequisites                           || exit 1 
  
  
#  NOTE: This creates a build directory so we don't overwrite the original source  
mkdir -p build                                             || exit 1
cd build                                                   || exit 1
  
  
echo "################################################"  
echo "#         CONFIGURE"  
echo "################################################"  
$DOWNLOAD_DIR/"${MY_TOOL}-${MY_TOOL_VER}"/configure        \
    --prefix="$TOP_DIR/${MY_TOOL}-${MY_GCC_VER}"           \
    --disable-multilib                                     \
    --enable-threads=posix                                 \
    --enable-__cxa_atexit                                  \
    --enable-clocale=gnu                                   \
    --enable-checking=release                              \
    --enable-languages=c,c++                               \
    --program-suffix=-"${MY_GCC_VER}"   || (cat config.log && exit 1)
  
echo "################################################"  
echo "#         MAKE"  
echo "################################################"  
ulimit -s 32768                                            || exit 1
make -j $PROCS                                             || exit 1
  
make install                                               || exit 1
ldconfig  
  
exit 0
