%{
#include <cstdio>
#include <cstring>
#include <cstdlib>
//#include "tools.h"
using namespace std;
extern FILE* yyin;
extern FILE* yyout;
extern int yylineno;
int yyerror(const char*);
int yylex();
int stk;
%}

%union {
	const char* s;
	int ival;
}

%token<s> REG VAR LABEL FUNC INT
%token GOTO IF CALL RETURN STORE LOAD LOADADDR MALLOC END

%type<s> OP LOP
%type<ival> Integer

%start S

%%

S: DL;

DL:
  DL FuncDecl '\n'
| DL GVarDecl '\n'
|
;

GVarDecl:
  VAR '=' Integer {
  	putchar('\n');
	printf("\t.global\t%s\n", $1);
	printf("\t.section\t.sdata\n");
	printf("\t.align\t2\n");
	printf("\t.type\t%s, @object\n", $1);
	printf("\t.size\t%s, 8\n", $1);
	printf("%s:\n", $1);
	printf("\t.dword\t%d\n\n", $3);
}
| VAR '=' MALLOC Integer {printf("\n\t.comm\t%s, %d, 8\n\n", $1, $4);}
;

Integer:
  INT {$$ = atoi($1);}
| '-' INT {$$ = -atoi($2);}

FuncDecl:
FUNC '[' Integer ']' '[' Integer ']' '\n' {
	putchar('\n');
	stk = $6*8+8;
	printf("\t.text\n");
	printf("\t.align\t2\n");
	printf("\t.global\t%s\n", $1);
	printf("\t.type\t%s, @function\n", $1);
	printf("%s:\n", $1);
	printf("\tadd\tsp, sp, -%d\n", stk);
	printf("\tsd\tra, %d(sp)\n", stk-8);
}
Body END FUNC {
	stk = 0;
	printf("\t.size\t%s, .-%s\n\n", $1, $1);
}
;

Body:
  Body Exp '\n'
|
;

Exp:
  REG '=' REG OP REG {
  	switch($4[0])
  	{
  	case '+': printf("\tadd\t%s, %s, %s\n", $1, $3, $5); break;
  	case '-': printf("\tsub\t%s, %s, %s\n", $1, $3, $5); break;
  	case '*': printf("\tmul\t%s, %s, %s\n", $1, $3, $5); break;
  	case '/': printf("\tdiv\t%s, %s, %s\n", $1, $3, $5); break;
  	case '%': printf("\trem\t%s, %s, %s\n", $1, $3, $5); break;
  	case '<': printf("\tslt\t%s, %s, %s\n", $1, $3, $5); break;
  	case '>': printf("\tsgt\t%s, %s, %s\n", $1, $3, $5); break;
  	case '|':
  		printf("\tor\t%s, %s, %s\n", $1, $3, $5);
  		printf("\tsnez\t%s, %s\n", $1, $1);
  		break;
  	case '&':
  		printf("\tsnez\t%s, %s\n", $1, $3);
  		printf("\tslti\t%s, %s, 31\n", $1, $1);
  		printf("\tsrai\t%s, %s, 31\n", $1, $1);
  		printf("\tand\t%s, %s, %s\n", $1, $1, $5);
  		printf("\tsnez\t%s, %s\n", $1, $1);
  		break;
  	case '@':
  		printf("\txor\t%s, %s, %s\n", $1, $3, $5);
  		printf("\tseqz\t%s, %s\n", $1, $1);
  		break;
  	case '~':
  		printf("\txor\t%s, %s, %s\n", $1, $3, $5);
  		printf("\tsnez\t%s, %s\n", $1, $1);
  		break;
  	default:
  		yyerror("unsupported binary operator");
  	}
}
| REG '=' OP REG {
	switch($3[0])
	{
	case '!':
		printf("\tseqz\t%s, %s\n", $1, $4);
		break;
	case '-':
		printf("\tsub\t%s, x0, %s\n", $1, $4);
		break;
	default:
		yyerror("unsupported unary operator");
	}
}
| REG '=' REG OP Integer {
	switch($4[0])
	{
	case '+': printf("\tadd\t%s, %s, %d\n", $1, $3, $5); break;
	case '<': printf("\tslt\t%s, %s, %d\n", $1, $3, $5); break;
	default: yyerror("unsupported i-operation");
	}
}
| REG '=' REG {printf("\tmv\t%s, %s\n", $1, $3);}
| REG '=' Integer {printf("\tli\t%s, %d\n", $1, $3);}
| REG '[' Integer ']' '=' REG {printf("\tsd\t%s, %d(%s)\n", $6, $3, $1);}
| REG '=' REG '[' Integer ']' {printf("\tld\t%s, %d(%s)\n", $1, $5, $3);}
| IF REG LOP REG GOTO LABEL {
	switch($3[0])
	{
	case '<': printf("\tblt\t%s, %s, .%s\n", $2, $4, $6); break;
	case '>': printf("\tbgt\t%s, %s, .%s\n", $2, $4, $6); break;
	case '@': printf("\tbeq\t%s, %s, .%s\n", $2, $4, $6); break;
	case '~': printf("\tbne\t%s, %s, .%s\n", $2, $4, $6); break;
	case '|':
		printf("\tbne\t%s, x0, .%s\n", $2, $6);
		printf("\tbne\t%s, x0, .%s\n", $4, $6);
		break;
	case '&':
		printf("\tbeq\t%s, x0, .fail%d\n", $2, yylineno);
		printf("\tbne\t%s, x0, .%s\n", $4, $6);
		printf(".fail%d:\n", yylineno);
		break;
	default: yyerror("unsupported logical-operation");
	}
}
| GOTO LABEL {printf("\tj\t.%s\n", $2);}
| LABEL ':' {printf(".%s:\n", $1);}
| CALL FUNC {printf("\tcall\t%s\n", $2);}
| STORE REG Integer {printf("\tsd\t%s, %d(sp)\n", $2, $3*8);}
| LOAD Integer REG {printf("\tld\t%s, %d(sp)\n", $3, $2*8);}
| LOAD VAR REG {printf("\tld\t%s, %s\n", $3, $2);}
| LOADADDR Integer REG {printf("\tadd\t%s, sp, %d\n", $3, $2*8);}
| LOADADDR VAR REG {printf("\tla\t%s, %s\n", $3, $2);}
| RETURN {
	printf("\tld\tra, %d(sp)\n", stk-8);
	printf("\tadd\tsp, sp, %d\n", stk);
	printf("\tjr\tra\n");
}
;

OP:
  '+' {$$="+";}
| '-' {$$="-";}
| '*' {$$="*";}
| '/' {$$="/";}
| '%' {$$="%";}
| '!' {$$="!";}
| LOP
;

LOP:
  '<' {$$="<";}
| '>' {$$=">";}
| '@' {$$="@=";}
| '&' {$$="&&";}
| '|' {$$="||";}
| '~' {$$="~=";}
;


%%
int main()
{
	yyparse();
	return 0;
}
int yyerror(const char *msg)
{
	fprintf(stderr ,"error:%d: %s\n", yylineno, msg);
	return 1;
}