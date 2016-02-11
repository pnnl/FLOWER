#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.63)
AC_INIT(flower, 6.0, flower@pnnl.gov)
AC_CONFIG_HEADER([config.h])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET
AC_PROG_RANLIB

AX_CXX_COMPILE_STDCXX_11([ext],[mandatory])

# Checks for libraries.
AC_CHECK_LIB([pcap],[pcap_create])


# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h memory.h string.h sys/ioctl.h sys/socket.h syslog.h unistd.h pcap/pcap.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_C_VOLATILE
AC_HEADER_STDBOOL
AC_HEADER_TIME
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UID_T

# Checks for library functions.
AC_FUNC_CHOWN
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_STAT
AC_PROG_GCC_TRADITIONAL
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([gettimeofday memset socket strerror strstr])

# Checks for Boost libraries
BOOST_REQUIRE([1.58])
BOOST_PROGRAM_OPTIONS([mt-d])
BOOST_FILESYSTEM([mt-d])
BOOST_SYSTEM([mt-d])
BOOST_REGEX([mt-d])
BOOST_DATE_TIME([mt-d])
BOOST_DATE_TIME([mt-d])
BOOST_TEST([mt-d])

AM_INIT_AUTOMAKE([1.11 subdir-objects -Wall -Werror])

AC_CONFIG_FILES([
                  Makefile
                ])
#                  src/flower/api/Makefile
#                  src/flower/exe/Makefile
#                  src/flower/api/test/Makefile
#                  src/flower/api/test/tickets/Makefile
#                  support/other/ide/kde/Makefile])

AC_OUTPUT
