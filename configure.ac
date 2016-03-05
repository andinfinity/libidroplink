AC_INIT([libidroplink], [0.1], [chris@andinfinity.de])
AM_INIT_AUTOMAKE([foreign subdir-objects])

case $(uname -s) in
    Darwin)
        AM_CONDITIONAL([DARWIN], true)
        ;;
    *)
        AM_CONDITIONAL([DARWIN], false)
        ;;
esac

m4_include([libcurl.m4])

AC_CHECK_HEADERS([stdlib.h strings.h])
AC_TYPE_SIZE_T

AC_PROG_CC_C89
AC_PROG_RANLIB
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
