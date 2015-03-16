dnl $Id$
dnl config.m4 for extension mffi

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

PHP_ARG_WITH(mffi, for mffi support,
[  --with-mffi             Include mffi support])


if test "$PHP_MFFI" != "no"; then
  dnl Write more examples of tests here...

  # --with-mffi -> check with-path
  SEARCH_PATH="/usr/local /usr"     # you might want to change this
  SEARCH_FOR="/include/ffi.h"  # you most likely want to change this
  if test -r $PHP_MFFI/$SEARCH_FOR; then # path given as parameter
    MFFI_DIR=$PHP_MFFI
  else # search default path list
    AC_MSG_CHECKING([for libffi files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        MFFI_DIR=$i
		AC_MSG_CHECKING($MFFI_DIR)
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi
  
  if test -z "$MFFI_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the libffi distribution])
  fi

  # --with-mffi -> add include path
  PHP_ADD_INCLUDE($MFFI_DIR/include)

  # --with-mffi -> check for lib and symbol presence
  LIBNAME=ffi # you may want to change this

  PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $MFFI_DIR/$PHP_LIBDIR, mffi_SHARED_LIBADD)
  
  PHP_SUBST(MFFI_SHARED_LIBADD)

  PHP_NEW_EXTENSION(mffi, mffi.c mffi_library.c mffi_function.c, $ext_shared)

  AC_FUNC_STRERROR_R
fi
