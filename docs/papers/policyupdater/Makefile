# Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>

LATEX=latex
DVIPS=dvips
PSPDF=ps2pdf
FIG2DEV=fig2dev

LATEX_SRCS=policyupdater
XFIG_SRCS=system list

CLASS=
STYLE=latex8.sty

.PHONY :    \
  all       \
  clean     \
  distclean \
  dvi       \
  ps	    \
  pdf       \
  eps       \
  $(NULL)

all : eps dvi ps pdf

all : eps dvi ps pdf

clean :
	$(RM) $(foreach file,$(LATEX_SRCS),$(file).log $(file).aux $(file).cb)

distclean : clean
	$(RM) $(foreach file,$(LATEX_SRCS),$(file).dvi $(file).ps $(file).pdf)
	$(RM) $(foreach file,$(XFIG_SRCS),$(file).eps)

eps : $(patsubst %,%.eps,$(XFIG_SRCS))

dvi : $(patsubst %,%.dvi,$(LATEX_SRCS))

ps : $(patsubst %,%.ps,$(LATEX_SRCS))

pdf : $(patsubst %,%.pdf,$(LATEX_SRCS))

policyupdater.dvi : policyupdater.tex system.fig $(CLASS) $(STYLE) 
	$(LATEX) policyupdater.tex
	$(LATEX) policyupdater.tex

%.ps : %.dvi
	$(DVIPS) -o $(@) $(<)

%.pdf : %.ps
	$(PSPDF) $(<) $(@)

%.eps : %.fig
	$(FIG2DEV) -L eps $(<) $(@) || $(FIG2DEV) -L ps $(<) $(@)
