# This file is part of PolicyUpdater.
#
# Copyright (C) 2003, 2004 University of Western Sydney
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
AC_INIT(mod_vlad,0.3.5,jcrescin@cit.uws.edu.au)

AC_CONFIG_HEADERS(config.h)

# set language
AC_LANG(C)

# set vars to itself to prevent the macros from setting default value
CFLAGS="$CFLAGS"
LDFLAGS="$LDFLAGS"

# set flags
MODVLAD_CFLAGS="-Wall -Wshadow -Wcast-align -Wredundant-decls -Wnested-externs"
MODVLAD_LDFLAGS=""

# some tests
AC_PROG_CC
AC_PROG_LEX
AC_PROG_YACC

# check compiler options
cat >> $$.c << \EOF
int main(void) {return 0;}
EOF

# check if -fPIC works
AC_MSG_CHECKING([whether $CC accepts -fPIC])
if "${CC}" -fPIC -o $$ $$.c > /dev/null 2>&1; then
  MODVLAD_FPIC="PIC"
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])

  # no? try -fpic
  AC_MSG_CHECKING([whether $CC accepts -fpic])
  if "${CC}" -fpic -o $$ $$.c > /dev/null 2>&1; then
    MODVLAD_FPIC="pic"
    AC_MSG_RESULT([yes])
  else
    AC_MSG_RESULT([no])
    MODVLAD_FPIC="no"
  fi
fi

# check if -shared works
AC_MSG_CHECKING([whether $CC compiler accepts -shared])
if "${CC}" -shared -o $$ $$.c; then
  MODVLAD_SHARED="yes"
  AC_MSG_RESULT([yes])
else
  MODVLAD_SHARED="no"
  AC_MSG_RESULT([no])
fi

# make sure they both are supported
if test "x$MODVLAD_FPIC" = "xno" || test "x$MODVLAD_SHARED" = "xno"; then
  AC_MSG_ERROR([*** $CC must support both -shared and -fPIC or -fpic ***])
else
  AC_SUBST(PICFLAG,[-f$MODVLAD_FPIC])
fi

# cleanup
rm -f $$ $$.c

# --enable-debug
AC_ARG_ENABLE(debug,AC_HELP_STRING([--enable-debug],[enable debugging output & compile with -g (default is disabled)]))

if test "x$enable_debug" != "x" && test "x$enable_debug" != "xno"; then
  AC_DEFINE(MODVLAD_DEBUG,[yes])
  AC_SUBST(DEBUG,[yes])
  MODVLAD_CFLAGS="-g $MODVLAD_CFLAGS"
else
  MODVLAD_CFLAGS="-O3 $MODVLAD_CFLAGS"
fi

# --enable-strip
AC_ARG_ENABLE(strip,AC_HELP_STRING([--enable-strip],[run strip program (default is disabled)]))

if test "x$enable_strip" != "x" && test "x$enable_strip" != "xno"; then
  AC_CHECK_PROG(STRIP,strip,[yes])
fi

# --with-libvlad
AC_ARG_WITH(libvlad, AC_HELP_STRING([--with-libvlad],[yes or path to libvlad (default is yes)]),,with_libvlad=[yes])

