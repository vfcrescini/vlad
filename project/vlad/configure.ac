# Vino Crescini  <jcrescin@cit.uws.edu.au>

AC_INIT(vlad,0.3.1,jcrescin@cit.uws.edu.au)

AC_CONFIG_HEADERS(config.h)

VLAD_CFLAGS="-ansi -Wall -Wstrict-prototypes -I\$(TOPSRCDIR) -I\$(INCLUDEDIR) -L\$(LIBDIR)"
VLAD_CXXFLAGS="-ansi -Wall -Wstrict-prototypes -I\$(TOPSRCDIR) -I\$(INCLUDEDIR) -L\$(LIBDIR)"
VLAD_LDFLAGS="-L\$(LIBDIR) $LDFLAGS"

AC_PROG_CC
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_LN_S

AC_ARG_ENABLE(debug,AC_HELP_STRING([--enable-debug],[enable debugging output & compile with -g (default is disabled)]))

if test "$enable_debug" != "" && test "$enable_debug" != "no"; then
  AC_DEFINE(DEBUG,1)
  AC_SUBST(DEBUG,1)
  VLAD_CFLAGS="$VLAD_CFLAGS -g"
  VLAD_CXXFLAGS="$VLAD_CXXFLAGS -g"
else
  VLAD_CFLAGS="$VLAD_CFLAGS -O2"
  VLAD_CXXFLAGS="$VLAD_CXXFLAGS -O2"
fi

AC_ARG_ENABLE(strip,AC_HELP_STRING([--enable-strip],[run strip program (default is disabled)]))
if test "$enable_strip" != "" && test "$enable_strip" != "no"; then
  AC_SUBST(STRIP,1)
fi

AC_ARG_WITH(smodels-src,AC_HELP_STRING([--with-smodels-src],[path to smodels source tree (default is disabled)]))

if test "$with_smodels_src" != "" && test "$with_smodels_src" != "no"; then
  AC_DEFINE(SMODELS,1)
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

CFLAGS="$VLAD_CFLAGS $CFLAGS"
CXXFLAGS="$VLAD_CXXFLAGS $CXXFLAGS"
LDFLAGS="$VLAD_LDFLAGS $LDFLAGS"

AC_DEFINE_UNQUOTED(VERSION,"$PACKAGE_VERSION")

AC_SUBST(DISTDIR,"\$(TOPSRCDIR)/dist")
AC_SUBST(BINDIR, "\$(DISTDIR)/bin")
AC_SUBST(INCLUDEDIR, "\$(DISTDIR)/include/vlad")
AC_SUBST(LIBDIR, "\$(DISTDIR)/lib/vlad")

AC_CONFIG_FILES(  \
  Makefile        \
  common/Makefile \
  kb/Makefile     \
  parser/Makefile \
  main/Makefile   \
)

AC_OUTPUT
