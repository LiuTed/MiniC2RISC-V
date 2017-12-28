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
char {return TYPE_CHAR;}

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
\'\\[abfnrtv\\?'"0]\' {
	switch(yytext[2])
	{
		case 'a': yylval.IntVal = '\a'; break;
		case 'b': yylval.IntVal = '\b'; break;
		case 'f': yylval.IntVal = '\f'; break;
		case 'n': yylval.IntVal = '\n'; break;
		case 'r': yylval.IntVal = '\r'; break;
		case 't': yylval.IntVal = '\t'; break;
		case 'v': yylval.IntVal = '\v'; break;
		case '\\': yylval.IntVal = '\\'; break;
		case '?': yylval.IntVal = '\?'; break;
		case '\'': yylval.IntVal = '\''; break;
		case '\"': yylval.IntVal = '\"'; break;
		case '0': yylval.IntVal = '\0'; break;
	}
	return INTEGER;
}
\'\\x[0-9a-fA-F]{2}\' {sscanf(&yytext[3], "%lx", &(yylval.IntVal)); return INTEGER;}
\'\\[0-8]{3}\' {sscanf(&(yytext[2]), "%lo", &(yylval.IntVal)); return INTEGER;}
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
