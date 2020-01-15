#include "node.hpp"
#include "codegen.hpp"
#include "parser.hpp"

#include <stdlib.h>

#define ADDRSPC 0

static void err_and_halt(std::string msg) {
    std::cout << std::endl << "[ERROR] " << msg << std::endl;
    abort();
}

static Function* printf_prototype(LLVMContext& ctx, Module *mod)
{
    std::vector<Type*> printf_arg_types;
    printf_arg_types.push_back(Type::getInt8PtrTy(ctx));

    FunctionType* printf_type =
        FunctionType::get(
            Type::getInt32Ty(ctx), printf_arg_types, true);

    Function *func = Function::Create(
                printf_type, Function::ExternalLinkage,
                Twine("printf"),
                mod
           );
    func->setCallingConv(CallingConv::C);
    return func;
}

/* Compile the AST into a module */
void CodeGenContext::generateCode(NProgram& root)
{
    root.codeGen(*this);
}

/* Executes the AST by running the main function */
void CodeGenContext::runCode() {
    this->module->print(outs(), nullptr);
}

void CodeGenContext::SaveIRToFile(std::string filename) {
    std::error_code code;
    raw_fd_ostream stream(filename, code);
    this->module->print(stream, nullptr);
}

/* Returns an LLVM type based on the identifier */
static Type *typeOf(const NIdentifier& type, CodeGenContext& ctx) 
{
    if (type.name == "int") {
        return ctx.GetIntegerType();
    }
    else if (type.name == "real") {
        
        return ctx.GetRealType();
    }

    err_and_halt("CodegenError<NIdentifier>: Unknown type identifier " + type.name);
}

/* -- Code Generation -- */

Value* NExpression::codeGen(CodeGenContext& context)
{ 
    if (dynamic_cast<NInteger*>(this) != nullptr)
        return dynamic_cast<NInteger*>(this)->codeGen(context);
    else if (dynamic_cast<NReal*>(this) != nullptr)
        return dynamic_cast<NReal*>(this)->codeGen(context);
    else if (dynamic_cast<NStringLiteral*>(this) != nullptr)
        return dynamic_cast<NStringLiteral*>(this)->codeGen(context);
    else if (dynamic_cast<NIdentifier*>(this) != nullptr)
        return dynamic_cast<NIdentifier*>(this)->codeGen(context);
    else if (dynamic_cast<NVariable*>(this) != nullptr)
        return dynamic_cast<NVariable*>(this)->codeGen(context);
    else if (dynamic_cast<NFunctionCall*>(this) != nullptr)
        return dynamic_cast<NFunctionCall*>(this)->codeGen(context);
    else if (dynamic_cast<NBinaryOp*>(this) != nullptr)
        return dynamic_cast<NBinaryOp*>(this)->codeGen(context);
    else if (dynamic_cast<NUnaryOp*>(this) != nullptr)
        return dynamic_cast<NUnaryOp*>(this)->codeGen(context);
    else
        return NULL;
}

Value* NStatement::codeGen(CodeGenContext& context)
{
    if (dynamic_cast<NAssignment*>(this) != nullptr)
        return dynamic_cast<NAssignment*>(this)->codeGen(context);
    else if (dynamic_cast<NExpressionStatement*>(this) != nullptr)
        return dynamic_cast<NExpressionStatement*>(this)->codeGen(context);
    else if (dynamic_cast<NVariableDecl*>(this) != nullptr)
        return dynamic_cast<NVariableDecl*>(this)->codeGen(context);
    else if (dynamic_cast<NVariableCompoundDecl*>(this) != nullptr)
        return dynamic_cast<NVariableCompoundDecl*>(this)->codeGen(context);
    else if (dynamic_cast<NFunctionDecl*>(this) != nullptr)
        return dynamic_cast<NFunctionDecl*>(this)->codeGen(context);
    else if (dynamic_cast<NIfStatement*>(this) != nullptr)
        return dynamic_cast<NIfStatement*>(this)->codeGen(context);
    else if (dynamic_cast<NForStatement*>(this) != nullptr)
        return dynamic_cast<NForStatement*>(this)->codeGen(context);
    else if (dynamic_cast<NWhileStatement*>(this) != nullptr)
        return dynamic_cast<NWhileStatement*>(this)->codeGen(context);
    else if (dynamic_cast<NPrintStatement*>(this) != nullptr)
        return dynamic_cast<NPrintStatement*>(this)->codeGen(context);
    else if (dynamic_cast<NReadStatement*>(this) != nullptr)
        return dynamic_cast<NReadStatement*>(this)->codeGen(context);
    else if (dynamic_cast<NReturnStatement*>(this) != nullptr)
        return dynamic_cast<NReturnStatement*>(this)->codeGen(context);
    else
        return NULL;
}

