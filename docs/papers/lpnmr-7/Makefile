# Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>

LATEX=latex
DVIPS=dvips
PSPDF=ps2pdf
FIG2DEV=fig2dev

LATEX_SRCS=lpnmr-7
XFIG_SRCS=fig1

CLASS=llncs.cls

.PHONY :    \
  all       \
  clean     \
  distclean \
  dvi       \
  ps        \
  pdf       \
  eps       \
  $(NULL)

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

lpnmr-7.dvi : lpnmr-7.tex fig1.eps $(CLASS)
	$(LATEX) lpnmr-7.tex
	$(LATEX) lpnmr-7.tex

lpnmr-7.ps : lpnmr-7.dvi
	$(DVIPS) -o $(@) lpnmr-7.dvi

lpnmr-7.pdf : lpnmr-7.ps
	$(PSPDF) lpnmr-7.ps $(@)

fig1.eps : fig1.fig
	$(FIG2DEV) -L eps fig1.fig $(@)
