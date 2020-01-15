#ifndef __CODEGEN_H
#define __CODEGEN_H

#include <stdio.h>

#include <stack>
#include <llvm/Pass.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Support/raw_ostream.h>

#include "node.hpp"

#define MODULE_NAME "main"
#define ROOT_FUNC   "main"
#define INTRO_CTX   "entry"

#define INTEGER_ID  "int"
#define FLOAT_ID    "real"

using namespace llvm;

static LLVMContext GlobCtx;

class CodeGenBlock {
public:
    BasicBlock *block;
    std::map<std::string, Value*> locals;
};

class CodeGenContext {
    std::stack<CodeGenBlock *> blocks;
    Function *mainFunction;
    Type *integer_type, *real_type;

public:
    Module *module;
    IRBuilder<> *curr_builder;
    std::map<std::string, Value*> globals;
    Function *curr_func;
    std::map<std::string, std::map<std::string, Value*>> symtab;

    CodeGenContext() {
        this->module = new Module(MODULE_NAME, GlobCtx);
        this->integer_type = Type::getInt64Ty(GlobCtx);
        this->real_type = Type::getDoubleTy(GlobCtx);
        this->curr_func = NULL;
    }
    
    void generateCode(NProgram& root);
    void runCode();
    std::map<std::string, Value*>& locals() { return this->symtab[this->curr_func->getName()]; /*return blocks.top()->locals;*/ }
    BasicBlock *currentBlock() { return blocks.top()->block; }
    bool isSymtabEmpty() { return this->blocks.empty(); }
    void pushBlock(BasicBlock *block) { blocks.push(new CodeGenBlock()); blocks.top()->block = block; }
    void popBlock() { CodeGenBlock *top = blocks.top(); blocks.pop(); delete top; }
    IRBuilder<>* GetCurrentBuilder() { return this->curr_builder; }
    Type *GetIntegerType() { return this->integer_type; }
    Type *GetRealType() { return this->real_type; }
    void SaveIRToFile(std::string filename);
};

#endif