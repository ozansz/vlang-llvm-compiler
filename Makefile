LLVMFLAGS=$(shell llvm-config --cxxflags --ldflags --system-libs --libs)
CPPFLAGS=-w
CXX=g++
RM=rm -f
BISON_FLAGS=-d
TESTFILES=$(ls tests/*.v)

all: clean ir compiler

compiler: parser
	$(CXX) $(CPPFLAGS) $(LLVMFLAGS) src/parser.cpp src/tokens.cpp src/node.cpp src/codegen.cpp src/compiler.cpp -o compiler

ir: parser
	$(CXX) $(CPPFLAGS) $(LLVMFLAGS) src/parser.cpp src/tokens.cpp src/node.cpp src/codegen.cpp src/ir_test.cpp -o irgen

parser: lexer
	$(CXX) $(CPPFLAGS) $(LLVMFLAGS) src/parser.cpp src/tokens.cpp src/node.cpp src/codegen.cpp src/parser_test.cpp -o parser

lexer:
	bison $(BISON_FLAGS) src/parser.y -o src/parser.cpp
	flex -o src/tokens.cpp src/tokens.l

tests: ir
	for tf in `ls tests/*.v`; do \
		echo "\n\n[+] Testing $$tf..."; \
		cat $$tf | ./parser  ; \
        cat $$tf | ./irgen  ; \
    done

clean:
	$(RM) src/*.hh src/parser.cpp src/parser.hpp src/tokens.cpp parser irgen compiler *.ll

.PHONY: clean tests