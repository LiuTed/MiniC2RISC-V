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
extern vector<node*> cfc;
const char* imm = "imm";
const char* x0 = "x0";
%}

%union{
	const char *name;
	int op;
	struct node *NPtr;
}

%token ADD SUB MUL DIV MOD AND OR NOT LT GT EQ NE
%token<name> VARIABLE PARAMETER LABEL FUNC INTEGER
%token VAR CALL END GOTO PARAM RETURN IF
%token NL

%token AEV VEA VEI VEV INFUNC OUTFUNC DYOP UNOP DYOPI NOP

%type<op> OP1 OP2 LOP
%type<NPtr> Expr GExpr VarDecl
%type<name> Lval Rval

%start S

%%

S: DL
;

DL:
  DL NL FuncDecl
| DL NL VarDecl {cfc.push_back($3);}
| DL NL GExpr {if($3) cfc.push_back($3);}
| FuncDecl
| VarDecl {cfc.push_back($1);}
| GExpr {if($1) cfc.push_back($1);}
;

VarDecl:
  VAR VARIABLE {$$ = new node; $$->t = VAR; $$->p.assign({$2});}
| VAR INTEGER VARIABLE {$$ = new node; $$->t = VAR; $$->p.assign({(char*)$2,$3});}
;

FuncDecl: FUNC '[' INTEGER ']'  {node *tmp = new node; tmp->t = INFUNC; tmp->p.assign({$1,$3}); cfc.push_back(tmp);}
NL FuncBody END FUNC {node *tmp2 = new node; tmp2->t = OUTFUNC; tmp2->p.assign({$1,$3,$9}); cfc.push_back(tmp2);}
;

FuncBody:
  FuncBody Expr NL {if($2) cfc.push_back($2);}
| FuncBody VarDecl NL {cfc.push_back($2);}
| 
;

Rval:
  INTEGER {node *ptr = new node; ptr->t = VEI; ptr->p.assign({imm,$1}); cfc.push_back(ptr); $$=imm;}
| Lval
;

Lval:
  VARIABLE
| PARAMETER
;

GExpr:
  Lval '=' INTEGER {$$ = new node; $$->t = VEI; $$->p.assign({$1,$3});}
| {$$ = nullptr;}
;

Expr:
  GExpr
| Lval '=' Lval {$$ = new node; $$->t = VEV; $$->p.assign({$1,$3});}
| Lval '=' Rval OP2 Rval {$$ = new node; $$->t = DYOP; $$->p.assign({$1,$3,(char*)$4,$5});}
| Lval '=' OP1 Rval {$$ = new node; $$->t = UNOP; $$->p.assign({$1,(char*)$3,$4});}
| Lval '[' Rval ']' '=' Rval {$$ = new node; $$->t = AEV; $$->p.assign({$1,$6,$3});}
| Lval '=' Lval '[' Rval ']' {$$ = new node; $$->t = VEA; $$->p.assign({$1,$3,$5});}
| IF Rval LOP Rval GOTO LABEL {$$ = new node; $$->t = IF; $$->p.assign({$2,(char*)$3,$4,$6});}
| GOTO LABEL {$$ = new node; $$->t = GOTO; $$->p.assign({$2});}
| LABEL ':' {$$ = new node; $$->t = LABEL; $$->p.assign({$1});}
| PARAM Rval {$$ = new node; $$->t = PARAM; $$->p.assign({$2});}
| Lval '=' CALL FUNC {$$ = new node; $$->t = CALL; $$->p.assign({$1,$4});}
| RETURN Rval {$$ = new node; $$->t = RETURN; $$->p.assign({$2});}
;

OP1: NOT {$$ = NOT;} | SUB {$$ = SUB;}
;

OP2: LOP | ADD {$$ = ADD;} | SUB {$$ = SUB;}
| MUL {$$ = MUL;} | DIV {$$ = DIV;} | MOD {$$ = MOD;}
;

LOP: AND {$$ = AND;} | OR {$$ = OR;} | LT {$$ = LT;} 
| GT {$$ = GT;} | EQ {$$ = EQ;} | NE {$$ = NE;}
;

%%
int main()
{
	yyparse();
	analyze();
	return 0;
}
int yyerror(const char *msg)
{
	fprintf(stderr ,"error:%d: %s\n", yylineno, msg);
	return 1;
}