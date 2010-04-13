ifeq ($(OS),Darwin)
  DOXYGEN = /Applications/Doxygen.app/Contents/Resources/doxygen
else
  DOXYGEN = /usr/bin/doxygen
endif


all: design.pdf doxygen

design.pdf: game.png
	pdflatex design.tex

game.png: game.dia
	dia -t png --export=game.png game.dia

doxygen:
	$(DOXYGEN) Doxyfile

clean:
	rm -rf game.png design.pdf design.log design.aux doxygen

.PHONY: design.pdf
