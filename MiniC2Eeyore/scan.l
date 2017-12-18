%{
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "parser.tab.h"
using namespace std;
extern YYSTYPE yylval;
extern int yyerror(const char*);
%}

%option yylineno

delim [ \t\n]
ws {delim}+
digit [0-9]
integer {digit}+
identifier [_A-Za-z][_0-9A-Za-z]*
brace [()[\]{}]

%%

{ws} {}
"/*"([^*]|"*"[^/])*"*/" {}
"//".*$ {}
";" {return SEMI;}
"," {return COMMA;}

int {return TYPE_INT;}
long {return TYPE_LONG;}

if {return IF;}
else {return ELSE;}
while {return WHILE;}
do {return DO;}
return {return RETURN;}
{brace} {return *yytext;}

{identifier} {yylval.container = strdup(yytext);
			  return IDENTIFIER;}
{integer} {yylval.IntVal = atoi(yytext);
		   return INTEGER;}
\'.\' {yylval.IntVal = yytext[1]; return INTEGER;}

"&&" {return LOGIC_AND;}
"||" {return LOGIC_OR;}
"<"  {return LT;}
">"  {return GT;}
"==" {return EQ;}
"!=" {return NE;}
"="  {return '=';}
"!"  {return LOGIC_NOT;}
"+"  {return ADD;}
"-"  {return SUB;}
"*"  {return MUL;}
"/"  {return DIV;}
"%"  {return MOD;}

. {yyerror("unknown character");}

%%

int yywrap()
{
	return 1;
}
