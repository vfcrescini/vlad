# Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>

LATEX=latex
DVIPS=dvips
PSPDF=ps2pdf
FIG2DEV=fig2dev

LATEX_SRCS=policyupdater
XFIG_SRCS=

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
	$(foreach file,$(LATEX_SRCS),$(RM) $(file).log $(file).aux $(file).cb)

distclean : clean
	$(foreach file,$(LATEX_SRCS),$(RM) $(file).dvi $(file).ps $(file).pdf)
	$(foreach file,$(XFIG_SRCS),$(RM) $(file).eps)

eps : $(patsubst %,%.eps,$(XFIG_SRCS))

dvi : $(patsubst %,%.dvi,$(LATEX_SRCS))

ps : $(patsubst %,%.ps,$(LATEX_SRCS))

pdf : $(patsubst %,%.pdf,$(LATEX_SRCS))

policyupdater.dvi : policyupdater.tex $(CLASS) $(STYLE)
	$(LATEX) policyupdater.tex
	$(LATEX) policyupdater.tex

policyupdater.ps : policyupdater.dvi
	$(DVIPS) -o $(@) policyupdater.dvi

policyupdater.pdf : policyupdater.ps
	$(PSPDF) policyupdater.ps $(@)
