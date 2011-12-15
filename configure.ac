AC_INIT([afptest], [1.0.3])
AC_CONFIG_SRCDIR([test/spectest.c])
AM_INIT_AUTOMAKE(foreign)
AM_CONFIG_HEADER(config.h)

AC_CANONICAL_SYSTEM
AC_PROG_CC

AC_CHECK_HEADERS(fcntl.h strings.h sys/fcntl.h unistd.h sys/cdefs.h)

AM_PROG_LIBTOOL
# AM_ICONV
AC_SYS_LARGEFILE

AC_CHECK_HEADERS([byteswap.h])

AC_CHECK_LIB(dl, dlopen)
AC_CHECK_LIB(nsl, main)
AC_CHECK_LIB(socket, socket)

AC_MSG_CHECKING(for bswap_64)
AC_TRY_LINK([#if HAVE_BYTESWAP_H
		 #include <byteswap.h>
		 #endif]
           ,[bswap_64(37);]
           ,neta_cv_bswap_64=yes,neta_cv_bswap_64=no)
AC_MSG_RESULT($neta_cv_bswap_64)
if test x"$neta_cv_bswap_64" = xyes; then
  AC_DEFINE(HAVE_BSWAP_64,1,[Define to 1 if bswap_64 is available via byteswap.h])
fi

AC_OUTPUT([Makefile
	macros/Makefile
	test/Makefile])
