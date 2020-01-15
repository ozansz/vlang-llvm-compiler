#ifndef __NODE_H
#define __NODE_H

#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>

#define VARIABLE_BASIC  0
#define VARIABLE_ARRAY  1

#define NODE_Dump_Typecast(x, o) ((x)o)->DumpNode()

typedef long long int IntegerType;
typedef double RealType;

class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDecl;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDecl*> VariableList;

class Node {
public:
    virtual ~Node() {}

    virtual llvm::Value* codeGen(CodeGenContext& context) { return NULL; }
    void DumpNode();
    bool IsBaseStatement();
};

class NExpression : public Node {
public:
    virtual llvm::Value* codeGen(CodeGenContext& context);
    void DumpNode();
};

class NStatement : public Node {
public:
    virtual llvm::Value* codeGen(CodeGenContext& context);
    void DumpNode();
};

class NInteger : public NExpression {
public:
    IntegerType value;
    NInteger(IntegerType value) : value(value) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);

    
    void DumpNode();
};

class NReal : public NExpression {
public:
    RealType value;
    NReal(RealType value) : value(value) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);

    
    void DumpNode();
};

class NStringLiteral : public NExpression {
public:
    std::string value;
    NStringLiteral(std::string value) : value(value) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);

    
    void DumpNode();
};

class NIdentifier : public NExpression {
public:
    std::string name;
    NIdentifier(std::string& name) : name(name) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);

    
    void DumpNode();
};

class NVariable : public NExpression {
public:
    NIdentifier& identifier;
    int type;
    NExpression& arr_size;
    NVariable(NIdentifier& identifier, int type, NExpression& arr_size) :
        identifier(identifier), type(type), arr_size(arr_size) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);

    
    void DumpNode();
};

class NFunctionCall : public NExpression {
public:
    NIdentifier& id;
    ExpressionList& arguments;
    NFunctionCall(NIdentifier& id, ExpressionList& arguments) :
        id(id), arguments(arguments) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);

    
    void DumpNode();
};

class NBinaryOp : public NExpression {
public:
    int op;
    NExpression& lhs;
    NExpression& rhs;
    NBinaryOp(NExpression& lhs, int op, NExpression& rhs) :
        lhs(lhs), rhs(rhs), op(op) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);

    
    void DumpNode();
};

class NUnaryOp : public NExpression {
public:
    int op;
    NExpression& expr;
    NUnaryOp(int op, NExpression& expr) :
        op(op), expr(expr) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);

    
    void DumpNode();
};

class NAssignment : public NStatement {
public:
    NVariable& lhs;
    NExpression& rhs;
    NAssignment(NVariable& lhs, NExpression& rhs) : 
        lhs(lhs), rhs(rhs) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);

    
    void DumpNode();
};

class NExpressionStatement : public NStatement {
public:
    NExpression& expression;
    NExpressionStatement(NExpression& expression) : 
        expression(expression) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);

    
    void DumpNode();
};

class NVariableDecl : public NStatement {
public:
    NIdentifier& type_id;
    NIdentifier& id;
    int type;
    int arr_size;
    NVariableDecl(NIdentifier& id, NIdentifier& type_id, int type, int arr_size) :
        type_id(type_id), id(id), type(type), arr_size(arr_size) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);

    
    void DumpNode();
};

class NVariableCompoundDecl : public NStatement {
public:
    VariableList& decls;
    NVariableCompoundDecl(VariableList& decls) : decls(decls) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);

    
    void DumpNode();
};

class NFunctionDecl : public NStatement {
public:
    NIdentifier& type;
    NIdentifier& id;
    VariableList& arguments;
    StatementList& body;
    NFunctionDecl(NIdentifier& type, NIdentifier& id, VariableList& arguments, StatementList& body) :
        type(type), id(id), arguments(arguments), body(body) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    
    
    void DumpNode();
};

class NIfStatement : public NStatement {
public:
    NExpression& condition;
    StatementList& then_body;
    StatementList& else_body;
    NIfStatement(NExpression& condition, StatementList& then_body, StatementList& else_body) :
        condition(condition), then_body(then_body), else_body(else_body) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);

    
    void DumpNode();
};

class NForStatement : public NStatement {
public:
    NVariable& iterator;
    NExpression& iter_assign;
    NExpression& iter_until;
    NExpression& iter_by;
    StatementList& body;
    NForStatement(NVariable& iterator, NExpression& iter_assign, NExpression& iter_until, NExpression& iter_by, StatementList& body) :
        iterator(iterator), iter_assign(iter_assign), iter_until(iter_until), iter_by(iter_by), body(body) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);

    
    void DumpNode();
};

class NWhileStatement : public NStatement {
public:
    NExpression& condition;
    StatementList& body;
    NWhileStatement(NExpression& condition, StatementList& body) : condition(condition), body(body) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);

    
    void DumpNode();
};

class NPrintStatement : public NStatement {
public:
    ExpressionList& arguments;
    NPrintStatement(ExpressionList& arguments) : arguments(arguments) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);

    
    void DumpNode();
};

class NReadStatement : public NStatement {
public:
    ExpressionList& destinations;
    NReadStatement(ExpressionList& destinations) : destinations(destinations) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);

    
    void DumpNode();
};

class NReturnStatement : public NStatement {
public:
    NExpression& expression;
    NReturnStatement(NExpression& expression) : expression(expression) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);

    
    void DumpNode();
};

class NProgram : public Node {
public:
    StatementList& variable_decl_stmts;
    StatementList& function_decl_stmts;
    NProgram(StatementList& variable_decl_stmts, StatementList& function_decl_stmts) :
        variable_decl_stmts(variable_decl_stmts), function_decl_stmts(function_decl_stmts) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);

    
    void DumpNode();
};

#endif