Value* NInteger::codeGen(CodeGenContext& context)
{  
    return ConstantInt::get(context.GetIntegerType(), this->value, true);
}

Value* NReal::codeGen(CodeGenContext& context)
{
    return ConstantFP::get(context.GetRealType(), this->value);
}

Value* NStringLiteral::codeGen(CodeGenContext& context)
{
    std::vector<Constant*> char_values;

    ArrayType* arr_type = ArrayType::get(context.GetIntegerType(), this->value.length() + 1);

    for (int i = 0; i < this->value.length(); i++)
        char_values.push_back(Constant::getIntegerValue(context.GetIntegerType(), APInt(ADDRSPC, int(this->value[i]), true)));

    return ConstantArray::get(arr_type, char_values);
}

Value* NIdentifier::codeGen(CodeGenContext& context)
{
    Value *var_ptr;

    if (context.locals().find(this->name) == context.locals().end()) {
        if (context.globals.find(this->name) == context.globals.end()) {
            err_and_halt("undeclared variable " + this->name);
        } else {
            var_ptr = context.globals[this->name];
        }
    } else {
        var_ptr = context.locals()[this->name];
    }

    return new LoadInst(var_ptr, "", false, context.currentBlock());
}

Value* NVariable::codeGen(CodeGenContext& context)
{
    Value *var_ptr;

    if (context.locals().find(this->identifier.name) == context.locals().end()) {
        if (context.globals.find(this->identifier.name) == context.globals.end()) {
            err_and_halt("undeclared variable " + this->identifier.name);
        } else {
            var_ptr = context.globals[this->identifier.name];
        }
    } else {
        var_ptr = context.locals()[this->identifier.name];
    }

    switch (this->type) {
        case VARIABLE_BASIC: return new LoadInst(var_ptr, "", false, context.currentBlock());
        case VARIABLE_ARRAY:
            Value* arr_index_val = this->arr_size.codeGen(context);
            
            if (arr_index_val->getType() != context.GetIntegerType())
                err_and_halt("CodeGen<NVariable>: Non-integer array index (" + this->identifier.name + ")");
            
            
            std::vector<Value*> index_vect;
            index_vect.push_back(arr_index_val);
            Value *gep = GetElementPtrInst::CreateInBounds(var_ptr, index_vect, "", context.currentBlock());
            
            return new LoadInst(gep, "", false, context.currentBlock());
    }

    err_and_halt("CodeGen<NVariable>: Undefined type attribute: " + std::to_string(this->type) + "(" + this->identifier.name + ")");
}

Value* NFunctionCall::codeGen(CodeGenContext& context)
{
    

    Function *function = context.module->getFunction(this->id.name.c_str());
    if (function == NULL) {
        
    }
    std::vector<Value*> args;
    ExpressionList::const_iterator it;
    for (it = arguments.begin(); it != arguments.end(); it++) {
        args.push_back((**it).codeGen(context));
    }
    CallInst *call = CallInst::Create(function, args, "", context.currentBlock());
    
    return call;
}

