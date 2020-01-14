#!/bin/bash
set -e

rm -f parser.cpp
rm -f parser.hpp
rm -f parser.output
rm -f tokens.cpp

bison -d -o parser.cpp parser.y
flex -o tokens.cpp tokens.l

g++ -I/usr/local/opt/llvm/include -std=c++11 parser.cpp tokens.cpp node.cpp parser_test.cpp -o parser