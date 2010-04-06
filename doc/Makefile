all: design.pdf

design.pdf: game.pdf
	pdflatex design.tex

game.pdf: game.dia
	dia -t pdf --export=game.pdf game.dia

clean:
	rm -f game.pdf design.pdf design.log design.aux
