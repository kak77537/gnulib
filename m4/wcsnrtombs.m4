# wcsnrtombs.m4 serial 1
dnl Copyright (C) 2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_WCSNRTOMBS],
[
  AC_REQUIRE([gl_WCHAR_H_DEFAULTS])

  AC_REQUIRE([AC_TYPE_MBSTATE_T])
  AC_CHECK_FUNCS_ONCE([wcsnrtombs])
  if test $ac_cv_func_wcsnrtombs = no; then
    HAVE_WCSNRTOMBS=0
    gl_REPLACE_WCHAR_H
    AC_LIBOBJ([wcsnrtombs])
    AC_LIBOBJ([wcsrtombs-state])
    gl_PREREQ_WCSNRTOMBS
  fi
])

# Prerequisites of lib/wcsnrtombs.c.
AC_DEFUN([gl_PREREQ_WCSNRTOMBS], [
  :
])