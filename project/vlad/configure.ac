# Vino Crescini  <jcrescin@cit.uws.edu.au>

AC_INIT(vlad,0.0.1,jcrescin@cit.uws.edu.au)
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LEX
if test "${LEX}" != "flex"; then
	AC_MSG_ERROR(cannot find flex)
fi
AC_PROG_YACC
if test "${YACC}" != "bison -y"; then
	AC_MSG_ERROR(cannot find bison)
fi
AC_PROG_LN_S
AC_ARG_ENABLE(debug,AC_HELP_STRING([--enable-debug],[default is disabled]))
AC_SUBST(enable_debug)
AC_CONFIG_FILES(  \
  Makefile        \
  common/Makefile \
  parser/Makefile \
  main/Makefile   \
)
AC_OUTPUT
