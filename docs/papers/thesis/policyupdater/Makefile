# Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>

LATEX=latex
DVIPS=dvips
PSPDF=ps2pdf
EPSPDF=epstopdf
PDFLATEX=pdflatex
FIG2DEV=fig2dev

LATEX_SRCS=policyupdater
XFIG_SRCS=      \
  acc-ctrl      \
  rels          \
  net-01        \
  net-02        \
  net-03        \
  net-04        \
  net-05        \
  net-06        \
  net-07        \
  sys-01        \
  sys-02        \
  sys-03        \
  sys-04        \
  net-struct-01 \
  net-struct-02 \
  net-struct-03 \
  net-struct-04 \
  net-struct-05 \

CLASS=
STYLE=
MISC=

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
ifdef LATEX_SRCS
	$(RM) $(foreach f,$(LATEX_SRCS),$(f).log $(f).aux $(f).cb $(f).toc $(f).lof $(f).lot)
endif

distclean : clean
ifdef LATEX_SRCS
	$(RM) $(foreach f,$(LATEX_SRCS),$(f).dvi $(f).ps $(f).pdf)
endif
ifdef XFIG_SRCS
	$(RM) $(foreach f,$(XFIG_SRCS),$(f).eps $(f).pdf)
endif

dvi : $(foreach f,$(LATEX_SRCS),$(f).dvi)

pdf : $(foreach f,$(LATEX_SRCS),$(f).pdf)

ps : $(foreach f,$(LATEX_SRCS),$(f).ps)

eps : $(foreach f,$(XFIG_SRCS),$(f).eps)

# static pattern rules

$(foreach f,$(LATEX_SRCS),$(f).ps) : %.ps : %.dvi
	$(DVIPS) -o $(@) $(patsubst %.ps,%.dvi,$(@))

$(foreach f,$(XFIG_SRCS),$(f).pdf) : %.pdf : %.eps
	$(EPSPDF) $(patsubst %.pdf,%.eps,$(@)) --outfile=$(@)

$(foreach f,$(XFIG_SRCS),$(f).eps) : %.eps : %.fig
	$(FIG2DEV) -L eps $(patsubst %.eps,%.fig,$(@)) $(@)

policyupdater.dvi : policyupdater.tex $(foreach f,$(XFIG_SRCS),$(f).eps) $(CLASS) $(STYLE) $(MISC)
	$(LATEX) policyupdater.tex
	$(LATEX) policyupdater.tex

policyupdater.pdf : policyupdater.tex $(foreach f,$(XFIG_SRCS),$(f).pdf) $(CLASS) $(STYLE) $(MISC)
	$(PDFLATEX) policyupdater.tex
	$(PDFLATEX) policyupdater.tex
