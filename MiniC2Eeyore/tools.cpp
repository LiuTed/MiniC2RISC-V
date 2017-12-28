#include "tools.h"
#include <vector>
#include <cstdio>
#include <cstring>
using namespace std;

stack<_label> labelstack;
int long_size;

int _var::width()
{
	if(length != 0) return long_size;
	else return type_width;
}

_label::_label()
{
	static int n = 0;
	name = new char[32];
	snprintf(name, 32, "l%d", n++);
}
using vlt = pair<const char*, _var>;
static vector<vlt> varlist;
static vector<_func> flist;

void addvar(const char* name, int type_width, int length)
{
	static int varn = 0;
	_var var;
	var.name = new char[32];
	snprintf(var.name, 32, "T%d", varn);
	varn++;
	var.type_width = type_width;
	var.length = length;
	varlist.push_back(vlt(name, var));
	if(length > 0)
		OUT("var %d %s\n", length*type_width, var.name);
	else
		OUT("var %s\n", var.name);
}
static int paracnt = 0;
void addpara(const char* name, int type_width, int length)
{
	_var var;
	var.name = new char[32];
	snprintf(var.name, 32, "p%d", paracnt);
	paracnt++;
	var.type_width = type_width;
	var.length = length;
	varlist.push_back(vlt(name, var));
}
void addfunc(const char* name, int type_width, int nparas)
{
	for(auto i = flist.begin(); i != flist.end(); ++i)
	{
		if(!strcmp(name, i->name))
		{
			if(i->def == 1)
			{
				yyerror("redefine function");
				exit(0);
			}
			i->def = 1;
			flist.push_back(*i);
			return;
		}
	}
	_func f;
	f.name = name;
	f.type_width = type_width;
	f.nparas = nparas;
	f.def = 1;//defined
	flist.push_back(f);
	flist.push_back(f);
}
void addfuncdecl(const char* name, int type_width, int nparas)
{
	for(auto i = flist.begin(); i != flist.end(); ++i)
	{
		if(!strcmp(name, i->name))
		{
			yyerror("redeclearing function");
			exit(0);
		}
	}
	_func f;
	f.name = name;
	f.type_width = type_width;
	f.nparas = nparas;
	f.def = 0;//decleared
	flist.push_back(f);
	flist.push_back(f);
	outfunc();
}
void outfunc()
{
	_func f = flist.back();
	for(int i = 0; i < f.nparas; i++)
	{
		_var v = varlist.back().second;
		delete[] v.name;
		varlist.pop_back();
	}
	flist.pop_back();
	paracnt = 0;
}
void inblock()
{
	varlist.push_back(vlt(NULL, _var()));
}
void outblock()
{
	while(!varlist.empty())
	{
		if(varlist.back().first)
			varlist.pop_back();
		else
		{
			varlist.pop_back();
			break;
		}
	}
}
_var* addtmp(int type_width)
{
	static int cnt = 0;
	_var *var = new _var;
	var->name = new char[32];
	snprintf(var->name, 32, "t%d", cnt);
	cnt++;
	var->type_width = type_width;
	var->length = 0;
	OUT("var %s\n", var->name);
	return var;
}
_var* findvar(const char* name)
{
	for(auto i = varlist.size() - 1; i != 0; i--)
	{
		if(!varlist[i].first) continue;
		if(!strcmp(varlist[i].first, name))
			return &(varlist[i].second);
	}
	if(!varlist[0].first) return NULL;
	if(!strcmp(varlist[0].first, name))
		return &(varlist[0].second);
	return NULL;
}
void CHECKNARROW(_var *dst, _var *ori)
{
	if(dst->width() < ori->width())
		WARNING("narrowing conversion");
}
_func* calling(const char* name)
{
	for(size_t i = 0; i < flist.size(); i++)
	{
		if(!strcmp(flist[i].name, name))
			return &(flist[i]);
	}
	yyerror("unknown function");
	exit(0);
	return NULL;
}