Value* NBinaryOp::codeGen(CodeGenContext& context)
{
    Instruction::BinaryOps instr;
    CmpInst::Predicate pred;

    switch (this->op) {
        case TCEQ:      pred = CmpInst::Predicate::ICMP_EQ; goto logic;
        case TCNE:      pred = CmpInst::Predicate::ICMP_NE; goto logic;
        case TCLT:      pred = CmpInst::Predicate::ICMP_SLT; goto logic;
        case TCLE:      pred = CmpInst::Predicate::ICMP_SLE; goto logic;
        case TCGT:      pred = CmpInst::Predicate::ICMP_SGT; goto logic;
        case TCGE:      pred = CmpInst::Predicate::ICMP_SGE; goto logic;
        case TPLUS:     instr = Instruction::Add; goto math;
        case TMINUS:    instr = Instruction::Sub; goto math;
        case TMUL:      instr = Instruction::Mul; goto math;
        case TDIV:      instr = Instruction::SDiv; goto math;
        case TNUMDIV:   instr = Instruction::SDiv; goto math;
        case TNUMMOD:   return BinaryOperator::CreateSRem(this->lhs.codeGen(context),
                            this->rhs.codeGen(context), "", context.currentBlock());
        case TLOGICAND: instr = Instruction::And; goto math;
        case TLOGICOR:  instr = Instruction::Or; goto math;
    }

    return NULL;
math:
    return BinaryOperator::Create(instr, this->lhs.codeGen(context), 
        this->rhs.codeGen(context), "", context.currentBlock());

logic:
    return new ICmpInst(*context.currentBlock(), pred, this->lhs.codeGen(context),
        this->rhs.codeGen(context), "");
}

Value* NUnaryOp::codeGen(CodeGenContext& context)
{
    switch (this->op) {
        case TMINUS:
            return BinaryOperator::CreateNeg(this->expr.codeGen(context), "", context.currentBlock());
        case TLOGICNOT:
            return BinaryOperator::CreateNot(this->expr.codeGen(context), "", context.currentBlock());
    }

    return NULL;
}

Value* NAssignment::codeGen(CodeGenContext& context)
{
    Value *var_ptr;

    if (context.locals().find(this->lhs.identifier.name) == context.locals().end()) {
        if (context.globals.find(this->lhs.identifier.name) == context.globals.end()) {
            err_and_halt("undeclared variable " + this->lhs.identifier.name);
        } else {
            var_ptr = context.globals[this->lhs.identifier.name];
        }
    } else {
        var_ptr = context.locals()[this->lhs.identifier.name];
    }

    switch (this->lhs.type) {
        case VARIABLE_BASIC: return new StoreInst(rhs.codeGen(context), var_ptr, false, context.currentBlock());
        case VARIABLE_ARRAY:
            
            Value* arr_index_val = this->lhs.arr_size.codeGen(context);

            if (arr_index_val->getType() != context.GetIntegerType())
                err_and_halt("CodeGen<NAssignment><NVariable>: Non-integer array index (" + this->lhs.identifier.name + ")");
            
            std::vector<Value*> index_vect;
            index_vect.push_back(arr_index_val);
            Value *gep = GetElementPtrInst::CreateInBounds(var_ptr, index_vect, "", context.currentBlock());
            
            return new StoreInst(rhs.codeGen(context), gep, false, context.currentBlock());
    }

    err_and_halt("CodeGen<NAssignment><NVariable>: Undefined type attribute: " + std::to_string(this->lhs.type) + "(" + this->lhs.identifier.name + ")");
}

Value* NExpressionStatement::codeGen(CodeGenContext& context)
{
    return this->expression.codeGen(context);
}

