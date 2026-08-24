.PHONY: all clean test site

all: index.html site
	mkdir -p build

index.html: src/index.tex
	cd build; \
	make4ht -s -c ../src/htstyle.cfg -d ../ ../src/index.tex "fn-in,mathml,mathjax";

clean: 
	rm -rf build/*
	rm -f index.html

test:
	python3 -m http.server 8080

site:
	cd site; \
	cmake --build build/; \
	./build/morosin_github_io_site > index.html; \
	make;

