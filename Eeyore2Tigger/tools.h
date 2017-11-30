#ifndef TOOLS_H
#define TOOLS_H
#include <vector>
#include <cstring>
#include "y.tab.h"
#include "bitmap.h"
using namespace std;


extern int yylineno;
struct node
{
	int lineno;
	bitmap live;
	int t, id;//type
	vector<const char*> p;//params
	node(){lineno = yylineno;}
};

extern vector<node*> cfc;//control flow chart (node list in fact)

#define ERR(lineno, msg, ...) {fprintf(stderr, "%s: line %d: " msg "\n", "error", lineno, ## __VA_ARGS__); assert(false);}
#define WAR(lineno, msg, ...) fprintf(stderr, "%s: line %d: " msg "\n", "warning", lineno, ## __VA_ARGS__);

extern int yyerror(const char*);

extern const char* imm;
extern const char* x0;

void analyze();

#endif