Value* NVariableDecl::codeGen(CodeGenContext& context)
{
    AllocaInst *alloc;
    GlobalVariable *gvar;

    if (context.isSymtabEmpty()) {
        
        if (this->type == VARIABLE_BASIC) {
            gvar = new GlobalVariable(*context.module, typeOf(this->type_id, context), false, GlobalValue::InternalLinkage, nullptr, this->id.name);
        } else if (this->type == VARIABLE_ARRAY) {
            Type *arr_type;

            if (this->arr_size == 0)
                arr_type = PointerType::get(typeOf(this->type_id, context), ADDRSPC);
            else
                arr_type = ArrayType::get(typeOf(this->type_id, context), this->arr_size);

            gvar = new GlobalVariable(*context.module, arr_type, false, GlobalValue::InternalLinkage, nullptr, this->id.name);
        } else
            err_and_halt("CodeGen<NVariableDecl>: Undefined type attribute: " + std::to_string(this->type) + "(" + this->type_id.name + ")");

        context.globals[this->id.name] = gvar;
        return gvar;
    }
            

    if (this->type == VARIABLE_BASIC) {
        alloc = new AllocaInst(typeOf(this->type_id, context), ADDRSPC, this->id.name, context.currentBlock());
        
    } else if (this->type == VARIABLE_ARRAY) {
        Type *arr_type;

            if (this->arr_size == 0)
                arr_type = PointerType::get(typeOf(this->type_id, context), ADDRSPC);
            else
                arr_type = ArrayType::get(typeOf(this->type_id, context), this->arr_size);
                
        alloc = new AllocaInst(arr_type, ADDRSPC, this->id.name.c_str(), context.currentBlock());
        
    } else
        err_and_halt("CodeGen<NVariableDecl>: Undefined type attribute: " + std::to_string(this->type) + "(" + this->type_id.name + ")");

    
    context.locals()[this->id.name] = alloc;
    return alloc;
}

Value* NVariableCompoundDecl::codeGen(CodeGenContext& context)
{
    VariableList::const_iterator it;
    for (it = this->decls.begin(); it != this->decls.end(); it++) {
        (**it).codeGen(context);
    }

    return NULL;
}

Value* NFunctionDecl::codeGen(CodeGenContext& context)
{
    std::vector<Type*> argTypes;
    VariableList::const_iterator it;
    for (it = this->arguments.begin(); it != this->arguments.end(); it++) {
        argTypes.push_back(typeOf((**it).type_id, context));
    }
    FunctionType *ftype = FunctionType::get(typeOf(this->type, context), argTypes, false);
    Function *function = Function::Create(ftype, GlobalValue::InternalLinkage, this->id.name.c_str(), context.module);
    BasicBlock *bblock = BasicBlock::Create(GlobCtx, INTRO_CTX, function, 0);

    context.pushBlock(bblock);

    context.curr_func = function;
    context.symtab[function->getName()] = * new std::map<std::string, Value*>;

    for (it = this->arguments.begin(); it != this->arguments.end(); it++) {
        (**it).codeGen(context);
    }
    
    StatementList::const_iterator sit;
    for (sit = this->body.begin(); sit != this->body.end(); sit++) {
        
        (**sit).codeGen(context);
    }

    context.popBlock();
    context.curr_func = NULL;

    
    return function;
}

Value* NIfStatement::codeGen(CodeGenContext& context)
{
    BasicBlock *then_block = BasicBlock::Create(GlobCtx, "then", context.currentBlock()->getParent());
    BasicBlock *else_block = BasicBlock::Create(GlobCtx, "else", context.currentBlock()->getParent());
    BasicBlock *fin_block = BasicBlock::Create(GlobCtx, "iffin", context.currentBlock()->getParent());

    BranchInst *ifbr = BranchInst::Create(then_block, else_block, this->condition.codeGen(context), context.currentBlock());

    context.pushBlock(then_block);

    StatementList::const_iterator t_it;
    for (t_it = this->then_body.begin(); t_it != this->then_body.end(); t_it++) {
        
        (**t_it).codeGen(context);
    }

    BranchInst::Create(fin_block, then_block);

    context.popBlock();

    context.pushBlock(else_block);

    StatementList::const_iterator e_it;
    for (e_it = this->else_body.begin(); e_it != this->else_body.end(); e_it++) {
        
        (**e_it).codeGen(context);
    }

    BranchInst::Create(fin_block, else_block);

    context.popBlock();

    context.popBlock();
    context.pushBlock(fin_block);

    return fin_block;
}

