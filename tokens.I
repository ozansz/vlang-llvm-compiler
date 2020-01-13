%{
#include <string>
#include "node.hpp"
#include "parser.hpp"
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)
extern "C" int yywrap() { }
%}

%%

[ \t\n\r]+                          ;
"%"~[\r\n]*                         ;
[a-zA-Z][a-zA-Z0-9]*                SAVE_TOKEN; return TIDENTIFIER;
[0-9]+"."[0-9]+([eE][+-]?[0-9+])?   SAVE_TOKEN; return TDOUBLE;
[0-9]+                              SAVE_TOKEN; return TINTEGER;
"\""(~["\\]|'\\' [n"])*"\""         SAVE_TOKEN; return TSTRINGLIT;
":="                                return TOKEN(TASSIGN);
"="                                 return TOKEN(TCEQ);
"<>"                                return TOKEN(TCNE);
"<"                                 return TOKEN(TCLT);
"<="                                return TOKEN(TCLE);
">"                                 return TOKEN(TCGT);
">="                                return TOKEN(TCGE);
"("                                 return TOKEN(TLPAREN);
")"                                 return TOKEN(TRPAREN);
"["                                 return TOKEN(TLBRACE);
"]"                                 return TOKEN(TRBRACE);
"."                                 return TOKEN(TDOT);
","                                 return TOKEN(TCOMMA);
":"                                 return TOKEN(TCOLON);
";"                                 return TOKEN(TSEMICOLON);
"+"                                 return TOKEN(TPLUS);
"-"                                 return TOKEN(TMINUS);
"*"                                 return TOKEN(TMUL);
"/"                                 return TOKEN(TDIV);
"mod"                               return TOKEN(TNUMMOD);
"div"                               return TOKEN(TNUMDIV);
"int"                               return TOKEN(TINT);
"real"                              return TOKEN(TREAL);
"var"                               return TOKEN(TVAR);
"func"                              return TOKEN(TFUNC);
"endfunc"                           return TOKEN(TENDFUNC);
"return"                            return TOKEN(TRETURN);
"and"                               return TOKEN(TLOGICAND);
"not"                               return TOKEN(TLOGICNOT);
"or"                                return TOKEN(TLOGICOR);
"if"                                return TOKEN(TIF);
"then"                              return TOKEN(TTHEN);
"else"                              return TOKEN(TELSE);
"endif"                             return TOKEN(TENDIF);
"do"                                return TOKEN(TDO);
"for"                               return TOKEN(TFOR);
"endfor"                            return TOKEN(TENDFOR);
"to"                                return TOKEN(TTO);
"by"                                return TOKEN(TBY);
"while"                             return TOKEN(TWHILE);
"endwhile"                          return TOKEN(TENDWHILE);
"print"                             return TOKEN(TPRINT);
"read"                              return TOKEN(TREAD);
.                                   printf("Unknown token: %s \n", yytext); yyterminate();

%%