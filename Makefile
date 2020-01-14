CPPFLAGS=-I/usr/local/opt/llvm/include -std=c++11
CXX=g++
RM=rm -f
BISON_FLAGS=-d
TESTFILES=$(ls tests/*.v)

all: clean parser

parser: lexer
	$(CXX) $(CPPFLAGS) parser.cpp tokens.cpp node.cpp parser_test.cpp -o parser

lexer:
	bison $(BISON_FLAGS) parser.y -o parser.cpp
	flex -o tokens.cpp tokens.l

tests: parser
	for tf in `ls tests/*.v`; do \
		echo "\n\n[+] Testing $$tf..."; \
        cat $$tf | ./parser  ; \
    done

clean:
	$(RM) *.hh parser.cpp parser.hpp tokens.cpp parser

.PHONY: clean tests