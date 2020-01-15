#include <iostream>
#include "node.hpp"
extern NProgram* programBlock;
extern int yyparse();

int main(int argc, char **argv)
{
    yyparse();

    std::cout << programBlock << std::endl;
    programBlock->DumpNode();

    return 0;
}