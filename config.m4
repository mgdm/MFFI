dnl $Id$
dnl config.m4 for extension mffi

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

PHP_ARG_WITH(mffi, for mffi support,
[  --with-mffi             Include mffi support])


if test "$PHP_MFFI" != "no"; then
  dnl Write more examples of tests here...

  export OLD_CPPFLAGS="$CPPFLAGS"
  export CPPFLAGS="$CPPFLAGS $INCLUDES"

  AC_MSG_CHECKING(PHP version)
  AC_TRY_COMPILE([#include <php_version.h>], [
#if PHP_VERSION_ID < 70000
#error  this extension requires at least PHP version 7.0.0
#endif
],
[AC_MSG_RESULT(ok)],
[AC_MSG_ERROR([need at least PHP 7.0.0])])

  PHP_SUBST(MFFI_SHARED_LIBADD)
  AC_DEFINE(HAVE_MFFI, 1, [ ])

  PHP_NEW_EXTENSION(mffi, mffi.c mffi_function.c mffi_library.c mffi_struct.c, $ext_shared)

  MFFI_CHECK_DIR=$PHP_MFFI
  MFFI_TEST_FILE=/include/ffi.h
  MFFI_LIBNAME=libffi
  condition="$MFFI_CHECK_DIR$MFFI_TEST_FILE"

  if test -r $condition; then
   MFFI_DIR=$MFFI_CHECK_DIR
   CFLAGS="$CFLAGS -I$MFFI_DIR/include"
   LDFLAGS="-lffi"
  else
    AC_MSG_CHECKING(for pkg-config)

    if test ! -f "$PKG_CONFIG"; then
      PKG_CONFIG=`which pkg-config`
    fi

      if test -f "$PKG_CONFIG"; then
        AC_MSG_RESULT(found)
        AC_MSG_CHECKING(for libffi)

        if $PKG_CONFIG --exists libffi; then
            if $PKG_CONFIG --atleast-version=3.0.13 libffi; then
                libffi_version_full=`$PKG_CONFIG --modversion libffi`
                AC_MSG_RESULT([found $libffi_version_full])
                MFFI_LIBS="$LDFLAGS `$PKG_CONFIG --libs libffi`"
                MFFI_INCS="$CFLAGS `$PKG_CONFIG --cflags-only-I libffi`"
                PHP_EVAL_INCLINE($MFFI_INCS)
                PHP_EVAL_LIBLINE($MFFI_LIBS, MFFI_SHARED_LIBADD)
                AC_DEFINE(HAVE_MFFI, 1, [whether libffi exists in the system])
            else
                AC_MSG_RESULT(too old)
                AC_MSG_ERROR(You need at least libffi 3.0.13)
            fi
        else
            AC_MSG_RESULT(not found)
            AC_MSG_ERROR(No libffi detected in the system)
        fi
      else
        AC_MSG_RESULT(not found)
        AC_MSG_ERROR(No pkg-config found .... )
      fi
   fi
fi
