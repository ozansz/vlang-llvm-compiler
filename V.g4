grammar V;

program: decl_list func_list;

func_list: func_decl+;
func_decl: basic_type FUNC ID func_param_list func_body ENDFUNC;
func_param_list: LPAREN variable_param_list* RPAREN;
func_body: decl_list stmt_list;

decl_list: (decl TERMINATOR)*;
decl: VAR variable_decl_list;

variable_decl_list: variable_decl (SEPERATOR variable_decl)*;
variable_param_list: variable_param (SEPERATOR variable_param)*;
variable_param: ID ':' variable_type;
variable_decl: ID ':' variable_type_decl;
variable_type_decl: basic_type vector_extension_decl?;
variable_type: basic_type vector_extension?;

// Note: ()+ means this is a multidimensional vector extension.
vector_extension: (LBRACE vector_dimension? RBRACE)+;
vector_extension_decl: (LBRACE vector_dimension RBRACE)+;
vector_extension_var: (LBRACE expression RBRACE)+;
vector_dimension: NATURAL_NUBER_LIT;

stmt_list: (stmt TERMINATOR)+;
stmt
    :assignment_stmt
    |return_stmt
    |print_stmt
    |read_stmt
    |for_stmt
    |if_stmt
    |while_stmt
    |function_call
    |inline_assembly_stmt
    ;

assignment_stmt: variable ASSIGNOP expression;

variable
    :ID
    |ID vector_extension_var
    ;

logical_expression
    :expression
    |expression ('='|'<>'|'<'|'<='|'>'|'>=') expression
    |logical_expression (AND|OR|NOT) logical_expression
    |(AND|OR|NOT) logical_expression
    ;

expression
    :term
    |expression ('+'|'-') term
    ;
term
    :factor
    |term ('*'|'/'|MOD|DIV) factor
    ;
factor
    :variable
    |function_call
    |NUMBER_LIT
    |NATURAL_NUBER_LIT
    |LPAREN expression RPAREN
    |'-' expression
    ;
expression_list: expression (SEPERATOR expression)*;

print_stmt: PRINT print_argument (SEPERATOR print_argument)*;
print_argument
    :expression
    |STRING_LIT
    ;

read_stmt: READ read_argument (SEPERATOR read_argument)*;
read_argument: variable; //TODO: Check this
return_stmt: RETURN expression;

for_stmt: FOR variable ASSIGNOP expression TO expression (BY expression)? stmt_list ENDFOR;

if_stmt: IF logical_expression THEN stmt_list (ELSE stmt_list)? ENDIF;

while_stmt: WHILE logical_expression DO stmt_list ENDWHILE;

function_call: ID LPAREN expression_list? RPAREN;

inline_assembly_stmt: ASM assembly_directive+ ENDASM;
assembly_directive: assembly_instruction (assembly_instruction_param (SEPERATOR assembly_instruction_param)*)? TERMINATOR;
assembly_instruction: ID (SINGLE_DOT ID)*;
assembly_instruction_param
    :NATURAL_NUBER_LIT
    |HEX_NUMBER_LIT
    |riscv_register
    |assembly_redirect
    |NATURAL_NUBER_LIT LPAREN riscv_register RPAREN
    ;
assembly_redirect: '<' ID '>';
riscv_register: ID;

basic_type: INT|REAL;

STRING_LIT: DOUBLEQUOTE (~["\\]|'\\' [n"])* DOUBLEQUOTE;
NATURAL_NUBER_LIT: DIGIT+;
NUMBER_LIT: DIGIT+ (SINGLE_DOT DIGIT+)? ([eE] [+-]? DIGIT+)?;
HEX_NUMBER_LIT: '0' [xh] [a-fA-F0-9]+;

TERMINATOR: ';';
SEPERATOR: ',';
DOUBLEQUOTE: '"';
BACKSLASH: '\\';
SINGLE_DOT: '.';

ASSIGNOP: ':=';

LBRACE: '[';
RBRACE: ']';
LPAREN: '(';
RPAREN: ')';

INT: 'int';
REAL: 'real';

ASM: '_asm';
ENDASM: '_endasm';
VAR: 'var';
FUNC: 'func';
ENDFUNC: 'endfunc';
RETURN: 'return';
TO: 'to';
BY: 'by';
AND: 'and';
MOD: 'mod';
DIV: 'div';
IF: 'if';
THEN: 'then';
ELSE: 'else';
ENDIF: 'endif';
ENDFOR: 'endfor';
OR: 'or';
DO: 'do';
PRINT: 'print';
READ: 'read';
WHILE: 'while';
ENDWHILE: 'endwhile';
FOR: 'for';
NOT: 'not';

ID: LETTER ALPA_NUM*;
ALPA_NUM: DIGIT|LETTER;
DIGIT: [0-9];
LETTER: [A-Za-z];

LINE_COMMENT: '%' ~[\r\n]* -> skip;
WS: [ \n\t\r]+ -> skip;