if test "x$with_libvlad" != "x" && test "x$with_libvlad" != "xno"; then
  if test "x$with_libvlad" = "xyes"; then
    AC_MSG_CHECKING([if libvlad is installed and usable])
    AC_TRY_LINK([#include <vlad/vlad.h>],
                [#ifndef VLAD_SMODELS
                 * no smodels support *
                 #endif
                 #ifndef VLAD_WRAPPER
                 * no c-wrapper support *
                 #endif
                 int test = VLAD_OK],
                AC_MSG_RESULT([yes]),
                AC_MSG_RESULT([no])
                AC_MSG_ERROR([*** libvlad test program failed to compile ***]))
  else
    AC_MSG_CHECKING([if libvlad path $with_libvlad is valid])
    if ! test -d "$with_libvlad"; then
      AC_MSG_RESULT([no])
      AC_MSG_ERROR([*** invalid path: $with_libvlad ***])
    else
      AC_MSG_RESULT([yes])
      AC_MSG_CHECKING([if libvlad in $with_libvlad can be used])
      OLD_CFLAGS="$CFLAGS"
      CFLAGS="$CFLAGS -I$with_libvlad/include"
      AC_TRY_LINK([#include <vlad/vlad.h>],
                  [#ifndef VLAD_SMODELS
                   * no smodels support *
                   #endif
                   #ifndef VLAD_WRAPPER
                   * no c-wrapper support *
                   #endif
                   int test = VLAD_OK],
                  AC_MSG_RESULT([yes]),
                  AC_MSG_RESULT([no])
                  AC_MSG_ERROR([*** libvlad test program failed to compile ***]))
      CFLAGS="$OLD_CFLAGS"
      # adjust relative paths and add them to the flags
      case "$with_libvlad" in
        /*)
          MODVLAD_CFLAGS="$MODVLAD_CFLAGS -I$with_libvlad/include"
          MODVLAD_LDFLAGS="$MODVLAD_LDFLAGS -L$with_libvlad/lib"
          ;;
        *)
          MODVLAD_CFLAGS="$MODVLAD_CFLAGS -I\$(TOPSRCDIR)/$with_libvlad/include"
          MODVLAD_LDFLAGS="$MODVLAD_LDFLAGS -L\$(TOPSRCDIR)/$with_libvlad/lib"
          ;;
      esac
    fi
  fi
else
  AC_MSG_ERROR([*** package requires libvlad ***])
fi

# --with-apache
AC_ARG_WITH(apache, AC_HELP_STRING([--with-apache],[yes or path to apache (default is yes)]),,with_apache=[yes])

if test "x$with_apache" != "x" && test "x$with_apache" != "xno"; then
  if test "x$with_apache" = "xyes"; then
    AC_MSG_CHECKING([if apache is installed and usable])
    AC_TRY_LINK([#include <httpd.h>],
                [const char *i = AP_SERVER_PROTOCOL;],
                AC_MSG_RESULT([yes]),
                AC_MSG_RESULT([no])
                AC_MSG_ERROR([*** apache test program failed to compile ***]))
  else
    AC_MSG_CHECKING([if apache path $with_apache is valid])
    if ! test -d "$with_apache"; then
      AC_MSG_RESULT([no])
      AC_MSG_ERROR([*** invalid path: $with_apache ***])
    else
      AC_MSG_RESULT([yes])
      AC_MSG_CHECKING([if apache in $with_apache can be used])
      OLD_CFLAGS="$CFLAGS"
      CFLAGS="$CFLAGS -I$with_apache/include"
      AC_TRY_LINK([#include <httpd.h>],
                  [const char *i = AP_SERVER_PROTOCOL;],
                  AC_MSG_RESULT([yes]),
                  AC_MSG_RESULT([no])
                  AC_MSG_ERROR([*** apache test program failed to compile ***]))
      CFLAGS="$OLD_CFLAGS"
      # adjust relative paths and add them to the flags
      case "$with_apache" in
        /*)
          MODVLAD_CFLAGS="$MODVLAD_CFLAGS -I$with_apache/include"
          ;;
        *)
          MODVLAD_CFLAGS="$MODVLAD_CFLAGS -I\$(TOPSRCDIR)/$with_apache/include"
          ;;
      esac
    fi
  fi
else
  AC_MSG_ERROR([*** package requires apache ***])
fi

CFLAGS="$MODVLAD_CFLAGS $CFLAGS"
LDFLAGS="$MODVLAD_LDFLAGS $LDFLAGS"

AC_DEFINE_UNQUOTED(MODVLAD_VERSION,"$PACKAGE_VERSION")
AC_SUBST(VERSION,"$PACKAGE_VERSION")
AC_CONFIG_FILES(Makefile)

AC_OUTPUT
