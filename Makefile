POSTS=$(shell find src/posts -name *.tex -exec basename {} .tex \; | sort | tac)

.PHONY: all clean test

all: index.html
	mkdir -p build

index.html: src/index.tex
	cd build; \
	make4ht -s -c ../src/htstyle.cfg -d ../ ../src/index.tex "fn-in,mathml,mathjax";

clean: 
	rm -rf build/*
	rm -f index.html

test:
	python3 -m http.server 8080
