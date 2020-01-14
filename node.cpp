#include <iostream>
#include "node.hpp"

void NExpression::DumpNode() {
    if (dynamic_cast<NInteger*>(this) != nullptr)
        return dynamic_cast<NInteger*>(this)->DumpNode();
    else if (dynamic_cast<NReal*>(this) != nullptr)
        return dynamic_cast<NReal*>(this)->DumpNode();
    else if (dynamic_cast<NStringLiteral*>(this) != nullptr)
        return dynamic_cast<NStringLiteral*>(this)->DumpNode();
    else if (dynamic_cast<NIdentifier*>(this) != nullptr)
        return dynamic_cast<NIdentifier*>(this)->DumpNode();
    else if (dynamic_cast<NVariable*>(this) != nullptr)
        return dynamic_cast<NVariable*>(this)->DumpNode();
    else if (dynamic_cast<NFunctionCall*>(this) != nullptr)
        return dynamic_cast<NFunctionCall*>(this)->DumpNode();
    else if (dynamic_cast<NBinaryOp*>(this) != nullptr)
        return dynamic_cast<NBinaryOp*>(this)->DumpNode();
    else if (dynamic_cast<NUnaryOp*>(this) != nullptr)
        return dynamic_cast<NUnaryOp*>(this)->DumpNode();
    else
        std::cout << "NExpression()";
}

void NStatement::DumpNode() {
    if (dynamic_cast<NAssignment*>(this) != nullptr)
        return dynamic_cast<NAssignment*>(this)->DumpNode();
    else if (dynamic_cast<NExpressionStatement*>(this) != nullptr)
        return dynamic_cast<NExpressionStatement*>(this)->DumpNode();
    else if (dynamic_cast<NVariableDecl*>(this) != nullptr)
        return dynamic_cast<NVariableDecl*>(this)->DumpNode();
    else if (dynamic_cast<NVariableCompoundDecl*>(this) != nullptr)
        return dynamic_cast<NVariableCompoundDecl*>(this)->DumpNode();
    else if (dynamic_cast<NFunctionDecl*>(this) != nullptr)
        return dynamic_cast<NFunctionDecl*>(this)->DumpNode();
    else if (dynamic_cast<NIfStatement*>(this) != nullptr)
        return dynamic_cast<NIfStatement*>(this)->DumpNode();
    else if (dynamic_cast<NForStatement*>(this) != nullptr)
        return dynamic_cast<NForStatement*>(this)->DumpNode();
    else if (dynamic_cast<NWhileStatement*>(this) != nullptr)
        return dynamic_cast<NWhileStatement*>(this)->DumpNode();
    else if (dynamic_cast<NPrintStatement*>(this) != nullptr)
        return dynamic_cast<NPrintStatement*>(this)->DumpNode();
    else if (dynamic_cast<NReadStatement*>(this) != nullptr)
        return dynamic_cast<NReadStatement*>(this)->DumpNode();
    else if (dynamic_cast<NReturnStatement*>(this) != nullptr)
        return dynamic_cast<NReturnStatement*>(this)->DumpNode();
    else
        std::cout << "NStatement()";
}

void NInteger::DumpNode() {
    std::cout << "NInteger(" << this->value << ")";
}

void NReal::DumpNode() {
    std::cout << "NReal(" << this->value << ")";
}

void NStringLiteral::DumpNode() {
    std::cout << "NStringLiteral(\"" << this->value << "\")";
}

void NIdentifier::DumpNode() {
    std::cout << "NIdentifier(" << this->name << ")";
}

void NVariable::DumpNode() {
    std::cout << "NVariable(" << this->identifier.name;
    
    if (this->type == VARIABLE_ARRAY) {
        std::cout << "[";
        this->arr_size.DumpNode();
        std::cout << "]";
    }

    std::cout << ")";
}

void NFunctionCall::DumpNode() {
    std::cout << "NFunctionCall(" << this->id.name; 
    
    for (int i = 0; i < this->arguments.size(); i++) {
        std::cout << ", ";
        this->arguments[i]->DumpNode();
    }
        
    std::cout << ")";
}

void NBinaryOp::DumpNode() {
    std::cout << "NBinaryOp(" << this->op << ", ";
    this->lhs.DumpNode();
    std::cout << ", ";
    this->rhs.DumpNode();
    std::cout << ")";
}

void NUnaryOp::DumpNode() {
    std::cout << "NUnaryOp(" << this->op << ", ";
    this->expr.DumpNode();
    std::cout << ")";
}

void NAssignment::DumpNode() {
    std::cout << "NAssignment(";
    this->lhs.DumpNode();
    std::cout << ", ";
    this->rhs.DumpNode();
    std::cout << ")";
}

