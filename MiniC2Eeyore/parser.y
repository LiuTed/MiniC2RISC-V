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
extern node* varlist;
%}
%union {
	struct node *NPtr;
	char *container;
	int IntVal;
}

%token <IntVal> INTEGER
%token <container> IDENTIFIER
%token SEMI COMMA
%token TYPE_INT IF ELSE WHILE RETURN
%token LOGIC_AND LOGIC_OR LOGIC_NOT LE LT GE GT EQ NE ASSIGN ADD SUB MUL DIV MOD

%nonassoc IFX
%nonassoc ELSE
%right ASSIGN
%left LOGIC_OR
%left LOGIC_AND
%left NE EQ
%left LE LT GE GT
%left ADD SUB
%left MUL DIV MOD
%right LOGIC_NOT MINUS

%type <NPtr> VarDefn FuncDecl FuncDefn Goal Statement StatementList Block Expression DyadicArithOpt DyadicLogicOpt CmpOpt IDList NEIDList AssignList
%type <container> Identifier
%type <IntVal> VarList NEVarList

%start Goal

%%

Goal : VarDefn Goal {varlist = $$ = $1; $1->concat($2); delete $2;}
	 | FuncDefn Goal {varlist = $$ = $2;}
	 | FuncDecl Goal {varlist = $$ = $2; delete $1;}
	 | {varlist = $$ = NULL;}
;

VarDefn	: Type Identifier SEMI {$$ = new node(getnewID(Globl));
								$$->append("var %s\n", $$->ID);
								addconv($2, $$->ID);
								}
		| Type Identifier '[' INTEGER ']' SEMI {$$ = new node(getnewID(Globl));
												$$->append("var %d %s\n", 8*$4, $$->ID);
												addconv($2, $$->ID);
												}
;

VarDecl : Type Identifier {addpara($2);}
		| Type Identifier '[' ']' {addpara($2);}
		| Type Identifier '[' INTEGER ']' {addpara($2);}
;

FuncDefn : Type Identifier '(' VarList ')' '{' StatementList '}' {$$ = new node(NULL);
																  $$->append("f_%s [%d]\n", $2, $4);
																  $$->concat($7);
																  $$->append("return 0\nend f_%s\n", $2);
																  delete $7;
																  endfunc();
																  addfunc($2, $$);
																  }
;

FuncDecl : Type Identifier '(' VarList ')' SEMI {$$ = new node(NULL);
												 endfunc();
												 addfunc($2, NULL);
												 }
;

VarList : NEVarList {$$ = $1;}
		| {inblock(); $$ = 0;}
;

NEVarList : VarDecl COMMA NEVarList {$$ = $3 + 1;}
		  | VarDecl {$$ = 1;}
;

StatementList : Statement StatementList {$$ = $1; $$->concat($2); delete $2;}
			  | '{' InBlock StatementList '}' OutBlock StatementList {$$ = $3; $$->concat($6); delete $6;}
			  | {$$ = NULL;}
;

Block : '{' InBlock StatementList '}' OutBlock {$$ = $3;}
	  | Statement {$$ = $1;}
;

InBlock : {inblock();}
;

OutBlock : {outblock();}
;

Statement : IF '(' Expression ')' Block %prec IFX {$$ = parseif($3, $5);}
		  | IF '(' Expression ')' Block ELSE Block %prec ELSE {$$ = parseif($3, $5, $7);}
		  | WHILE '(' Expression ')' Block {$$ = parsewhile($3, $5);}
		  | Identifier ASSIGN AssignList SEMI {$$ = $3;
											   $3->append("%s = %s\n", conv($1), $3->ID);
											   delete[] $3->ID;
		  									   $3->ID = NULL;
		  									   }
		  | Identifier '[' Expression ']' ASSIGN AssignList SEMI {$$ = $3;
		  														  char *idx = getnewID(Temp);
		  														  $$->concat($6);
		  														  $$->append("var %s\n%s = 8 * %s\n", idx, idx, $3->ID);
		  														  $$->append("%s [%s] = %s\n", conv($1), idx, $6->ID);
		  														  delete[] $3->ID;
		  														  $3->ID = NULL;
		  														  delete $6;
		  														  }
		  | VarDefn {$$ = $1;}
		  | RETURN Expression SEMI {$$ = $2;
		  							$$->append("return %s\n", $2->ID);
		  							delete[] $2->ID;
		  							$2->ID = NULL;
		  							}
		  | SEMI {$$ = new node(NULL);}
