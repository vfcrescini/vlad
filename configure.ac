# This file is part of PolicyUpdater.
#
# Copyright (C) 2003, 2004, 2005, 2006 University of Western Sydney
# by Vino Fernando Crescini <jcrescin@cit.uws.edu.au>
#
# PolicyUpdater is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# PolicyUpdater is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with PolicyUpdater; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

AC_INIT(vlad,1.4.2,jcrescin@cit.uws.edu.au)

AC_CONFIG_HEADERS(lib/config.h)

# set language
AC_LANG(C++)

# set vars to itself to prevent the macros from setting default value
CFLAGS="$CFLAGS"
CXXFLAGS="$CXXFLAGS"

# set flags
VLAD_C_WFLG="-Wall -Wshadow -Wcast-align -Wstrict-prototypes"
VLAD_C_OFLG="-ansi -pedantic"
VLAD_CXX_WFLG="-Wall -Wshadow -Wcast-align"
VLAD_CXX_OFLG="-ansi -pedantic"
VLAD_CFLAGS="$VLAD_C_OFLG $VLAD_C_WFLG -I\$(DISTDIR)/include"
VLAD_CXXFLAGS="$VLAD_CXX_OFLG $VLAD_CXX_WFLG -I\$(DISTDIR)/include"
VLAD_LDFLAGS="-L\$(DISTDIR)/lib"

# some tests
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_LN_S

# check compiler options
cat >> $$.c << \EOF
int main(void) {return 0;}
EOF

# check if -fPIC works
AC_MSG_CHECKING([whether $CXX accepts -fPIC])
if "${CXX}" -fPIC -o $$ $$.c > /dev/null 2>&1; then
  VLAD_FPIC="PIC"
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])

  # no? try -fpic
  AC_MSG_CHECKING([whether $CXX accepts -fpic])
  if "${CXX}" -fpic -o $$ $$.c > /dev/null 2>&1; then
    VLAD_FPIC="pic"
    AC_MSG_RESULT([yes])
  else
    AC_MSG_RESULT([no])
    VLAD_FPIC="no"
  fi
fi

# check if -shared works
AC_MSG_CHECKING([whether $CXX compiler accepts -shared])
if "${CXX}" -shared -o $$ $$.c > /dev/null 2>&1; then
  VLAD_SHARED="yes"
  AC_MSG_RESULT([yes])
else
  VLAD_SHARED="no"
  AC_MSG_RESULT([no])
fi

# make sure they both are supported
if test "x$VLAD_FPIC" != "xno" && test "x$VLAD_SHARED" != "xno"; then
  AC_SUBST(BUILDSHARED,[$VLAD_FPIC])
fi

# cleanup
rm -f $$ $$.c

# --enable-static
AC_ARG_ENABLE(static,AC_HELP_STRING([--enable-static],[generate a statically-linked executable (default is disabled)]))

if test "x$enable_static" != "x" && test "x$enable_static" != "xno"; then
  AC_SUBST(STATIC,[yes])
else
  if test "x$VLAD_FPIC" = "xno" || test "x$VLAD_SHARED" = "xno"; then
    AC_MSG_WARN([cannot build shared libraries, will link statically])
    AC_SUBST(STATIC,[yes])
  fi
fi

# --enable-debug
AC_ARG_ENABLE(debug,AC_HELP_STRING([--enable-debug],[enable debugging output & compile with -g (default is disabled)]))

if test "x$enable_debug" != "x" && test "x$enable_debug" != "xno"; then
  AC_DEFINE(VLAD_DEBUG,[yes])
  AC_SUBST(DEBUG,[yes])
  VLAD_CFLAGS="-g $VLAD_CFLAGS"
  VLAD_CXXFLAGS="-g $VLAD_CXXFLAGS"
else
  VLAD_CFLAGS="-O3 $VLAD_CFLAGS"
  VLAD_CXXFLAGS="-O3 $VLAD_CXXFLAGS"
fi

# --enable-memtest
AC_ARG_ENABLE(memtest,AC_HELP_STRING([--enable-memtest],[enable memory testing feature for debugging (default is disabled)]))

if test "x$enable_memtest" != "x" && test "x$enable_memtest" != "xno"; then
  AC_DEFINE(VLAD_MEMTEST,[yes])
  AC_SUBST(MEMTEST,[yes])
fi

# --enable-strip
AC_ARG_ENABLE(strip,AC_HELP_STRING([--enable-strip],[run strip program (default is disabled)]))

