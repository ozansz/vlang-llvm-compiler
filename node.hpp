#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>

#define VARIABLE_BASIC  0
#define VARIABLE_ARRAY  1

#define NODE_INTEGER    0
#define NODE_REAL       1
#define NODE_STRLIT     2 
#define NODE_ID         3
#define NODE_VAR        4
#define NODE_FNCALL     5
#define NODE_BINOP      6
#define NODE_UNOP       7
#define NODE_ASSGN      8
#define NODE_EXPRSTMT   9
#define NODE_VARDECL    10
#define NODE_VARCOMPDECL    11
#define NODE_FNDECL     12
#define NODE_IFSTMT     13
#define NODE_FORSTMT    14
#define NODE_WHLSTMT    15
#define NODE_PRNSTMT    16
#define NODE_RDSTMT     17
#define NODE_RETSTMT    18

#define NODE_PROG       96
#define NODE_STMT       97
#define NODE_EXPR       98
#define NODE_NODE       99

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

    //virtual llvm::Value* codeGen(CodeGenContext& context);

    Node() : _ndtype(NODE_NODE) { }

    int _ndtype;
    void DumpNode();
};

class NExpression : public Node {
public:
    NExpression() : _ndtype(NODE_EXPR) { }

    int _ndtype;
    void DumpNode();
};

class NStatement : public Node {
public:
    NStatement() : _ndtype(NODE_STMT) { }

    int _ndtype;
    void DumpNode();
};

class NInteger : public NExpression {
public:
    IntegerType value;
    NInteger(IntegerType value) : value(value), _ndtype(NODE_INTEGER) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);

    int _ndtype;
    void DumpNode();
};

class NReal : public NExpression {
public:
    RealType value;
    NReal(RealType value) : value(value), _ndtype(NODE_REAL) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);

    int _ndtype;
    void DumpNode();
};

class NStringLiteral : public NExpression {
public:
    std::string value;
    NStringLiteral(std::string value) : value(value), _ndtype(NODE_STRLIT) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);

    int _ndtype;
    void DumpNode();
};

class NIdentifier : public NExpression {
public:
    std::string name;
    NIdentifier(std::string& name) : name(name), _ndtype(NODE_ID) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);

    int _ndtype;
    void DumpNode();
};

class NVariable : public NExpression {
public:
    NIdentifier& identifier;
    int type;
    int arr_size;
    NVariable(NIdentifier& identifier, int type, int arr_size) :
        identifier(identifier), type(type), arr_size(arr_size), _ndtype(NODE_VAR) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);

    int _ndtype;
    void DumpNode();
};

class NFunctionCall : public NExpression {
public:
    NIdentifier& id;
    ExpressionList& arguments;
    NFunctionCall(NIdentifier& id, ExpressionList& arguments) :
        id(id), arguments(arguments), _ndtype(NODE_FNCALL) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);

    int _ndtype;
    void DumpNode();
};

class NBinaryOp : public NExpression {
public:
    int op;
    NExpression& lhs;
    NExpression& rhs;
    NBinaryOp(NExpression& lhs, int op, NExpression& rhs) :
        lhs(lhs), rhs(rhs), op(op), _ndtype(NODE_BINOP) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);

    int _ndtype;
    void DumpNode();
};

class NUnaryOp : public NExpression {
public:
    int op;
    NExpression& expr;
    NUnaryOp(int op, NExpression& expr) :
        op(op), expr(expr), _ndtype(NODE_UNOP) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);

    int _ndtype;
    void DumpNode();
};

class NAssignment : public NStatement {
public:
    NVariable& lhs;
    NExpression& rhs;
    NAssignment(NVariable& lhs, NExpression& rhs) : 
        lhs(lhs), rhs(rhs), _ndtype(NODE_ASSGN) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);

    int _ndtype;
    void DumpNode();
};

class NExpressionStatement : public NStatement {
public:
    NExpression& expression;
    NExpressionStatement(NExpression& expression) : 
        expression(expression), _ndtype(NODE_EXPRSTMT) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);

    int _ndtype;
    void DumpNode();
};

class NVariableDecl : public NStatement {
public:
    NIdentifier& type_id;
    NIdentifier& id;
    int type;
    int arr_size;
    NVariableDecl(NIdentifier& type_id, NIdentifier& id, int type, int arr_size) :
        type_id(type_id), id(id), type(type), arr_size(arr_size), _ndtype(NODE_VARDECL) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);

    int _ndtype;
    void DumpNode();
};

class NVariableCompoundDecl : public NStatement {
public:
    VariableList& decls;
    NVariableCompoundDecl(VariableList& decls) : decls(decls), _ndtype(NODE_VARCOMPDECL) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);

    int _ndtype;
    void DumpNode();
};

class NFunctionDecl : public NStatement {
public:
    NIdentifier& type;
    NIdentifier& id;
    VariableList& arguments;
    StatementList& body;
    NFunctionDecl(NIdentifier& type, NIdentifier& id, VariableList& arguments, StatementList& body) :
        type(type), id(id), arguments(arguments), body(body), _ndtype(NODE_FNDECL) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);
    
    int _ndtype;
    void DumpNode();
};

class NIfStatement : public NStatement {
public:
    NExpression& condition;
    StatementList& then_body;
    StatementList& else_body;
    NIfStatement(NExpression& condition, StatementList& then_body, StatementList& else_body) :
        condition(condition), then_body(then_body), else_body(else_body), _ndtype(NODE_IFSTMT) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);

    int _ndtype;
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
        iterator(iterator), iter_assign(iter_assign), iter_until(iter_until), iter_by(iter_by), body(body), _ndtype(NODE_FORSTMT) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);

    int _ndtype;
    void DumpNode();
};

class NWhileStatement : public NStatement {
public:
    NExpression& condition;
    StatementList& body;
    NWhileStatement(NExpression& condition, StatementList& body) : condition(condition), body(body), _ndtype(NODE_WHLSTMT) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);

    int _ndtype;
    void DumpNode();
};

class NPrintStatement : public NStatement {
public:
    ExpressionList& arguments;
    NPrintStatement(ExpressionList& arguments) : arguments(arguments), _ndtype(NODE_PRNSTMT) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);

    int _ndtype;
    void DumpNode();
};

class NReadStatement : public NStatement {
public:
    ExpressionList& destinations;
    NReadStatement(ExpressionList& destinations) : destinations(destinations), _ndtype(NODE_RDSTMT) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);

    int _ndtype;
    void DumpNode();
};

class NReturnStatement : public NStatement {
public:
    NExpression& expression;
    NReturnStatement(NExpression& expression) : expression(expression), _ndtype(NODE_RETSTMT) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);

    int _ndtype;
    void DumpNode();
};

class NProgram : public Node {
public:
    StatementList& variable_decl_stmts;
    StatementList& function_decl_stmts;
    NProgram(StatementList& variable_decl_stmts, StatementList& function_decl_stmts) :
        variable_decl_stmts(variable_decl_stmts), function_decl_stmts(function_decl_stmts), _ndtype(NODE_PROG) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);

    int _ndtype;
    void DumpNode();
};