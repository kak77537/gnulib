# pselect.m4
dnl Copyright (C) 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_PSELECT],
[
  AC_REQUIRE([gl_HEADER_SYS_SELECT])
  AC_REQUIRE([AC_C_RESTRICT])
  AC_CHECK_FUNCS_ONCE([pselect])

  if test $ac_cv_func_pselect = yes; then
    AC_CACHE_CHECK([whether signature of pselect conforms to POSIX],
      gl_cv_sig_pselect,
      [AC_LINK_IFELSE(
         [AC_LANG_PROGRAM(
              [[#include <sys/select.h>
                ]],
              [[int (*p) (int, fd_set *, fd_set *, fd_set *restrict,
                          struct timespec const *restrict,
                          sigset_t const *restrict) = pselect;
                return !p;]])],
         [gl_cv_sig_pselect=yes],
         [gl_cv_sig_pselect=no])])
  fi

  if test $ac_cv_func_pselect = no || test $gl_cv_sig_pselect = no; then
    REPLACE_PSELECT=1
  fi
])