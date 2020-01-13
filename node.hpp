#include <iostream>
#include <vector>
#include <llvm/Value.h>

#define VARIABLE_BASIC  0
#define VARIABLE_ARRAY  1

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
    virtual llvm::Value* codeGen(CodeGenContext& context) { }
};

class NExpression : public Node {
};

class NStatement : public Node {
};

class NInteger : public NExpression {
public:
    IntegerType value;
    NInteger(IntegerType value) : value(value) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NReal : public NExpression {
public:
    RealType value;
    NReal(RealType value) : value(value) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NStringLiteral : public NExpression {
public:
    std::string value;
    NStringLiteral(std::string value) : value(value) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NIdentifier : public NExpression {
public:
    std::string name;
    NIdentifier(const std::string& name) : name(name) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NVariable : public NExpression {
public:
    std::string name;
    int type;
    int arr_size;
    NVariable(const std::string& name, int type, int arr_size) :
        name(name), type(type), arr_size(arr_size) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NFunctionCall : public NExpression {
public:
    const NIdentifier& id;
    ExpressionList arguments;
    NFunctionCall(const NIdentifier& id, ExpressionList& arguments) :
        id(id), arguments(arguments) { }
    NFunctionCall(const NIdentifier& id) : id(id) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NBinaryOp : public NExpression {
public:
    int op;
    NExpression& lhs;
    NExpression& rhs;
    NBinaryOp(NExpression& lhs, int op, NExpression& rhs) :
        lhs(lhs), rhs(rhs), op(op) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NUnaryOp : public NExpression {
public:
    int op;
    NExpression& expr;
    NBinaryOp(int op, NExpression& expr) :
        op(op), expr(expr) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NBlock : public NExpression {
public:
    StatementList statements;
    NBlock() { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NAssignment : public NStatement {
public:
    NIdentifier& lhs;
    NExpression& rhs;
    NAssignment(NIdentifier& lhs, NExpression& rhs) : 
        lhs(lhs), rhs(rhs) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NExpressionStatement : public NStatement {
public:
    NExpression& expression;
    NExpressionStatement(NExpression& expression) : 
        expression(expression) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NVariableDecl : public NStatement {
public:
    const NIdentifier& type;
    NIdentifier& id;
    int type;
    int arr_size;
    NVariableDecl(const NIdentifier& type, NIdentifier& id, int type, int arr_size) :
        type(type), id(id), type(type), arr_size(arr_size) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NVariableCompoundDecl : public NStatement {
public:
    VariableList decls;
    NVariableCompoundDecl(VariableList decls) : decls(decls) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NFunctionDecl : public NStatement {
public:
    const NIdentifier& type;
    const NIdentifier& id;
    VariableList arguments;
    StatementList body;
    NFunctionDecl(const NIdentifier& type, const NIdentifier& id, 
            const VariableList& arguments, StatementList body) :
        type(type), id(id), arguments(arguments), body(body) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NIfStatement : public NStatement {
public:
    NExpression& condition;
    StatementList then_body;
    StatementList else_body;
    NIfStatement(const NExpression& condition, StatementList then) :
        condition(condition), then(then) { }
    NIfStatement(const NExpression& condition, StatementList then, StatementList else) :
        condition(condition), then(then), else(else) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NForStatement : public NStatement {
public:
    const NVariable& iterator;
    NExpression& iter_assign;
    NExpression& iter_until;
    NExpression& iter_by;
    StatementList body;
    NForStatement(const NVariable& iterator, NExpression& iter_assign, NExpression& iter_until, StatementList body) :
        iterator(iterator), iter_assign(iter_assign), iter_until(iter_until), body(body) { }
    NForStatement(const NVariable& iterator, NExpression& iter_assign, NExpression& iter_until, NExpression& iter_by, StatementList body) :
        iterator(iterator), iter_assign(iter_assign), iter_until(iter_until), iter_by(iter_by), body(body) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NWhileStatement : public NStatement {
public:
    NExpression& condition;
    StatementList body;
    NWhileStatement(NExpression& condition, StatementList body) : condition(condition), body(body) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NPrintStatement : public NStatement {
public:
    ExpressionList arguments;
    NPrintStatement(ExpressionList arguments) : arguments(arguments) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NReadStatement : public NStatement {
public:
    ExpressionList destinations;
    NReadStatement(ExpressionList destinations) : destinations(destinations) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NReturnStatement : public NStatement {
public:
    NExpression& expression;
    NReturnStatement(NExpression& expression) : expression(expression) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NProgram : public Node {
public:
    StatementList variable_decl_stmts;
    StatementList function_decl_stmts;
    NProgram(StatementList variable_decl_stmts, StatementList function_decl_stmts) :
        variable_decl_stmts(variable_decl_stmts), function_decl_stmts(function_decl_stmts) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};