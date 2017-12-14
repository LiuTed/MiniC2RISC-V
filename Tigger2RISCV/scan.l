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
integer {digit}+

sc [[\]=:]

%%

{ws} {}
"//".*\n {}
^({ws}|\n)*\n {}
[\n] {return '\n';}

s[0-9]|s1[01]|t[0-6]|a[0-7]|x0 {yylval.s = strdup(yytext); return REG;}
v{digit}+ {yylval.s = strdup(yytext); return VAR;}
l{digit}+ {yylval.s = strdup(yytext); return LABEL;}
f_[_0-9a-zA-Z]* {yylval.s = strdup(yytext+2); return FUNC;}
[-+*/%!<>]|"&&"|"||" {return *yytext;}
"!=" {return '~';}
"==" {return '@';}
{sc} {return *yytext;}
{integer} {yylval.s = strdup(yytext); return INT;}
goto {return GOTO;}
if {return IF;}
call {return CALL;}
return {return RETURN;}
store {return STORE;}
load {return LOAD;}
loadaddr {return LOADADDR;}
malloc {return MALLOC;}
end {return END;}


. {yyerror("illegal token");}

%%

int yywrap()
{
	return 1;
}
