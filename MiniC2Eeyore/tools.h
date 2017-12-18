#include <cstdio>
#include <algorithm>
#include <stack>
using std::max;

extern int long_size;

struct _var
{
	char* name;
	int type_width;
	int length;
	int width();
};
struct _label
{
	char* name;
	_label();
};
struct _func
{
	const char* name;
	int type_width;
	int nparas;
	int def;
};

void addvar(const char* name, int type_width, int length = 0);
void addpara(const char* name, int type_width, int length = 0);
void addfunc(const char* name, int type_width, int nparas);
void addfuncdecl(const char* name, int type_width, int nparas);
void outfunc();
void inblock();
void outblock();
_var* addtmp(int type_width);
_var* findvar(const char* name);
void CHECKNARROW(_var *dst, _var *ori);
_func* calling(const char* name, int nparas);

extern std::stack<_label> labelstack;

extern int yylineno;
extern FILE* yyout;
#define OUT(format, ...) fprintf(yyout, format, ##__VA_ARGS__)
#define WARNING(format, ...) fprintf(stderr, "warning: %d: " format, yylineno, ##__VA_ARGS__)
extern int yyerror(const char*);
