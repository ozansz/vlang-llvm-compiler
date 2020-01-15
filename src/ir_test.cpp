#include <iostream>
#include "codegen.hpp"
#include "node.hpp"

using namespace std;

extern int yyparse();
extern NProgram* programBlock;

int main()
{
    yyparse();
    std::cout << programBlock << std::endl;

    CodeGenContext *context = new CodeGenContext();
    context->generateCode(*programBlock);
    context->runCode();
    
    return 0;
}