# Vino Crescini  <jcrescin@cit.uws.edu.au>

AC_INIT(vlad,0.0.4,jcrescin@cit.uws.edu.au)

AC_CONFIG_HEADERS(config.h)

CFLAGS="-ansi -Wall -Wstrict-prototypes -I. -I\$(TOPSRCDIR) -I\$(TOPSRCDIR)/include -L\$(TOPSRCDIR)\lib"
CXXFLAGS="-ansi -Wall -Wstrict-prototypes -I. -I\$(TOPSRCDIR) -I\$(TOPSRCDIR)/include -L\$(TOPSRCDIR)\lib"
LDFLAGS="-L\$(TOPSRCDIR)/lib"

AC_PROG_CC
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_LN_S

AC_ARG_ENABLE(debug,AC_HELP_STRING([--enable-debug],[default is disabled]))

if test "$enable_debug" != "" && test "$enable_debug" != "no"; then
  AC_DEFINE(DEBUG,1)
  CFLAGS="$CFLAGS -g"
  CXXFLAGS="$CXXFLAGS -g"
else
  CFLAGS="$CFLAGS -O2"
  CXXFLAGS="$CXXFLAGS -O2"
fi

AC_DEFINE_UNQUOTED(VERSION,"$PACKAGE_VERSION")

AC_CONFIG_FILES(  \
  Makefile        \
  common/Makefile \
  kb/Makefile     \
  parser/Makefile \
  main/Makefile   \
)

AC_OUTPUT
