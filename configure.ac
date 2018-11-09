AC_CONFIG_MACRO_DIRS([m4])

m4_define([v_maj], [3])
m4_define([v_min], [0])
m4_define([v_mic], [1])
m4_define([project_version], [v_maj.v_min.v_mic])
m4_define([lt_cur], [m4_eval(v_maj+v_min)])
m4_define([lt_rev], [v_mic])
m4_define([lt_age], [v_min])

AC_INIT([alt_logger], [project_version], zorak.x86@gmail.com)
AM_INIT_AUTOMAKE

AM_MAINTAINER_MODE

AC_PROG_LIBTOOL

version_info="lt_cur:lt_rev:lt_age"
AC_SUBST([version_info])

# Checks for programs.
AC_PROG_CXX
# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([alt_mutex/locker_mutex.h])
AC_CHECK_HEADERS([alt_mutex/mutex_instance.h])
AC_CHECK_HEADERS([pwd.h])
AC_CHECK_HEADERS([stdarg.h]) 
AC_CHECK_HEADERS([stdio.h])
AC_CHECK_HEADERS([string.h])
AC_CHECK_HEADERS([syslog.h])
AC_CHECK_HEADERS([sys/stat.h])
AC_CHECK_HEADERS([sys/types.h])
AC_CHECK_HEADERS([time.h])
AC_CHECK_HEADERS([unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC

AX_PTHREAD()

AX_LIB_SQLITE3()

SQLITE3_REQ_VERSION="3.0.0"
AX_LIB_SQLITE3($SQLITE3_REQ_VERSION)
if test "x$SQLITE3_VERSION" = "x"; then
	AC_MSG_ERROR([not found SQLite.])
fi

LT_INIT([win32-dll])
AC_OUTPUT(Makefile src/Makefile)
