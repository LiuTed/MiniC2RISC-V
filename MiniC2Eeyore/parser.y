%{
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "tools.h"
using namespace std;
extern FILE* yyin;
extern FILE* yyout;
extern int yylineno;
int yyerror(const char*);
int yylex();
#ifndef INT_MAX
#define INT_MAX 0x7fffffff
#endif
%}
%union {
	struct _var *var;
	char *container;
	long int IntVal;
}

%token <IntVal> INTEGER
%token <container> IDENTIFIER
%token SEMI COMMA
%token TYPE_INT TYPE_LONG TYPE_CHAR IF ELSE WHILE RETURN DO
%token LOGIC_AND LOGIC_OR LOGIC_NOT LT GT EQ NE ADD SUB MUL DIV MOD

%nonassoc IFX
%nonassoc ELSE
%left COMMA
%left EL
%right '='
%left LOGIC_OR
%left LOGIC_AND
%left NE EQ
%left LT GT
%left ADD SUB
%left MUL DIV MOD
%right LOGIC_NOT MINUS

%type<var> Expr BOP Dummy1
%type<IntVal> Type VarDeclList NEVarDeclList ExprList NEExprList

%start S

%%

S:
  S VarDefn
| S FuncDefn
| S FuncDecl
|
;

VarDefn:
  Type IDENTIFIER SEMI {addvar($2, $1);}
| Type IDENTIFIER '[' INTEGER ']' SEMI {addvar($2, $1, $4);}
| Type IDENTIFIER '=' INTEGER SEMI {
	addvar($2, $1);
	_var *v = findvar($2);
	OUT("%s = %ld\n", v->name, $4);
}
;

VarDecl:
  Type IDENTIFIER {addpara($2, $1);}
| Type IDENTIFIER '[' ']' {addpara($2, $1, -1);}
| Type IDENTIFIER '[' INTEGER ']' {addpara($2, $1, $4);}
;

FuncDefn: Type IDENTIFIER '(' VarDeclList ')' '{' {
	addfunc($2, $1, $4);
	inblock();
	OUT("f_%s [%ld]\n", $2, $4);
} StatementList '}' {
	outblock();
	outfunc();
	OUT("return 0\n");
	OUT("end f_%s\n", $2);
}
;

FuncDecl: Type IDENTIFIER '(' VarDeclList ')' SEMI {
	addfuncdecl($2, $1, $4);
}
;

VarDeclList:
  NEVarDeclList
| {$$ = 0;}
;

NEVarDeclList:
  VarDecl {$$ = 1;}
| NEVarDeclList COMMA VarDecl {$$ = $1 + 1;}
;

Type:
  TYPE_INT {$$ = 4;}
| TYPE_LONG {$$ = long_size;}
| TYPE_CHAR {$$ = 1;}
;

Block:
  Statement
| '{' {
	inblock();
} StatementList '}' {outblock();}
;

StatementList:
  StatementList Block
|
;

IFHead:
  IF '(' Expr ')' {
  	_label fail;
  	labelstack.push(fail);
  	OUT("if %s == 0 goto %s\n", $3->name, fail.name);
} Block
;

Statement:
  IFHead %prec IFX {
  	_label fail = labelstack.top();
  	labelstack.pop();
	OUT("%s:\n", fail.name);
}
| IFHead ELSE {
  	_label fail = labelstack.top();
  	labelstack.pop();
	_label lend;
  	labelstack.push(lend);
	OUT("goto %s\n", lend.name);
	OUT("%s:\n", fail.name);
} Block {
  	_label lend = labelstack.top();
  	labelstack.pop();
	OUT("%s:\n", lend.name);
}
| WHILE {
	_label judge;
	_label fail;
	labelstack.push(judge);
	labelstack.push(fail);
	OUT("%s:\n", judge.name);
} '(' Expr ')' {
	_label fail = labelstack.top();
	OUT("if %s == 0 goto %s\n", $4->name, fail.name);
} Block {
	_label fail = labelstack.top();
	labelstack.pop();
	_label judge = labelstack.top();
	labelstack.pop();
	OUT("goto %s\n", judge.name);
	OUT("%s:\n", fail.name);
}
| DO {
	_label start;
	labelstack.push(start);
	OUT("%s:\n", start.name);
} Block WHILE '(' Expr ')' SEMI {
	_label start = labelstack.top();
	labelstack.pop();
	OUT("if %s != 0 goto %s\n", $6->name, start.name);
}
| Expr SEMI
| VarDefn
| RETURN Expr SEMI {
	OUT("return %s\n", $2->name);
}
| SEMI
;

