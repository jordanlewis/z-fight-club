ifeq ($(OS),Darwin)
  DOXYGEN = /Applications/Doxygen.app/Contents/Resources/doxygen
else
  DOXYGEN = /usr/bin/doxygen
endif


all: design.pdf doxygen

design.pdf: game.pdf
	pdflatex design.tex

game.pdf: game.dia
	dia -t pdf --export=game.pdf game.dia

doxygen:
	$(DOXYGEN) Doxyfile

clean:
	rm -rf game.pdf design.pdf design.log design.aux doxygen

.PHONY: design.pdf
