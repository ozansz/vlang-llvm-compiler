#include <iostream>
#include "codegen.hpp"
#include "node.hpp"

using namespace std;

extern int yyparse();
extern NProgram* programBlock;

int main()
{
    yyparse();

    CodeGenContext *context = new CodeGenContext();
    context->generateCode(*programBlock);
    context->SaveIRToFile("out.ll");
    
    return 0;
}