void NExpressionStatement::DumpNode() {
    std::cout << "NExpressionStatement(";
    this->expression.DumpNode();
    std::cout << ")";
}

void NVariableDecl::DumpNode() {
    std::cout << "NVariableDecl(" << this->id.name << ":" << this->type_id.name;
    
    if (this->type == VARIABLE_ARRAY)
        std::cout << "[" << this->arr_size << "]";
    
    std::cout << ")";
}

void NVariableCompoundDecl::DumpNode() {
    bool first_iter = true;

    std::cout << "NVariableCompoundDecl(";
    
    for (int i = 0; i < this->decls.size(); i++) {
        std::cout << (first_iter ? "" : ", ");
        this->decls[i]->DumpNode();
        first_iter = false;
    }
        
    std::cout << ")";
}

void NFunctionDecl::DumpNode() {
    bool first_iter = true;

    std::cout << "NFunctionDecl(" << this->type.name << " " << this->id.name << "(";
    
    for (int i = 0; i < this->arguments.size(); i++) {
        std::cout << (first_iter ? "" : ", ");
        this->arguments[i]->DumpNode();
        first_iter = false;
    }

    std::cout << "), (";
    
    first_iter = true;

    for (int i = 0; i < this->body.size(); i++) {
        std::cout << (first_iter ? "" : ", ");
        this->body[i]->DumpNode();
        first_iter = false;
    }

    std::cout <<"))";
}

void NIfStatement::DumpNode() {
    bool first_iter = true;

    std::cout << "NIfStatement(";
    this->condition.DumpNode();
    std::cout << ", (";
    
    for (int i = 0; i < this->then_body.size(); i++) {
        std::cout << (first_iter ? "" : ", ");
        this->then_body[i]->DumpNode();
        first_iter = false;
    }

    std::cout << "), (";

    first_iter = true;
    
    for (int i = 0; i < this->else_body.size(); i++) {
        std::cout << (first_iter ? "" : ", ");
        this->else_body[i]->DumpNode();
        first_iter = false;
    }

    std::cout <<"))";
}

void NForStatement::DumpNode() {
    bool first_iter = true;

    std::cout << "NForStatement(";
    this->iterator.DumpNode();
    std::cout << ", ";
    this->iter_assign.DumpNode();
    std::cout << ", ";
    this->iter_until.DumpNode();
    std::cout << ", ";
    this->iter_by.DumpNode();
    std::cout << ", (";
    
    for (int i = 0; i < this->body.size(); i++) {
        std::cout << (first_iter ? "" : ", ");
        this->body[i]->DumpNode();
        first_iter = false;
    }

    std::cout << "))";
}

void NWhileStatement::DumpNode() {
    bool first_iter = true;

    std::cout << "NWhileStatement(";
    this->condition.DumpNode();
    std::cout << ", (";
    
    for (int i = 0; i < this->body.size(); i++) {
        std::cout << (first_iter ? "" : ", ");
        this->body[i]->DumpNode();
        first_iter = false;
    }

    std::cout <<"))";
}

void NPrintStatement::DumpNode() {
    bool first_iter = true;

    std::cout << "NPrintStatement(";
    
    for (int i = 0; i < this->arguments.size(); i++) {
        std::cout << (first_iter ? "" : ", ");
        this->arguments[i]->DumpNode();
        first_iter = false;
    }

    std::cout << ")";
}

void NReadStatement::DumpNode() {
    bool first_iter = true;

    std::cout << "NReadStatement(";
    
    for (int i = 0; i < this->destinations.size(); i++) {
        std::cout << (first_iter ? "" : ", ");
        this->destinations[i]->DumpNode();
        first_iter = false;
    }

    std::cout << ")";
}

void NReturnStatement::DumpNode() {
    std::cout << "NReturnStatement(";
    this->expression.DumpNode();
    std::cout << ")";
}

void NProgram::DumpNode() {
    bool first_iter = true;

    std::cout << "NProgram(";

    for (int i = 0; i < this->variable_decl_stmts.size(); i++) {
        std::cout << (first_iter ? "" : ", ");
        this->variable_decl_stmts[i]->DumpNode();
        first_iter = false;
    }

    first_iter = true;

    if (this->variable_decl_stmts.size() > 0)
        std::cout << ", ";

    for (int i = 0; i < this->function_decl_stmts.size(); i++) {
        std::cout << (first_iter ? "" : ", ");
        this->function_decl_stmts[i]->DumpNode();
        first_iter = false;
    }

    std::cout << ")";
}