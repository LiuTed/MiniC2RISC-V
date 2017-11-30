#ifndef TOOLS_H
#define TOOLS_H
#include <list>
#include <cstdlib>
#include <cstdio>
#include <cstring>
using namespace std;

static char buf[32767];
void addpara(char*);
void endfunc();
void inblock();
void outblock();
void addconv(char*,char*);
char *conv(char*);
struct node
{
	char *ID;
	list<char*> code;
	node(char *_ID){ID = _ID;}
	~node(){delete[] ID;for(auto c : code) delete[] c;}
	template<typename... Args>
	void append(Args... args)
	{
		snprintf(buf, sizeof(buf), args...);
		code.push_back(strdup(buf));
	}
	template<typename... Args>
	void prepend(Args... args)
	{
		snprintf(buf, sizeof(buf), args...);
		code.push_front(strdup(buf));
	}
	void concat(node *);
};
void addfunc(char*,node*);
extern node *varlist;
node* parseif(node* cond, node* then, node* fail = NULL);
node* parsewhile(node* cond, node* then);
node* parsedyop(node* left, int op, node* right);
void genCode(FILE* fp);
extern int yyerror(const char*);


typedef enum {Temp, Globl, Label} ID_Type;
char* getnewID(ID_Type);
#endif