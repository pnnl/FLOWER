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
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

AX_CXX_COMPILE_STDCXX_11([ext],[mandatory])

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h memory.h string.h sys/ioctl.h sys/socket.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_TYPE_UID_T
AC_C_VOLATILE

# Checks for library functions.
AC_FUNC_CHOWN
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_CHECK_FUNCS([gettimeofday memset socket strerror strstr])

AM_INIT_AUTOMAKE([1.11 subdir-objects -Wall -Werror])

#BOOST_REQUIRE([1.34.1])
BOOST_REQUIRE
BOOST_FILESYSTEM
BOOST_PROGRAM_OPTIONS
BOOST_REGEX
BOOST_SYSTEM

AC_CONFIG_FILES([
                  Makefile
                ])
#                  src/flower/api/Makefile
#                  src/flower/exe/Makefile
#                  src/flower/api/test/Makefile
#                  src/flower/api/test/tickets/Makefile
#                  support/other/ide/kde/Makefile])

AC_OUTPUT
