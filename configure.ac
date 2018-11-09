AC_CONFIG_MACRO_DIRS([m4])

m4_define(LIBALTOIDLOGGERVERSION,2.0.3)
AC_INIT(alt_logger, LIBALTOIDLOGGERVERSION, zorak.x86@gmail.com)
AM_INIT_AUTOMAKE(alt_logger, LIBALTOIDLOGGERVERSION)
AM_MAINTAINER_MODE

AC_PROG_LIBTOOL

# Checks for programs.
AC_PROG_CXX
# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([alt_mutex/locker_mutex.h alt_mutex/mutex_instance.h pwd.h sqlite3.h stdarg.h stdio.h string string.h syslog.h sys/stat.h sys/types.h time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([sqlite3_step])

AC_OUTPUT(Makefile src/Makefile)
LT_INIT([win32-dll])
