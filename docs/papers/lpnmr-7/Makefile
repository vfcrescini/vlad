# Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>

LATEX=latex
DVIPS=dvips
PSPDF=ps2pdf
SRC=lpnmr-7
CLASS=llncs.cls

.PHONY :    \
  all       \
  clean     \
  distclean \
  dvi       \
  ps        \
  pdf       \
  $(NULL)

all : dvi ps pdf

clean :
	$(RM) $(SRC).log $(SRC).aux

distclean : clean
	$(RM) $(SRC).dvi $(SRC).ps $(SRC).pdf

dvi : $(SRC).dvi

ps : $(SRC).ps

pdf : $(SRC).pdf

$(SRC).dvi : $(SRC).tex $(CLASS)
	$(LATEX) $(SRC).tex
	$(LATEX) $(SRC).tex

$(SRC).ps : $(SRC).dvi
	$(DVIPS) -o $(@) $(SRC).dvi

$(SRC).pdf : $(SRC).ps
	$(PSPDF) $(SRC).ps $(@)
