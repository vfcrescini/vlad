# Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>

LATEX=latex
DVIPS=dvips
PSPDF=ps2pdf
EPSPDF=epstopdf
PDFLATEX=pdflatex
FIG2DEV=fig2dev

LATEX_SRCS=policyupdater
XFIG_SRCS=figure-01 figure-02 figure-03

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

clean :
	$(RM) $(foreach file,$(LATEX_SRCS),$(file).log $(file).aux $(file).cb)

distclean : clean
	$(RM) $(foreach file,$(LATEX_SRCS),$(file).dvi $(file).ps $(file).pdf)
	$(RM) $(foreach file,$(XFIG_SRCS),$(file).eps $(file).pdf)

dvi : $(patsubst %,%.dvi,$(LATEX_SRCS))

pdf : $(patsubst %,%.pdf,$(LATEX_SRCS))

ps : $(patsubst %,%.ps,$(LATEX_SRCS))

eps : $(patsubst %,%.eps,$(XFIG_SRCS))

$(patsubst %,%.ps,$(LATEX_SRCS)) : $(patsubst %,%.dvi,$(LATEX_SRCS))
	$(DVIPS) -o $(@) $(<)

$(patsubst %,%.pdf,$(XFIG_SRCS)) : $(patsubst %,%.eps,$(XFIG_SRCS))
	$(EPSPDF) $(<) --outfile=$(@)

$(patsubst %,%.eps,$(XFIG_SRCS)) : $(patsubst %,%.fig,$(XFIG_SRCS))
	$(FIG2DEV) -L eps $(<) $(@) || $(FIG2DEV) -L ps $(<) $(@)

policyupdater.dvi : policyupdater.tex $(patsubst %,%.eps,$(XFIG_SRCS)) $(CLASS) $(STYLE)
	$(LATEX) policyupdater.tex
	$(LATEX) policyupdater.tex

policyupdater.pdf : policyupdater.tex $(patsubst %,%.pdf,$(XFIG_SRCS)) $(CLASS) $(STYLE)
	$(PDFLATEX) policyupdater.tex
	$(PDFLATEX) policyupdater.tex
