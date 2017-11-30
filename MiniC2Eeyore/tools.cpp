#include "tools.h"
#include "y.tab.h"
#include <vector>
#include <utility>
#include <cstdio>
#include <cstring>
static vector<pair<char*,char*> > vec;

static int paracount = 0;
void addpara(char* id)
{
	if(paracount == 0)
	{
		inblock();
	}
	char *c = new char[16];
	sprintf(c, "p%d", paracount++);
	addconv(id, c);
}
void endfunc()
{
	outblock();
	paracount = 0;
}
void inblock()
{
	vec.push_back(pair<char*,char*>(NULL,NULL));
}
void outblock()
{
	while(!vec.empty())
	{
		auto b = vec.back();
		if(b.first || b.second)
		{
			delete[] b.first;
			vec.pop_back();
		}
		else break;
	}
	if(!vec.empty()) vec.pop_back();
}
void addconv(char* from,char* to)
{
	vec.push_back(pair<char*,char*>(from,to));
}
char *conv(char* from)
{
	for(auto it = vec.rbegin(); it!=vec.rend(); ++it)
	{
		if(!it->first) continue;
		if(!strcmp(it->first, from)) return it->second;
	}
	return NULL;
}
static vector<pair<char*,node*> > funclist;
void addfunc(char* name,node* ptr)
{
	for(auto& i : funclist)
	{
		if(!strcmp(i.first, name))
		{
			if(!(i.second==NULL&&ptr!=NULL))
			{
				yyerror("function redeclared");
				return;
			}
			else
			{
				i.second=ptr;
				return;
			}
		}
	}
	funclist.push_back(pair<char*,node*>(name,ptr));
}

void node::concat(node* p)
{
	if(p)
		code.splice(code.end(), p->code);
}


node* parseif(node* cond, node* then, node* fail)
{
	node *res = new node(NULL);
	res->concat(cond);
	char *faillabel = getnewID(Label);
	res->append("if %s == 0 goto %s\n", cond->ID, faillabel);
	res->concat(then);
	if(fail)
	{
		char *endlabel = getnewID(Label);
		res->append("goto %s\n%s:\n", endlabel, faillabel);
		res->concat(fail);
		res->append("%s:\n", endlabel);
	}
	else
	{
		res->append("%s:\n", faillabel);
	}
	delete cond;
	delete then;
	delete fail;
	return res;
}
node* parsewhile(node* cond, node* then)
{
	node *res = new node(NULL);
	char *looplabel = getnewID(Label);
	char *judgelabel = getnewID(Label);
	res->append("goto %s\n%s:\n", judgelabel, looplabel);
	res->concat(then);
	res->append("%s:\n", judgelabel);
	res->concat(cond);
	res->append("if %s != 0 goto %s\n", cond->ID, looplabel);
	delete cond;
	delete then;
	return res;
}
node* parsedyop(node* left, int op, node* right)
{
	node *res = new node(getnewID(Temp));
	res->concat(left);
	res->concat(right);
	const char *cop;
	switch(op)
	{
		case ADD:
			cop = " + ";
			break;
		case SUB:
			cop = " - ";
			break;
		case MUL:
			cop = " * ";
			break;
		case DIV:
			cop = " / ";
			break;
		case MOD:
			cop = " % ";
			break;
		case LOGIC_OR:
			cop = " || ";
			break;
		case LOGIC_AND:
			cop = " && ";
			break;
		case EQ:
			cop = " == ";
			break;
		case NE:
			cop = " != ";
			break;
		case LT:
			cop = " < ";
			break;
		case GT:
			cop = " > ";
			break;
		default:
			yyerror("unknown op in parsedyop");
	}
	res->append("var %s\n%s = %s%s%s\n", res->ID, res->ID, left->ID, cop, right->ID);
	delete left;
	delete right;
	return res;
}
static int count[3] = {0, 0, 0};//temp globl label
char* getnewID(ID_Type T)
{
	switch(T)
	{
		case Temp:
			sprintf(buf, "t%d", count[0]++);
			break;
		case Globl:
			sprintf(buf, "T%d", count[1]++);
			break;
		case Label:
			sprintf(buf, "l%d", count[2]++);
			break;
	}
	return strdup(buf);
}
node* varlist = NULL;
static void outputCode(FILE* fp, node* root)
{
	if(!root) return;
	for(const auto& c:root->code)
	{
		fprintf(fp, "%s", c);
	}
}
void genCode(FILE* fp)
{
	outputCode(fp, varlist);
	for(auto& i : funclist)
	{
		outputCode(fp, i.second);
	}
}