%{
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "y.tab.h"
using namespace std;
extern YYSTYPE yylval;
extern int yyerror(const char*);
%}

%option yylineno

delim [ \t]
ws {delim}+
digit [0-9]
integer ("+"|"-")?{digit}+

sc [,[\]=:]

%%

{ws} {}
"//".*\n {}
[\n] {return NL;}

[Tt]{digit}+ {yylval.name = strdup(yytext); return VARIABLE;}
p{digit}+ {yylval.name = strdup(yytext); return PARAMETER;}
l{digit}+ {yylval.name = strdup(yytext); return LABEL;}
f_[_a-zA-Z][_a-zA-Z0-9]* {yylval.name = strdup(yytext); return FUNC;}
{integer} {yylval.name = strdup(yytext); return INTEGER;}

var {return VAR;}
call {return CALL;}
end {return END;}
goto {return GOTO;}
param {return PARAM;}
return {return RETURN;}
if {return IF;}

{sc} {return *yytext;}
"+" {return ADD;}
"-" {return SUB;}
"*" {return MUL;}
"/" {return DIV;}
"%" {return MOD;}
"&&" {return AND;}
"||" {return OR;}
"!" {return NOT;}
"<" {return LT;}
">" {return GT;}
"==" {return EQ;}
"!=" {return NE;}

. {yyerror("illegal token");
}

%%

int yywrap()
{
	return 1;
}
