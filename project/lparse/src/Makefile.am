## Process this file with automake to produce Makefile.in

bin_PROGRAMS = lparse

lparse_SOURCES = array.cc parsetree.cc debug.cc extern.cc graph.cc \
		 instance.cc iterate.cc list.cc literal.cc \
		 main.cc predicate.cc rule.cc \
		 symbol.cc term.cc library.cc smodels.h\
		 array.h graph.h literal.h symbol.h debug.h instance.h  \
                 parsetree.h term.h defines.h iterate.h parse.yy lex.ll \
                 extern.h library.h predicate.h global.h list.h rule.h \
		 error.h error.cc smodels.cc sm_smodels.cc sm_smodels.h \
                 priority.h priority.cc

lparse_LDFLAGS = -rdynamic 

EXTRA_DIST = parse.h smodels-mode.el windows_config.h Makefile.Win

YFLAGS = -d

