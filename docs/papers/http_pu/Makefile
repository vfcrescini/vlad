# Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>

LATEX=latex
DVIPS=dvips
PSPDF=ps2pdf
FIG2DEV=fig2dev

LATEX_SRCS=http_pu
XFIG_SRCS=

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

http_pu.dvi : http_pu.tex $(CLASS)
	$(LATEX) http_pu.tex
	$(LATEX) http_pu.tex

http_pu.ps : http_pu.dvi
	$(DVIPS) -o $(@) http_pu.dvi

http_pu.pdf : http_pu.ps
	$(PSPDF) http_pu.ps $(@)
