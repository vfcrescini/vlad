all : ps pdf

clean :
	$(RM) resume_vino.ps resume_vino.pdf

ps : resume_vino.ps

pdf : resume_vino.pdf

resume_vino.ps : resume_vino.html
	html2ps -n -F -s 1.0 -o $@ resume_vino.html

resume_vino.pdf : resume_vino.ps
	ps2pdf resume_vino.ps $@
