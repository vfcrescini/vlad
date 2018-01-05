# Vino Crescini  <jcrescin@cit.uws.edu.au>

AC_INIT(vlad,0.7.1,jcrescin@cit.uws.edu.au)

AC_CONFIG_HEADERS(config.h)

# set language
AC_LANG(C++)

# set vars to itself to prevent the macros from setting default value
CFLAGS="$CFLAGS"
CXXFLAGS="$CXXFLAGS"

# set flags
VLAD_CFLAGS="-ansi -Wall -Wstrict-prototypes -I\$(TOPSRCDIR) -I\$(DISTDIR)/include"
VLAD_CXXFLAGS="-ansi -Wall -Wstrict-prototypes -I\$(TOPSRCDIR) -I\$(DISTDIR)/include"
VLAD_LDFLAGS="-L\$(DISTDIR)/lib/vlad"

# some tests
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_LN_S

# --enable-debug
AC_ARG_ENABLE(debug,AC_HELP_STRING([--enable-debug],[enable debugging output & compile with -g (default is disabled)]))

if test "x$enable_debug" != "x" && test "x$enable_debug" != "xno"; then
  AC_DEFINE(DEBUG,[yes])
  AC_SUBST(DEBUG,[yes])
  VLAD_CFLAGS="-g $VLAD_CFLAGS"
  VLAD_CXXFLAGS="-g $VLAD_CXXFLAGS"
else
  VLAD_CFLAGS="-O3 $VLAD_CFLAGS"
  VLAD_CXXFLAGS="-O3 $VLAD_CXXFLAGS"
fi

# --enable-strip
AC_ARG_ENABLE(strip,AC_HELP_STRING([--enable-strip],[run strip program (default is disabled)]))

if test "x$enable_strip" != "x" && test "x$enable_strip" != "xno"; then
  AC_CHECK_PROG(STRIP,strip,[yes])
fi

# --with-smodels
AC_ARG_WITH(smodels, AC_HELP_STRING([--with-smodels],[enable smodels support: yes or path to smodels source tree (default is no)]))

if test "x$with_smodels" != "x" && test "x$with_smodels" != "xno"; then
  AC_DEFINE(SMODELS,[yes])
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

CFLAGS="$VLAD_CFLAGS $CFLAGS"
CXXFLAGS="$VLAD_CXXFLAGS $CXXFLAGS"
LDFLAGS="$VLAD_LDFLAGS $LDFLAGS"

AC_DEFINE_UNQUOTED(VERSION,"$PACKAGE_VERSION")

AC_SUBST(DISTDIR,"\$(TOPSRCDIR)/dist")

AC_CONFIG_FILES(  \
  Makefile        \
  common/Makefile \
  kb/Makefile     \
  parser/Makefile \
  main/Makefile   \
)

AC_OUTPUT