Value* NForStatement::codeGen(CodeGenContext& context)
{
    Value *var_ptr;

    if (context.locals().find(this->iterator.identifier.name) == context.locals().end()) {
        if (context.globals.find(this->iterator.identifier.name) == context.globals.end()) {
            err_and_halt("undeclared variable " + this->iterator.identifier.name);
        } else {
            var_ptr = context.globals[this->iterator.identifier.name];
        }
    } else {
        var_ptr = context.locals()[this->iterator.identifier.name];
    }

    var_ptr = new LoadInst(var_ptr, "", false, context.currentBlock());

    

    Value *iter_by_val = this->iter_by.codeGen(context);

    if (iter_by_val == NULL)
        iter_by_val = ConstantInt::get(context.GetIntegerType(), 0);

    

    if (this->iterator.type == VARIABLE_BASIC) {
        new StoreInst(iter_assign.codeGen(context), var_ptr, false, context.currentBlock());
    } else if (this->iterator.type == VARIABLE_ARRAY) {
        
        Value* arr_index_val = this->iterator.arr_size.codeGen(context);

        if (arr_index_val->getType() != context.GetIntegerType())
            err_and_halt("CodeGen<NFor><NVariable>: Non-integer array index (" + this->iterator.identifier.name + ")");
        
        
        std::vector<Value*> index_vect;
        index_vect.push_back(arr_index_val);
        Value *gep = GetElementPtrInst::CreateInBounds(var_ptr, index_vect, "", context.currentBlock());
        
        new StoreInst(iter_assign.codeGen(context), gep, false, context.currentBlock());
    } else {
        err_and_halt("CodeGen<NFor><NVariable>: Undefined type attribute: " + std::to_string(this->iterator.type) + "(" + this->iterator.identifier.name + ")");
    }

    

    BasicBlock *loop_block = BasicBlock::Create(GlobCtx, "loop", context.currentBlock()->getParent());
    BasicBlock *loop_end = BasicBlock::Create(GlobCtx, "endloop", context.currentBlock()->getParent());
    context.pushBlock(loop_block);

    StatementList::const_iterator it;
    for (it = this->body.begin(); it != this->body.end(); it++) {
        
        (**it).codeGen(context);
    }

    

    if (this->iterator.type == VARIABLE_BASIC) {
        var_ptr->print(outs(), true);
        this->iter_until.codeGen(context)->print(outs(), true);
        Value* updated_val = BinaryOperator::Create(Instruction::Add, var_ptr, iter_by_val, "", context.currentBlock());
        new StoreInst(updated_val, var_ptr, false, context.currentBlock());
        Value *loop_cond = new ICmpInst(*context.currentBlock(), CmpInst::Predicate::ICMP_SLE, var_ptr, this->iter_until.codeGen(context), "");
        BranchInst::Create(loop_block, loop_end, loop_cond, loop_block);
    } else if (this->iterator.type == VARIABLE_ARRAY) {
        
        Value* arr_index_val = this->iterator.arr_size.codeGen(context);

        if (arr_index_val->getType() != context.GetIntegerType())
            err_and_halt("CodeGen<NFor><NVariable>: Non-integer array index (" + this->iterator.identifier.name + ")");
        
        
        std::vector<Value*> index_vect;
        index_vect.push_back(arr_index_val);
        Value *gep = GetElementPtrInst::CreateInBounds(var_ptr, index_vect, "", context.currentBlock());
        

        Value* updated_val = BinaryOperator::Create(Instruction::Add, gep, iter_by_val, "", context.currentBlock());
        new StoreInst(updated_val, gep, false, context.currentBlock());
        Value *loop_cond = new ICmpInst(*context.currentBlock(), CmpInst::Predicate::ICMP_SLE, gep, this->iter_until.codeGen(context), "");
        BranchInst::Create(loop_block, loop_end, loop_cond, loop_block);
    } else {
        err_and_halt("CodeGen<NFor><NVariable>: Undefined type attribute: " + std::to_string(this->iterator.type) + "(" + this->iterator.identifier.name + ")");
    }

    context.popBlock();

    context.popBlock();
    context.pushBlock(loop_end);

    return NULL;
}