;

AssignList : Expression
		   | Identifier ASSIGN AssignList {$$ = $3;
		   								   $3->append("%s = %s\n", conv($1), $3->ID);}
		   | Identifier '[' Expression ']' ASSIGN AssignList {$$ = $6;
		  													  char *idx = getnewID(Temp);
		  													  $$->concat($3);
		  													  $$->append("var %s\n%s = 8 * %s\n", idx, idx, $3->ID);
		  													  $$->append("%s [%s] = %s\n", conv($1), idx, $6->ID);
		  													  delete $3;
		  													  }

Expression : DyadicArithOpt
		   | DyadicLogicOpt
		   | CmpOpt
		   | Identifier '[' Expression ']' {$$ = new node(getnewID(Temp));
										  	char *idx = getnewID(Temp);
										  	$$->concat($3);
										  	$$->append("var %s\n%s = 8 * %s\n", idx, idx, $3->ID);
										  	$$->append("var %s\n%s = %s [%s]\n", $$->ID, $$->ID, conv($1), idx);
										  	delete[] idx;
										  	delete $3;
										  	}
		   | INTEGER {$$ = new node(getnewID(Temp));
		   			  $$->append("var %s\n%s = %d\n", $$->ID, $$->ID, $1);
		   			  }
		   | Identifier {$$ = new node(strdup(conv($1)));
		   				 }
		   | LOGIC_NOT Expression {$$ = new node(getnewID(Temp));
		   						   $$->concat($2);
		   						   $$->append("var %s\n%s = ! %s\n", $$->ID, $$->ID, $2->ID);
		   						   delete $2;
		   						   }
		   | SUB Expression %prec MINUS {$$ = new node(getnewID(Temp));
		   						   		 $$->concat($2);
		   						   		 $$->append("var %s\n%s = -%s\n", $$->ID, $$->ID, $2->ID);
		   						   		 delete $2;
		   						   		 }
		   | Identifier '(' IDList ')' {$$ = new node(getnewID(Temp));
		   								$$->append("var %s\n", $$->ID);
		   								$$->concat($3);
		   								$$->append("%s = call f_%s\n", $$->ID, $1);
		   								delete $3;
		   								}
		   | '(' Expression ')' {$$ = $2;}
;

IDList : NEIDList
	   | {$$ = new node(NULL);}
;

NEIDList : Identifier COMMA NEIDList {$$ = $3; $$->prepend("param %s\n", conv($1));}
		 | Identifier  {$$ = new node(NULL); $$->append("param %s\n", conv($1));}
;

DyadicArithOpt : Expression ADD Expression {$$ = parsedyop($1, ADD, $3);}
		  	   | Expression SUB Expression {$$ = parsedyop($1, SUB, $3);}
		  	   | Expression MUL Expression {$$ = parsedyop($1, MUL, $3);}
			   | Expression DIV Expression {$$ = parsedyop($1, DIV, $3);}
			   | Expression MOD Expression {$$ = parsedyop($1, MOD, $3);}
;

DyadicLogicOpt : Expression LOGIC_AND Expression {$$ = parsedyop($1, LOGIC_AND, $3);}
			   | Expression LOGIC_OR Expression {$$ = parsedyop($1, LOGIC_OR, $3);}
;

CmpOpt : Expression LT Expression {$$ = parsedyop($1, LT, $3);}
	   | Expression EQ Expression {$$ = parsedyop($1, EQ, $3);}
	   | Expression GT Expression {$$ = parsedyop($1, GT, $3);}
	   | Expression NE Expression {$$ = parsedyop($1, NE, $3);}
;

Identifier : IDENTIFIER {$$ = $1;}
;

Type : TYPE_INT
;

%%
int main()
{
	yyparse();
	genCode(yyout);
	return 0;
}
int yyerror(const char *msg)
{
	fprintf(stderr ,"error:line %d: %s\n", yylineno, msg);
	return 1;
}