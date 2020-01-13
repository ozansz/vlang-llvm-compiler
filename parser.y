%{
    #include "node.hpp"

    /* the top level root node of our final AST */
    NProgram programBlock;

    extern int yylex();
    void yyerror(const char *s) { printf("ERROR: %s\n", s); }
%}

/* Represents the many different ways we can access our data */
%union {
    NExpression *expr;
    NStatement *stmt;
    NIdentifier *ident;
    NVariable *variable;
    NVariableDecl *var_decl;
    std::vector<NVariableDeclaration*> *varvec;
    std::vector<NExpression*> *exprvec;
    std::vector<NStatement> *stmtvec;
    int token;
}

/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
 */

%token <string> TIDENTIFIER TINTEGER TDOUBLE TSTRINGLIT
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TDOT TSEMICOLON TASSIGN
%token <token> TPLUS TMINUS TMUL TDIV TNUMMOD TNUMDIV
%token <token> TINT TREAL TVAR TFUNC TENDFUNC TRETURN
%token <token> TLOGICAND TLOGICNOT TLOGICOR TIF TTHEN TELSE TENDIF
%token <token> TDO TFOR TENDFOR TTO TBY TWHILE TENDWHILE TPRINT TREAD

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */

%type <token> unaryop binaryop
%type <ident> identifier
%type <exprvec> function_call_arg_list read_arg_list print_arg_list
%type <expr> function_call_expression binaryop_expression unaryop_expression expression string_literal_expression real_expression integer_expression
%type <stmt> variable_decl_statement read_statement print_statement while_statement for_statement if_statement function_decl return_statement assignment_statement statement
%type <variable> variable
%type <var_decl> variable_decl
%type <varvec> variable_decl_list
%type <stmtvec> stmt_list function_decl_list global_var_decl_list

/* Operator precedence for mathematical operators */
%left TPLUS TMINUS
%left TMUL TDIV

%start program

%%

program
        : global_var_decl_list function_decl_list {programBlock->variable_decl_stmts = $1; programBlock->function_decl_stmts = $2;}
        ;

global_var_decl_list
        : variable_decl_statement                       {$$ = new StatementList(); $$->push_back($<stmt>1);}
        | global_var_decl_list variable_decl_statement  {$1->push_back($<stmt>2);}
        ;

function_decl_list
        : function_decl                         {$$ = new StatementList(); $$->push_back($<stmt>1);}
        | function_decl_list function_decl      {$1->push_back($<stmt>2);}
        ;

stmt_list
        : statement             {$$ = new StatementList(); $$->push_back($<stmt>1);}
        | stmt_list statement   {$1->push_back($<stmt>2);}
        ;

statement
        : assignment_statement TSEMICOLON
        | return_statement TSEMICOLON
        | expression TSEMICOLON
        | variable_decl_statement TSEMICOLON
        | function_decl TSEMICOLON
        | if_statement TSEMICOLON
        | for_statement TSEMICOLON
        | while_statement TSEMICOLON
        | print_statement TSEMICOLON
        | read_statement TSEMICOLON
        ;

assignment_statement
        : variable TASSIGN expression   {$$ = new NAssignment(*$1, $2, *$3);}
        ;
    
return_statement
        : TRETURN expression    {$$ = new NReturnStatement(*$2);}
        ;

variable_decl_statement
        : TVAR variable_decl_list TSEMICOLON {$$ = $2;}
        ;

variable_decl_list
        : variable_decl                             {$$ = new NVariableCompoundDecl(); $$->decls.push_back($<var_decl>1);}
        | variable_decl_list TCOMMA variable_decl   {$1->decls.push_back($<var_decl>3);}
        ;

variable_decl
        : identifier TCOLON identifier                                      {$$ = NVariableDecl(*$1, *$3, VARIABLE_BASIC, 0);}
        | identifier TCOLON identifier TLBRACE TRBRACE                      {$$ = NVariableDecl(*$1, *$3, VARIABLE_ARRAY, 0);}
        | identifier TCOLON identifier TLBRACE integer_expression TRBRACE   {$$ = NVariableDecl(*$1, *$3, VARIABLE_ARRAY, $5->value);}
        ;

variable
        : identifier                                        {$$ = new NVariable(*$1, VARIABLE_BASIC, 0);}
        | identifier TLBRACE integer_expression TRBRACE     {$$ = new NVariable(*$1, VARIABLE_ARRAY, $3->value);}
        ;

function_decl
        : identifier identifier TLPAREN TRPAREN stmt_list TENDFUNC                      {$$ = new NFunctionDecl(*$1, *$2, new StatementList(), *$5);}
        | identifier identifier TLPAREN variable_decl_list TRPAREN stmt_list TENDFUNC   {$$ = new NFunctionDecl(*$1, *$2, *$4, *$6);}
        ;

if_statement
        : TIF expression TTHEN stmt_list TENDIF                 {$$ = new NIfStatement(*$2, *$4);}
        | TIF expression TTHEN stmt_list TELSE stmt_list TENDIF {$$ = new NIfStatement(*$2, *$4, *$6);}
        ;

for_statement
        : TFOR variable TASSIGN expression TTO expression stmt_list TENDFOR {$$ = new NForStatement(*$2, *$4, *$6, *$7);}
        | TFOR variable TASSIGN expression TTO expression TBY expression stmt_list TENDFOR {$$ = new NForStatement(*$2, *$4, *$6, *$8, *$9);}
        ;

while_statement
        : TWHILE expression TDO stmt_list TENDWHILE {$$ = new NWhileStatement(*$2, *$4);}

print_statement
        : TPRINT print_arg_list             {$$ = new NPrintStatement(*$2);}
        ;

print_arg_list
        : expression                        {$$ = new ExpressionList(); $$->push_back($1);}
        | print_arg_list TCOMMA expression  {$1->push_back($3);}
        ;

read_statement                              
        : TREAD read_arg_list               {$$ = new NReadStatement(*$2);}
        ;

read_arg_list
        : variable                          {$$ = new ExpressionList(); $$->push_back($1);}
        | read_arg_list TCOMMA variable     {$1->push_back($3);}
        ;

expression
        : integer_expression            
        | real_expression               
        | string_literal_expression           
        | variable                      
        | function_call_expression            
        | binaryop_expression             
        | unaryop_expression              
        | TLPAREN expression TRPAREN    {$$ = $2;}          
        ;

integer_expression
        : TINTEGER {$$ = new NInteger(atol($1->c_str()));}
        ;

real_expression
        : TDOUBLE {$$ = new NReal(atof($1->c_str()));}
        ;

string_literal_expression
        : TSTRINGLIT {$$ = new NStringLiteral($1);}
        ;

identifier
        : TIDENTIFIER {$$ = new NIdentifier(*$1);}
        ;

function_call_expression
        : identifier TLPAREN function_call_arg_list TRPAREN {$$ = new NFunctionCall(*$1, *$3);}
        ;

function_call_arg_list
        : /* blank */ {$$ = new ExpressionList();}
        | expression  {$$ = new ExpressionList(); $$->push_back($1);}
        | function_call_arg_list TCOMMA expression {$1->push_back($3);}
        ;

binaryop_expression
        : expression binaryop expression {$$ = new NBinaryOp(*$1, $2, *$3);}
        ;

binaryop
        : TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE
        | TPLUS | TMINUS | TMUL | TDIV
        | TNUMDIV | TNUMMOD
        | TLOGICAND | TLOGICOR
        ; 

unaryop_expression
        : unaryop expression {$$ = new NUnaryOp($1, *$2);}
        ;

unaryop
        : TMINUS | TLOGICNOT
        ;

%%