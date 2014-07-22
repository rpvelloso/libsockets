#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AM_INIT_AUTOMAKE
# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([crypto], [ERR_load_crypto_strings])
AC_CHECK_LIB([pthread], [pthread_self])
AC_CHECK_LIB([ssl], [SSL_library_init])
AC_CHECK_LIB([lua], [luaL_newstate])
AC_CHECK_LIB([sockets], [main])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([dup2 getcwd strdup strtol])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