Expr:
  BOP
| SUB Expr %prec MINUS {
	$$ = addtmp($2->width());
	OUT("%s = -%s\n", $$->name, $2->name);
}
| LOGIC_NOT Expr {
	$$ = addtmp(4);
	OUT("%s = !%s\n", $$->name, $2->name);
}
| IDENTIFIER '=' Expr {
	$$ = findvar($1);
	CHECKNARROW($$, $3);
	OUT("%s = %s\n", $$->name, $3->name);
}
| IDENTIFIER '[' Expr ']' '=' Expr {
	_var *arr = findvar($1);
	$$ = addtmp(arr->type_width);
	CHECKNARROW($$, $6);
	_var *idx = addtmp(long_size);
	OUT("%s = %s * %d\n", idx->name, $3->name, arr->type_width);
	OUT("%s [%s, %d] = %s\n", arr->name, idx->name, min($6->width(), arr->type_width), $6->name);
	OUT("%s = %s\n", $$->name, $6->name);
}
| INTEGER {
	$$ = addtmp($1>INT_MAX?long_size:4);
	OUT("%s = %ld\n", $$->name, $1);
}
| IDENTIFIER {
	$$ = findvar($1);
}
| IDENTIFIER '[' Expr ']' {
	_var *arr = findvar($1);
	$$ = addtmp(arr->type_width);
	_var * idx = addtmp($3->width());
	OUT("%s = %s * %d\n", idx->name, $3->name, arr->type_width);
	OUT("%s = %s [%s, %d]\n", $$->name, arr->name, idx->name, arr->type_width);
}
| IDENTIFIER '(' Dummy1 {
	_func *f = calling($1);
	$3 = addtmp(f->type_width);
} ExprList ')' {
	$$ = $3;
	OUT("%s = call f_%s\n", $$->name, $1);
}
| '(' Expr ')' {$$ = $2;}
;

Dummy1: {$$=NULL;}
;

ExprList:
  NEExprList
| {$$ = 0;}
;

NEExprList:
  Expr {OUT("param %s\n", $1->name); $$ = 1;} %prec EL
| NEExprList COMMA Expr {OUT("param %s\n", $3->name); $$ = $1+1;} %prec EL
;

BOP:
  Expr ADD Expr {
  	$$ = addtmp(max($1->width(), $3->width()));
  	OUT("%s = %s + %s\n", $$->name, $1->name, $3->name);
}
| Expr SUB Expr {
  	$$ = addtmp(max($1->width(), $3->width()));
  	OUT("%s = %s - %s\n", $$->name, $1->name, $3->name);
}
| Expr MUL Expr {
  	$$ = addtmp(max($1->width(), $3->width()));
  	OUT("%s = %s * %s\n", $$->name, $1->name, $3->name);
}
| Expr DIV Expr {
  	$$ = addtmp(max($1->width(), $3->width()));
  	OUT("%s = %s / %s\n", $$->name, $1->name, $3->name);
}
| Expr MOD Expr {
  	$$ = addtmp(max($1->width(), $3->width()));
  	OUT("%s = %s %% %s\n", $$->name, $1->name, $3->name);
}
| Expr LT Expr {
  	$$ = addtmp(4);
  	OUT("%s = %s < %s\n", $$->name, $1->name, $3->name);
}
| Expr GT Expr {
  	$$ = addtmp(4);
  	OUT("%s = %s > %s\n", $$->name, $1->name, $3->name);
}
| Expr EQ Expr {
  	$$ = addtmp(4);
  	OUT("%s = %s == %s\n", $$->name, $1->name, $3->name);
}
| Expr NE Expr {
  	$$ = addtmp(4);
  	OUT("%s = %s != %s\n", $$->name, $1->name, $3->name);
}
| Expr LOGIC_AND Expr {
  	$$ = addtmp(4);
  	OUT("%s = %s && %s\n", $$->name, $1->name, $3->name);
}
| Expr LOGIC_OR Expr {
  	$$ = addtmp(4);
  	OUT("%s = %s || %s\n", $$->name, $1->name, $3->name);
}
| Expr COMMA Expr {
	$$ = $3;
}

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
	fprintf(stderr ,"error: line %d: %s\n", yylineno, msg);
	return 1;
}