Value* NWhileStatement::codeGen(CodeGenContext& context)
{
    Value *while_cond = this->condition.codeGen(context);

    BasicBlock *while_block = BasicBlock::Create(GlobCtx, "whl", context.currentBlock()->getParent());
    BasicBlock *while_end = BasicBlock::Create(GlobCtx, "endwhl", context.currentBlock()->getParent());

    BranchInst::Create(while_block, while_end, while_cond, context.currentBlock());

    context.pushBlock(while_block);

    StatementList::const_iterator it;
    for (it = this->body.begin(); it != this->body.end(); it++) {
        
        (**it).codeGen(context);
    }

    BranchInst::Create(while_block, while_end, while_cond, context.currentBlock());

    context.popBlock();

    context.popBlock();
    context.pushBlock(while_end);

    return NULL;
}

Value* NPrintStatement::codeGen(CodeGenContext& context)
{
    Function *printf = context.module->getFunction("printf");

    ExpressionList::const_iterator it;

    for (it = this->arguments.begin(); it != this->arguments.end(); it++) {
        Value *put_val = (**it).codeGen(context);

        std::string format_string = "%s\n";

        if (put_val->getType()->isIntegerTy())
            format_string = "%d\n";
        else if (put_val->getType()->isDoubleTy())
            format_string = "%lf\n";

        Constant *format_const = ConstantDataArray::getString(GlobCtx, format_string);
        GlobalVariable *var = new GlobalVariable(
            *context.module, ArrayType::get(context.GetIntegerType(), 4),
            true, GlobalValue::PrivateLinkage, format_const, ".str");
        
        Constant *zero = Constant::getNullValue(context.GetIntegerType());

        std::vector<Value*> indices;
        indices.push_back(zero);
        indices.push_back(zero);
        Value *var_ref = GetElementPtrInst::CreateInBounds(var, indices, "", context.currentBlock());

        std::vector<Value*> args;
        args.push_back(var_ref);
        args.push_back(put_val);

        CallInst::Create(printf, args, "", context.currentBlock());
    }

    return NULL;
}

Value* NReadStatement::codeGen(CodeGenContext& context)
{
    return NULL;
}

Value* NReturnStatement::codeGen(CodeGenContext& context)
{
    return ReturnInst::Create(GlobCtx, this->expression.codeGen(context), context.currentBlock());
}

Value* NProgram::codeGen(CodeGenContext& context)
{
    Function *printf = printf_prototype(GlobCtx, context.module);

    StatementList::const_iterator vit;
    for (vit = this->variable_decl_stmts.begin(); vit != this->variable_decl_stmts.end(); vit++) {
        
        (**vit).codeGen(context);
    }

    StatementList::const_iterator fit;
    for (fit = this->function_decl_stmts.begin(); fit != this->function_decl_stmts.end(); fit++) {
        
        (**fit).codeGen(context);
    }

    FunctionType *ftype = FunctionType::get(context.GetIntegerType(), false);
    Function *start_func = Function::Create(ftype, GlobalValue::InternalLinkage, "_start", context.module);
    BasicBlock *entry_block = BasicBlock::Create(GlobCtx, "entry", start_func, 0);
    
    context.pushBlock(entry_block);
    
    if (context.module->getFunction("main") == NULL)
        err_and_halt("CodeGen<NProgram> There is no entry function 'main' exists!");

    CallInst::Create(context.module->getFunction("main"), "", context.currentBlock());
    ReturnInst::Create(GlobCtx, ConstantInt::get(context.GetIntegerType(), 0), context.currentBlock());
    
    context.popBlock();

    
    return NULL;
}