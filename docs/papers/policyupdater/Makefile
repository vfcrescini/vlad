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
	$(RM) $(foreach f,$(LATEX_SRCS),$(f).log $(f).aux $(f).cb)

distclean : clean
	$(RM) $(foreach f,$(LATEX_SRCS),$(f).dvi $(f).ps $(f).pdf)
	$(RM) $(foreach f,$(XFIG_SRCS),$(f).eps $(f).pdf)

dvi : $(foreach f,$(LATEX_SRCS),$(f).dvi)

pdf : $(foreach f,$(LATEX_SRCS),$(f).pdf)

ps : $(foreach f,$(LATEX_SRCS),$(f).ps)

eps : $(foreach f,$(XFIG_SRCS),$(f).eps)

$(foreach f,$(LATEX_SRCS),$(f).ps) : $(patsubst %.ps,%.dvi,$(@))
	$(DVIPS) -o $(@) $(patsubst %.ps,%.dvi,$(@))

$(foreach f,$(XFIG_SRCS),$(f).pdf) : $(patsubst %.pdf,%.eps,$(@))
	$(EPSPDF) $(patsubst %.pdf,%.eps,$(@)) --outfile=$(@)

$(foreach f,$(XFIG_SRCS),$(f).eps) : $(patsubst %.eps,%.fig,$(@))
	$(FIG2DEV) -L eps $(patsubst %.eps,%.fig,$(@)) $(@)

policyupdater.dvi : policyupdater.tex $(foreach f,$(XFIG_SRCS),$(f).eps) $(CLASS) $(STYLE)
	$(LATEX) policyupdater.tex

policyupdater.pdf : policyupdater.tex $(foreach f,$(XFIG_SRCS),$(f).pdf) $(CLASS) $(STYLE)
	$(PDFLATEX) policyupdater.tex