if test "x$enable_strip" != "x" && test "x$enable_strip" != "xno"; then
  AC_CHECK_PROG(STRIP,strip,[yes])
fi

# --enable-wrapper
AC_ARG_ENABLE(wrapper,AC_HELP_STRING([--enable-wrapper],[enable C-interface wrapper (default is disabled)]))

if test "x$enable_wrapper" != "x" && test "x$enable_wrapper" != "xno"; then
  AC_DEFINE(VLAD_WRAPPER,[yes])
  AC_SUBST(WRAPPER,[yes])
fi

# --with-smodels
AC_ARG_WITH(smodels, AC_HELP_STRING([--with-smodels],[enable smodels support: yes or path to smodels source tree (default is no)]))

if test "x$with_smodels" != "x" && test "x$with_smodels" != "xno"; then
  AC_DEFINE(VLAD_SMODELS,[yes])
  AC_SUBST(SMODELS,[yes])
  if test "x$with_smodels" = "xyes"; then
    AC_MSG_CHECKING([if smodels is installed and usable])
    OLD_LDFLAGS="$LDFLAGS"
    LDFLAGS="$LDFLAGS -lsmodels"
    AC_TRY_LINK([#include <smodels.h>
                 #include <api.h>],
                [Smodels s;
                 Api a(&s.program);],
                AC_MSG_RESULT([yes]),
                AC_MSG_RESULT([no])
                AC_MSG_ERROR([*** smodels test program failed to compile ***]))
    LDFLAGS="$OLD_LDFLAGS"
  else
    AC_MSG_CHECKING([if smodels path $with_smodels is valid])
    if ! test -d "$with_smodels"; then
      AC_MSG_RESULT([no])
      AC_MSG_ERROR([*** invalid path: $with_smodels ***])
    else
      AC_MSG_RESULT([yes])
      AC_MSG_CHECKING([if smodels in $with_smodels can be used])
      OLD_CXXFLAGS="$CXXFLAGS"
      OLD_LDFLAGS="$LDFLAGS"
      CXXFLAGS="$CXXFLAGS -I$with_smodels"
      LDFLAGS="$LDFLAGS -L$with_smodels/.libs -lsmodels"
      AC_TRY_LINK([#include <smodels.h>
                   #include <api.h>],
                  [Smodels s;
                   Api a(&s.program);],
                  AC_MSG_RESULT([yes]),
                  AC_MSG_RESULT([no])
                  AC_MSG_ERROR([*** smodels test program failed to compile ***]))
      CXXFLAGS="$OLD_CXXFLAGS"
      LDFLAGS="$OLD_LDFLAGS"

      # adjust relative paths and add them to the flags
      case "$with_smodels" in
        /*)
          VLAD_CFLAGS="$VLAD_CFLAGS -I$with_smodels"
          VLAD_CXXFLAGS="$VLAD_CXXFLAGS -I$with_smodels"
          VLAD_LDFLAGS="$VLAD_LDFLAGS -L$with_smodels/.libs"
          ;;
        *)
          VLAD_CFLAGS="$VLAD_CFLAGS -I\$(TOPSRCDIR)/$with_smodels"
          VLAD_CXXFLAGS="$VLAD_CXXFLAGS -I\$(TOPSRCDIR)/$with_smodels"
          VLAD_LDFLAGS="$VLAD_LDFLAGS -L\$(TOPSRCDIR)/$with_smodels/.libs"
          ;;
      esac
    fi
  fi
fi

# --enable-timer
AC_ARG_ENABLE(timer,AC_HELP_STRING([--enable-timer],[enable displaying of computation time (default is disabled)]))

if test "x$enable_timer" != "x" && test "x$enable_timer" != "xno"; then
  AC_DEFINE(VLAD_TIMER,[yes])
  AC_SUBST(TIMER,[yes])
fi

CFLAGS="$VLAD_CFLAGS $CFLAGS"
CXXFLAGS="$VLAD_CXXFLAGS $CXXFLAGS"
LDFLAGS="$VLAD_LDFLAGS $LDFLAGS"

AC_DEFINE_UNQUOTED(VLAD_VERSION,"$PACKAGE_VERSION")
AC_SUBST(VERSION,"$PACKAGE_VERSION")
AC_SUBST(DISTDIR,"\$(TOPSRCDIR)/dist")
AC_SUBST(LIBDIR,"\$(TOPSRCDIR)/lib")

AC_CONFIG_FILES(  \
  Makefile        \
  lib/Makefile    \
  main/Makefile   \
)

AC_OUTPUT
