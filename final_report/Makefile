FILE = surbatovich_ruppel_report

all:
	pdflatex -shell-escape $(FILE).tex
	pdflatex -shell-escape $(FILE).tex
	bibtex $(FILE) 
	pdflatex -shell-escape $(FILE).tex
	pdflatex -shell-escape $(FILE).tex
	rm -f $(FILE).aux  $(FILE).blg  $(FILE).dvi $(FILE).ps  $(FILE).out  *.backup *.bak  *~

	
fast:
	latex $(FILE)
	dvips -t letter $(FILE).dvi
	ps2pdf $(FILE).ps
	rm -f $(FILE).aux $(FILE).bbl $(FILE).blg $(FILE).log $(FILE).dvi $(FILE).ps $(FILE).out

clean:
	rm -f $(FILE).aux $(FILE).bbl $(FILE).blg $(FILE).log  $(FILE).dvi $(FILE).ps $(FILE).out 