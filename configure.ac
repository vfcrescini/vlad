# Vino Crescini  <jcrescin@cit.uws.edu.au>

AC_INIT(vlad,0.3.1,jcrescin@cit.uws.edu.au)

AC_CONFIG_HEADERS(config.h)

TMP_CFLAGS="-ansi -Wall -Wstrict-prototypes -I. -I\$(TOPSRCDIR) -I\$(TOPSRCDIR)/include -L\$(TOPSRCDIR)/lib"
TMP_CXXFLAGS="-ansi -Wall -Wstrict-prototypes -I. -I\$(TOPSRCDIR) -I\$(TOPSRCDIR)/include -L\$(TOPSRCDIR)/lib"
TMP_LDFLAGS="-L\$(TOPSRCDIR)/lib $LDFLAGS"

AC_PROG_CC
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_LN_S

AC_ARG_ENABLE(debug,AC_HELP_STRING([--enable-debug],[enable debugging output & compile with -g (default is disabled)]))

if test "$enable_debug" != "" && test "$enable_debug" != "no"; then
  AC_DEFINE(DEBUG,1)
  AC_SUBST(DEBUG,1)
  TMP_CFLAGS="$TMP_CFLAGS -g"
  TMP_CXXFLAGS="$TMP_CXXFLAGS -g"
else
  TMP_CFLAGS="$TMP_CFLAGS -O2"
  TMP_CXXFLAGS="$TMP_CXXFLAGS -O2"
fi

AC_ARG_ENABLE(strip,AC_HELP_STRING([--enable-strip],[run strip program (default is disabled)]))
if test "$enable_strip" != "" && test "$enable_strip" != "no"; then
  AC_SUBST(STRIP,1)
fi

AC_ARG_WITH(smodels-src,AC_HELP_STRING([--with-smodels-src],[path to smodels source tree (default is disabled)]))

if test "$with_smodels_src" != "" && test "$with_smodels_src" != "no"; then
  AC_SUBST(SMODELS,1)
  if test "$with_smodels_src" == "yes"; then
    AC_SUBST(SMODELS_PATH,[yes])
  else
    if ! test -d "$withval"; then
      AC_MSG_ERROR([*** invalid path: $with_smodels_src ***])
    else
      AC_SUBST(SMODELS_PATH,[$with_smodels_src])
    fi
  fi
fi

AC_DEFINE_UNQUOTED(VERSION,"$PACKAGE_VERSION")

CFLAGS="$TMP_CFLAGS $CFLAGS"
CXXFLAGS="$TMP_CXXFLAGS $CXXFLAGS"
LDFLAGS="$TMP_LDFLAGS $LDFLAGS"

AC_CONFIG_FILES(  \
  Makefile        \
  common/Makefile \
  kb/Makefile     \
  parser/Makefile \
  main/Makefile   \
)

AC_OUTPUT
