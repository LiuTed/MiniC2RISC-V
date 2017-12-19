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
int long_size;
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
	printf("\t.size\t%s, %d\n", $1, long_size);
	printf("%s:\n", $1);
	if(long_size == 8)
		printf("\t.dword\t%d\n\n", $3);
	else if(long_size == 4)
		printf("\t.word\t%d\n\n", $3);
	else yyerror("unknown long width");
}
| VAR '=' MALLOC Integer {printf("\n\t.comm\t%s, %d, %d\n\n", $1, $4, long_size);}
;

Integer:
  INT {$$ = atoi($1);}
| '-' INT {$$ = -atoi($2);}

FuncDecl:
FUNC '[' Integer ']' '[' Integer ']' '\n' {
	putchar('\n');
	stk = $6*long_size+long_size;
	printf("\t.text\n");
	printf("\t.align\t2\n");
	printf("\t.global\t%s\n", $1);
	printf("\t.type\t%s, @function\n", $1);
	printf("%s:\n", $1);
	printf("\tadd\tsp, sp, -%d\n", stk);
	if(long_size == 8)
		printf("\tsd\tra, %d(sp)\n", stk-8);
	else if(long_size == 4)
		printf("\tsw\tra, %d(sp)\n", stk-4);
	else yyerror("unknown long width");
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
| REG '[' Integer ',' Integer ']' '=' REG {
	switch($5)
	{
		case 1:
			printf("\tsb\t%s, %d(%s)\n", $8, $3, $1);
			break;
		case 2:
			printf("\tsh\t%s, %d(%s)\n", $8, $3, $1);
			break;
		case 4:
			printf("\tsw\t%s, %d(%s)\n", $8, $3, $1);
			break;
		case 8:
			printf("\tsd\t%s, %d(%s)\n", $8, $3, $1);
			break;
		default:
			yyerror("unexpected size");
	}
}
| REG '=' REG '[' Integer ',' Integer ']' {
	switch($7)
	{
		case 1:
			printf("\tlb\t%s, %d(%s)\n", $1, $5, $3);
			break;
		case 2:
			printf("\tlh\t%s, %d(%s)\n", $1, $5, $3);
			break;
		case 4:
			printf("\tlw\t%s, %d(%s)\n", $1, $5, $3);
			break;
		case 8:
			printf("\tld\t%s, %d(%s)\n", $1, $5, $3);
			break;
		default:
			yyerror("unexpected size");
	}
}
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
| STORE REG Integer {
	if(long_size == 8)
		printf("\tsd\t%s, %d(sp)\n", $2, $3*8);
	else
		printf("\tsw\t%s, %d(sp)\n", $2, $3*4);
}
| LOAD Integer REG {
	if(long_size == 8)
		printf("\tld\t%s, %d(sp)\n", $3, $2*8);
	else
		printf("\tlw\t%s, %d(sp)\n", $3, $2*4);
}
| LOAD VAR REG {
	if(long_size == 8)
		printf("\tld\t%s, %s\n", $3, $2);
	else
		printf("\tlw\t%s, %s\n", $3, $2);
}
| LOADADDR Integer REG {printf("\tadd\t%s, sp, %d\n", $3, $2*8);}
| LOADADDR VAR REG {printf("\tla\t%s, %s\n", $3, $2);}
| RETURN {
	if(long_size == 8)
		printf("\tld\tra, %d(sp)\n", stk-8);
	else
		printf("\tlw\tra, %d(sp)\n", stk-4);
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
int main(int argc, char* argv[])
{
	long_size = 8;
	for(int i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "-m32"))
			long_size = 4;
	}
	yyparse();
	return 0;
}
int yyerror(const char *msg)
{
	fprintf(stderr ,"error:%d: %s\n", yylineno, msg);
	return 1;
}