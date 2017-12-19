#include "tools.h"
#include "y.tab.h"
#include <vector>
#include <map>
#include <set>
#include <cstring>
#include <algorithm>
#include <cassert>
using namespace std;
int long_size;

namespace std
{
template<>
struct less<char*>
{
	bool operator () (const char* &a, const char* &b) const
	{
		return strcmp(a, b) < 0;
	}
};
template<>
struct less<const char*>
{
	bool operator () (const char* const &a, const char* const &b) const
	{
		return strcmp(a, b) < 0;
	}
};
}
struct _variable
{
	const char* name;
	const char* belongto;
	int length;//0 means variable, > 0 means array
	int color;
	int index;
	int startat;
	int init;
	bool initialized;
	bool operator < (const _variable& b) const
	{
		return less<const char*>()(name, b.name);
	}
};
struct _label
{
	int name;
	const char* belongto;
	int target;
	bool operator < (const _label& b) const
	{
		return name < b.name;
	}
};
struct _func
{
	const char* name;
	int nparams;
	int stacksize;
	int callsaveat;
	int involsaveat;
	int colornum;
	int startat;
	int endat;
	bitmap involsave;
	set<_variable> vars;
	set<_label> labels;
	vector<const char*> idx2var;
//low addr -> high addr
//prev stack frame | caller-saved register | involatile register | array | spilled register |
};

int alignup(int n, int align)
{
	return n-(n%align)+align;
}

vector<node*> cfc;
vector<_func> funclist;
set<_variable> globalvarlist;
set<const char*> checkvarname;
static const char* paranames[] = {"p0","p1","p2","p3","p4","p5","p6","p7"};
static const char* regnames[] = {
	"s0","s1","s2","s3","s4","s5","s6","s7","s8","s9","s10","s11",
	"t0","t1","t2","t3","t4",
	"a0","a1","a2","a3","a4","a5","a6","a7",
	"x0"
};//try not to use volatile register to avoid call-time saving
const int regnum = 12+5+8+1;
const int involregnum = 12;
//x0 is zero
//t5 and t6 are reserved for variables on stack and global variables
#define a0 17
#define t0 12
#define s0 0
#define sw1 "t5"
#define sw2 "t6"
static inline bool inreg(int color)
{
	return 0 <= color && color < regnum;
}
static inline bool vol(int color)
{
	return t0 <= color && color < regnum-1;
}
static inline bool invol(int color)
{
	return (0 <= color && color < involregnum) || color == regnum-1;
}
static int dyop(int a, int b, const char* op, int lineno)
{
	switch(reinterpret_cast<size_t>(op))
	{
		case ADD:return a+b;
		case SUB:return a-b;
		case MUL:return a*b;
		case DIV:if(b==0){ERR(lineno, "divided by zero!");} return a/b;
		case MOD:if(b==0){ERR(lineno, "mod by zero!");} return a%b;
		case AND:return a&&b;
		case OR:return a||b;
		case LT:return a<b;
		case GT:return a>b;
		case EQ:return a==b;
		case NE:return a!=b;
		default:return 0;
	}
}

static void analyze1()//test region and generate basic informations
{
	for(int i = 0; i < cfc.size(); i++)
	{
		node &n = *cfc[i];
		if(n.t == VAR)
		{
			_variable t;
			t.belongto = "global";
			t.initialized = false;
			t.init = 0;
			t.startat = n.lineno;
			t.color = -1;
			if(n.p.size() == 2)
			{
				t.name = n.p[1];
				t.length = atoi(n.p[0]);
			}
			else
			{
				t.name = n.p[0];
				t.length = 0;
			}
			if(checkvarname.count(t.name))
			{
				ERR(n.lineno, "variable %s has been defined previously", t.name);
			}
			globalvarlist.insert(t);
			checkvarname.insert(t.name);
		}
		else if(n.t == VEI)
		{
			_variable tmp;
			tmp.name = n.p[0];
			auto j = globalvarlist.find(tmp);
			if(j == globalvarlist.end())
			{
				ERR(n.lineno, "undefined variable %s", n.p[0]);
			}
			else
			{
				if(j->length != 0)
				{
					ERR(n.lineno, "type incompatible (assign from integer to array)");
				}
				if(j->initialized)
				{
					WAR(n.lineno, "reinitializing global variable %s", n.p[0]);
				}
				_variable& ref = const_cast<_variable&>(*j);
				ref.initialized = true;
				ref.init = (n.p[1]==x0?0:atoi(n.p[1]));
			}
		}
		else if(n.t == INFUNC)
		{
			_func f;
			f.name = n.p[0];
			f.startat = i;
			f.nparams = atoi(n.p[1]);
			if(f.nparams > 8)
				ERR(n.lineno, "function with more than 8 parameters is not supported");
			for(auto &v : globalvarlist)
			{
				_variable tmp = v;
				tmp.index = f.vars.size();
				f.vars.insert(tmp);
				f.idx2var.push_back(tmp.name);
			}
			map<const char*, int, less<const char*> > constant;
			constant.clear();
			do
			{
				cfc[i+1]->dead = cfc[i]->dead;
				if(cfc[i]->t == VAR)
				{
					_variable tmp;
					tmp.name = cfc[i]->p.back();
					tmp.belongto = f.name;
					tmp.length = (cfc[i]->p.size() == 2? atoi(cfc[i]->p.front()): 0);
					tmp.startat = cfc[i]->lineno;
					tmp.color = -1;
					if(checkvarname.count(tmp.name))
					{
						ERR(cfc[i]->lineno, "variable %s has been defined previously", tmp.name);
					}
					checkvarname.insert(tmp.name);
					tmp.index = f.vars.size();
					f.vars.insert(tmp);
					f.idx2var.push_back(tmp.name);
				}
				else if(cfc[i]->t == LABEL)
				{
					_label l;
					l.name = atoi(cfc[i]->p[0]+1);
					l.belongto = f.name;
					l.target = i;
					if(f.labels.count(l))
					{
						ERR(cfc[i]->lineno, "label l%d has been defined previously", l.name);
					}
					f.labels.insert(l);
					cfc[i]->dead = false;
					cfc[i+1]->dead = false;
					constant.clear();
				}
				else if(cfc[i]->t == VEI)
				{
					constant[cfc[i]->p.front()] = atoi(cfc[i]->p.back());
				}
				else if(cfc[i]->t == VEV)
				{
					auto res = constant.find(cfc[i]->p.back());
					if(res != constant.end())
					{
						constant[cfc[i]->p[0]] = res->second;
						cfc[i]->t = VEI;
						char *buf = new char[32];
						snprintf(buf, 32, "%d", res->second);
						cfc[i]->p[1] = buf;
					}
					else
					{
						constant.erase(cfc[i]->p[0]);
					}
				}
				else if(cfc[i]->t == DYOP)
				{
					auto res = constant.find(cfc[i]->p[3]);
					if(res != constant.end())
					{
						int c2 = res->second;
						res = constant.find(cfc[i]->p[1]);
						if(res != constant.end())
						{
							int c1 = res->second;
							int c = dyop(c1, c2, cfc[i]->p[2], cfc[i]->lineno);
							constant[cfc[i]->p[0]] = c;
							cfc[i]->t = VEI;
							char *buf = new char[32];
							snprintf(buf, 32, "%d", c);
							cfc[i]->p.assign({cfc[i]->p.front(), buf});
						}
						else
						{
							constant.erase(cfc[i]->p[0]);
							if(cfc[i]->p[2] == (char*)ADD || cfc[i]->p[2] == (char*)LT)
							{
								cfc[i]->t = DYOPI;
								cfc[i]->p[3] = (char*)c2;
							}
							else if(cfc[i]->p[2] == (char*)SUB)
							{
								cfc[i]->t = DYOPI;
								cfc[i]->p[2] = (char*)ADD;
								cfc[i]->p[3] = (char*)(-c2);
							}
							if(cfc[i]->p[2] == (char*)ADD && cfc[i]->p[3] == (char*)0)
							{
								cfc[i]->t = NOP;
							}
						}
					}
					else
					{
						constant.erase(cfc[i]->p[0]);
						res = constant.find(cfc[i]->p[1]);
						if(res != constant.end())
						{
							int c1 = res->second;
							if(cfc[i]->p[2] == (char*)ADD || cfc[i]->p[2] == (char*)GT)
							{
								cfc[i]->t = DYOPI;
								if(cfc[i]->p[2] == (char*)GT) cfc[i]->p[2] = (char*)LT;
								cfc[i]->p[1] = cfc[i]->p[3];
								cfc[i]->p[3] = (char*)c1;
							}
						}
					}
				}
				else if(cfc[i]->t == UNOP)
				{
					auto res = constant.find(cfc[i]->p[2]);
					if(res != constant.end())
					{
						int c = res->second;
						if(cfc[i]->p[1] == (char*)NOT) c = !c;
						else if(cfc[i]->p[1] == (char*)SUB) c = -c;
						cfc[i]->t = VEI;
						char *buf = new char[32];
						snprintf(buf, 32, "%d", c);
						cfc[i]->p.assign({cfc[i]->p[0], buf});
						constant[cfc[i]->p[0]] = c;
					}
					else
					{
						constant.erase(cfc[i]->p.front());
					}
				}
				else if(cfc[i]->t == IF)
				{
					auto res = constant.find(cfc[i]->p[2]);
					if(res != constant.end())
					{
						int c2 = res->second;
						res = constant.find(cfc[i]->p[0]);
						if(res != constant.end())
						{
							int c1 = res->second;
							int c = dyop(c1, c2, cfc[i]->p[1], cfc[i]->lineno);
							if(c)
							{
								cfc[i+1]->dead = true;
								cfc[i]->t = GOTO;
								cfc[i]->p.assign({cfc[i]->p.back()});
							}
						}
					}
				}
				else if(cfc[i]->t == GOTO)
				{
					cfc[i+1]->dead = true;
					constant.clear();
				}
				else if(cfc[i]->t == RETURN)
				{
					cfc[i+1]->dead = true;
				}
				else if(cfc[i]->t == CALL)
				{
					constant.erase(cfc[i]->p[0]);
					for(auto& j : globalvarlist)
					{
						constant.erase(j.name);
					}
				}
				else if(cfc[i]->t == VEA)
				{
					constant.erase(cfc[i]->p[0]);
				}
				i++;
			}while(i<cfc.size() && cfc[i]->t != OUTFUNC);
			cfc[i]->dead = false;
			if(i >= cfc.size())
			{
				ERR(n.lineno, "function %s without ending", n.p[0]);
			}
			if(strcmp(n.p[0], cfc[i]->p[2]))
			{
				ERR(cfc[i]->lineno, "function %s with incompatible ending", n.p[0]);
			}
			f.endat = i;
			for(int j = 0; j < f.nparams; j++)
			{
				_variable tmp;
				tmp.name = paranames[j];
				tmp.startat = f.startat;
				tmp.length = 0;
				tmp.belongto = f.name;
				tmp.index = f.vars.size();
				tmp.color = a0+j;
				f.vars.insert(tmp);
				f.idx2var.push_back(tmp.name);
			}
			{
				_variable tmp;
				tmp.name = x0;
				tmp.color = regnum-1;
				tmp.index = f.vars.size();
				tmp.length = 0;
				f.vars.insert(tmp);
				f.idx2var.push_back(tmp.name);
				tmp.name = imm;
				tmp.color = -1;
				tmp.index = f.vars.size();
				tmp.length = 0;
				f.vars.insert(tmp);
				f.idx2var.push_back(tmp.name);
			}
			funclist.push_back(f);
		}
	}
}

static void analyze2()//liveness
{
	for(auto& func : funclist)
	{
		bool change;
		cfc[func.startat]->live = bitmap(func.vars.size());
		do
		{
			change = false;
			cfc[func.endat]->live = bitmap(func.vars.size());
			for(int i = func.endat - 1; i >= func.startat; i--)//generate liveness
			{
				node &n = *cfc[i];
				bitmap newlive(cfc[i+1]->live);
				_variable var;
				_label label;
				if(!cfc[i]->dead)
				{
				switch(n.t)
				{
					case GOTO:
					{
						label.name = atoi(n.p[0]+1);
						auto res = func.labels.find(label);
						if(res == func.labels.end())
							ERR(n.lineno, "undefined label %s", n.p[0]);
						newlive.reset();
						newlive |= cfc[res->target]->live;
						break;
					}
					case IF:
					{
						label.name = atoi(n.p[3]+1);
						auto res = func.labels.find(label);
						if(res == func.labels.end())
							ERR(n.lineno, "undefined label %s", n.p[3]);
						newlive |= cfc[res->target]->live;
						var.name = n.p[0];
						auto res2 = func.vars.find(var);
						if(res2 == func.vars.end())
							ERR(n.lineno, "unknown variable %s", var.name);
						newlive.set(res2->index);
						var.name = n.p[2];
						res2 = func.vars.find(var);
						if(res2 == func.vars.end())
							ERR(n.lineno, "unknown variable %s", var.name);
						newlive.set(res2->index);
						break;
					}
					case VEV:
					case UNOP:
					{
						var.name = n.p.front();
						auto res = func.vars.find(var);
						if(res == func.vars.end())
							ERR(n.lineno, "unknown variable %s", var.name);
						newlive.clear(res->index);
						var.name = n.p.back();
						res = func.vars.find(var);
						if(res == func.vars.end())
							ERR(n.lineno, "unknown variable %s", var.name);
						newlive.set(res->index);
						break;
					}
					case VEI:
					{
						var.name = n.p.front();
						auto res = func.vars.find(var);
						if(res == func.vars.end())
							ERR(n.lineno, "unknown variable %s", var.name);
						newlive.clear(res->index);
						break;
					}
					case AEV:
					{
						var.name = n.p[0];
						auto res = func.vars.find(var);
						if(res == func.vars.end())
							ERR(n.lineno, "unknown variable %s", var.name);
						newlive.set(res->index);
						var.name = n.p[1];
						res = func.vars.find(var);
						if(res == func.vars.end())
							ERR(n.lineno, "unknown variable %s", var.name);
						newlive.set(res->index);
						var.name = n.p[3];
						res = func.vars.find(var);
						if(res == func.vars.end())
							ERR(n.lineno, "unknown variable %s", var.name);
						newlive.set(res->index);
						break;
					}
					case VEA:
					{
						var.name = n.p[0];
						auto res = func.vars.find(var);
						if(res == func.vars.end())
							ERR(n.lineno, "unknown variable %s", var.name);
						newlive.clear(res->index);
						var.name = n.p[1];
						res = func.vars.find(var);
						if(res == func.vars.end())
							ERR(n.lineno, "unknown variable %s", var.name);
						newlive.set(res->index);
						var.name = n.p[2];
						res = func.vars.find(var);
						if(res == func.vars.end())
							ERR(n.lineno, "unknown variable %s", var.name);
						newlive.set(res->index);
						break;
					}
					case DYOP:
					{
						var.name = n.p[0];
						auto res = func.vars.find(var);
						if(res == func.vars.end())
							ERR(n.lineno, "unknown variable %s", var.name);
						newlive.clear(res->index);
						var.name = n.p[1];
						res = func.vars.find(var);
						if(res == func.vars.end())
							ERR(n.lineno, "unknown variable %s", var.name);
						newlive.set(res->index);
						var.name = n.p.back();
						res = func.vars.find(var);
						if(res == func.vars.end())
							ERR(n.lineno, "unknown variable %s", var.name);
						newlive.set(res->index);
						break;
					}
					case DYOPI:
					{
						var.name = n.p[0];
						auto res = func.vars.find(var);
						if(res == func.vars.end())
							ERR(n.lineno, "unknown variable %s", var.name);
						newlive.clear(res->index);
						var.name = n.p[1];
						res = func.vars.find(var);
						if(res == func.vars.end())
							ERR(n.lineno, "unknown variable %s", var.name);
						newlive.set(res->index);
						break;
					}
					case VAR:
					{
						var.name = n.p.back();
						auto res = func.vars.find(var);
						assert(res != func.vars.end());
						newlive.clear(res->index);
						break;
					}
					case RETURN:
					{
						newlive.reset();
					}
					case PARAM:
					{
						var.name = n.p.front();
						auto res = func.vars.find(var);
						if(res == func.vars.end())
							ERR(n.lineno, "unknown variable %s", var.name);
						newlive.set(res->index);
						break;
					}
					case CALL:
					{
						var.name = n.p.front();
						auto res = func.vars.find(var);
						if(res == func.vars.end())
							ERR(n.lineno, "unknown variable %s", var.name);
						newlive.clear(res->index);
						break;
					}
					default:break;
				}
				}
				if(!(newlive == n.live))
				{
					n.live = newlive;
					change = true;
				}
			}//end for
		}while(change);

		for(int i = func.endat - 1; i > func.startat; i--)
		{
			cfc[func.startat]->live |= cfc[i]->live;
		}
	}
}

static void analyze3()//color
{
	vector<set<int> > adjlist;
	vector<int> wt;
	vector<int> SEO;//simplicial elimination ordering
	for(auto& f : funclist)
	{
		adjlist.clear();
		adjlist.resize(f.vars.size());
		SEO.clear();
		wt.clear();
		wt.resize(f.vars.size(), 0);
		for(int i = f.startat + 1; i < f.endat; i++)
		{
			for(int j = 0; j < f.vars.size(); j++)
			{
				if(cfc[i]->live.test(j)!=1) continue;
				for(int k = j+1; k < f.vars.size(); k++)
				{
					if(cfc[i]->live.test(k)!=1) continue;
					adjlist[j].insert(k);
					adjlist[k].insert(j);
				}
			}
		}//generate adjacency list
		for(int i = 0; i < f.vars.size(); i++)
		{
			int maxidx = 0;
			for(int j = 0; j < wt.size(); j++)
			{
				if(wt[j] < 0) continue;
				if(wt[j] > wt[maxidx]) maxidx = j;
			}
			SEO.push_back(maxidx);//simplicial elimination ordering
			wt[maxidx] = -1;
			for(auto j : adjlist[maxidx])
			{
				if(wt[j] < 0) continue;
				wt[j]++;
			}
		}
		bitmap c(SEO.size()+regnum);
		f.colornum = 0;
		for(auto i : SEO)//greedy coloring
		{
			c.reset();
			c.set(regnum-1);
			_variable v;
			decltype(f.vars.find(v)) res;
			v.name = f.idx2var[i];
			res = f.vars.find(v);
			assert(res != f.vars.end());
			_variable& ref = const_cast<_variable&>(*res);
			if(ref.color >= 0) continue;//has been colored previously
			if(!strcmp(ref.belongto, "global"))
			{
				ref.color = -1;
				continue;
			}//global variable need not to be colored
			if(ref.length > 0)
			{
				ref.color = -1;
				continue;
			}//array will always be on stack, and will be colored later
			for(auto j : adjlist[i])
			{
				v.name = f.idx2var[j];
				res = f.vars.find(v);
				assert(res != f.vars.end());
				if(res->color >= 0)
					c.set(res->color);
			}
			ref.color = c.find();
			f.colornum = max(f.colornum, ref.color+1);
		}
		f.stacksize = max(0, f.colornum-regnum);
	}
}

void analyze4()//open stack for each function
{
	int callsavesize = 0, involsavesize = 0;
	for(auto& f : funclist)
	{
		callsavesize = involsavesize = 0;
		int cnt;
		bitmap use_s(involregnum);
		for(const auto& v : f.vars)
		{
			node &n = *cfc[f.startat];
			if(n.live.test(v.index) == 1)
			{
				if(invol(v.color))
					use_s.set(v.color - s0);
			}
			if(v.length > 0 && strcmp(v.belongto, "global"))//local array
			{
				_variable& ref = const_cast<_variable&>(v);
				ref.color = f.stacksize + regnum;
				f.stacksize += v.length/4;
			}
		}
		for(int i = f.startat + 1; i < f.endat; i++)
		{
			if(cfc[i]->t == CALL)
			{
				cnt = 0;
				node &n = *cfc[i];
				bitmap tmpuse(involregnum);
				int tmp;
				for(const auto& v : f.vars)
				{
					if(n.live.test(v.index) == 1)
					{
						if(invol(v.color))
							tmpuse.set(v.color - s0);
					}
				}
				for(const auto& v : f.vars)
				{
					if(n.live.test(v.index) == 1)
					{
						if(vol(v.color))
						{
							tmp = tmpuse.find();
							if(tmp < 0) cnt++;
							else tmpuse.set(tmp);
						}
					}
				}
				callsavesize = max(callsavesize, cnt);
				use_s |= tmpuse;
			}
		}
		for(int i = 0; i < involregnum; i++)
		{
			if(use_s.test(i) == 1) involsavesize++;
		}
		f.involsaveat = f.stacksize;
		f.callsaveat = f.stacksize + involsavesize;
		f.stacksize += involsavesize + callsavesize;
		f.involsave = use_s;
	}
}



static void savess(_func& f)
{
	int ptr = 0;
	for(int i = 0; i < involregnum; i++)
	{
		if(f.involsave.test(i) == 1)
		{
			printf("store %s %d\n", regnames[i+s0], f.involsaveat + ptr);
			ptr++;
		}
	}
}
static void loadss(_func& f)
{
	int ptr = 0;
	for(int i = 0; i < involregnum; i++)
	{
		if(f.involsave.test(i) == 1)
		{
			printf("load %d %s\n", f.involsaveat + ptr, regnames[i+s0]);
			ptr++;
		}
	}
}
static void loadvar(const _variable& v, int reg)
{
	if(v.color < 0)
	{
		if(v.length > 0)
			printf("loadaddr v%d %s\n", v.startat, regnames[reg]);
		else
			printf("load v%d %s\n", v.startat, regnames[reg]);
	}
	else if(v.color >= regnum)
	{
		if(v.length > 0)
			printf("loadaddr %d %s\n", v.color-regnum, regnames[reg]);
		else
			printf("load %d %s\n", v.color-regnum, regnames[reg]);
	}
	else
		printf("%s = %s\n", regnames[reg], regnames[v.color]);
}
static void loadsw(const _variable& v, const char* sw)
{
	if(v.color < 0)
	{
		if(v.length > 0)
			printf("loadaddr v%d %s\n", v.startat, sw);
		else
			printf("load v%d %s\n", v.startat, sw);
	}
	else if(v.color >= regnum)
	{
		if(v.length > 0)
			printf("loadaddr %d %s\n", v.color-regnum, sw);
		else
			printf("load %d %s\n", v.color-regnum, sw);
	}
	else
		printf("%s = %s\n", sw, regnames[v.color]);
}
static void storesw(const char* sw, const _variable& v, int s)
{
	if(v.color < 0)
		printf("loadaddr v%d %s\n%s [0, %d] = %s\n", v.startat, (sw==sw1?sw2:sw1), (sw==sw1?sw2:sw1), s, sw);
	else if(v.color >= regnum)
		printf("store %s %d\n", sw, v.color-regnum);
	else
		printf("%s = %s\n", regnames[v.color], sw);
}
static const char* ops[] = {
	"+","-","*","/","%","&&","||","!","<",">","==","!="
};
static int t2c(const char* t)
{
	int token = static_cast<int>(reinterpret_cast<size_t>(t));
	switch(token)
	{
		case ADD: return 0;
		case SUB: return 1;
		case MUL: return 2;
		case DIV: return 3;
		case MOD: return 4;
		case AND: return 5;
		case OR : return 6;
		case NOT: return 7;
		case LT : return 8;
		case GT : return 9;
		case EQ : return 10;
		case NE : return 11;
		default : return -1;
	}
}

static void analyze5()//gen code
{
	printf("v0 = malloc %d\n", long_size*(regnum-1-a0));//used for save parameters
	for(auto& v : globalvarlist)
	{
		if(v.length > 0)
		{
			if(v.length % 4) ERR(v.startat, "array %s with length not dividable by 4", v.name);
			printf("v%d = malloc %d\n", v.startat, v.length);
		}
		else
		{
			if(!v.initialized) WAR(v.startat, "uninitialized global variable %s", v.name);
			printf("v%d = %d\n", v.startat, v.init);
		}
	}

	for(auto& f : funclist)
	{
		printf("%s [%d] [%d]\n", f.name, f.nparams, f.stacksize);
		savess(f);
		vector<const char*> para;
		para.clear();
		_variable var;
		_label label;
		decltype(f.vars.find(var)) res;
		for(int i = f.startat + 1; i < f.endat; i++)
		{
			node &n = *cfc[i];

			if(cfc[i]->dead) continue;

			switch(n.t)
			{
				case GOTO:
				{
					printf("goto %s\n", n.p[0]);
					break;
				}
				case IF:
				{
					const char *reg1, *reg2;
					var.name = n.p[0];
					res = f.vars.find(var);
					if(inreg(res->color))
						reg1 = regnames[res->color];
					else
					{
						reg1 = sw1;
						loadsw(*res, sw1);
					}
					var.name = n.p[2];
					res = f.vars.find(var);
					if(inreg(res->color))
						reg2 = regnames[res->color];
					else
					{
						reg2 = sw2;
						loadsw(*res, sw2);
					}
					printf("if %s %s %s goto %s\n", reg1, ops[t2c(n.p[1])], reg2, n.p[3]);
					break;
				}
				case UNOP:
				{
					const char *reg1, *result;
					var.name = n.p[2];
					res = f.vars.find(var);
					if(!inreg(res->color))
					{
						loadsw(*res, sw1);
						reg1 = sw1;
					}
					else reg1 = regnames[res->color];

					var.name = n.p[0];
					res = f.vars.find(var);
					if(!inreg(res->color))
						result = sw1;
					else result = regnames[res->color];

					printf("%s = %s%s\n", result, ops[t2c(n.p[1])], reg1);
					if(res->color >= regnum)
					{
						if(res->length > 0)
							ERR(n.lineno, "trying to assign a int/array to array");
						printf("store %s %d\n", result, res->color-regnum);
					}
					else if(res->color < 0)
					{
						if(res->length > 0)
							ERR(n.lineno, "trying to assign a int/array to array");
						printf("loadaddr v%d %s\n", res->startat, sw2);
						printf("%s [0, %d] = %s\n", sw2, long_size, result);
					}
					break;
				}
				case DYOP:
				{
					const char *reg1, *reg2, *result;
					var.name = n.p[1];
					res = f.vars.find(var);
					if(!inreg(res->color))
					{
						loadsw(*res, sw1);
						reg1 = sw1;
					}
					else reg1 = regnames[res->color];

					var.name = n.p[3];
					res = f.vars.find(var);
					if(!inreg(res->color))
					{
						loadsw(*res, sw2);
						reg2 = sw2;
					}
					else reg2 = regnames[res->color];

					var.name = n.p[0];
					res = f.vars.find(var);
					if(!inreg(res->color))
					{
						result = sw1;
					}
					else result = regnames[res->color];
					printf("%s = %s %s %s\n", result, reg1, ops[t2c(n.p[2])], reg2);
					if(res->color >= regnum)
					{
						if(res->length > 0)
							ERR(n.lineno, "trying to assign a int/array to array");
						printf("store %s %d\n", result, res->color-regnum);
					}
					else if(res->color < 0)
					{
						if(res->length > 0)
							ERR(n.lineno, "trying to assign a int/array to array");
						printf("loadaddr v%d %s\n", res->startat, sw2);
						printf("%s [0, %d] = %s\n", sw2, long_size, result);
					}
					break;
				}
				case DYOPI:
				{
					const char *reg1, *result;
					var.name = n.p[1];
					res = f.vars.find(var);
					if(!inreg(res->color))
					{
						loadsw(*res, sw1);
						reg1 = sw1;
					}
					else reg1 = regnames[res->color];

					var.name = n.p[0];
					res = f.vars.find(var);
					if(!inreg(res->color))
					{
						result = sw1;
					}
					else result = regnames[res->color];
					printf("%s = %s %s %d\n", result, reg1, ops[t2c(n.p[2])], (int)reinterpret_cast<size_t>(n.p[3]));
					if(res->color >= regnum)
					{
						if(res->length > 0)
							ERR(n.lineno, "trying to assign a int/array to array");
						printf("store %s %d\n", result, res->color-regnum);
					}
					else if(res->color < 0)
					{
						if(res->length > 0)
							ERR(n.lineno, "trying to assign a int/array to array");
						printf("loadaddr v%d %s\n", res->startat, sw2);
						printf("%s [0, %d] = %s\n", sw2, long_size, result);
					}
					break;
				}
				case AEV:
				{
					const char *reg2;
					var.name = n.p[0];
					res = f.vars.find(var);
					loadsw(*res, sw1);
					var.name = n.p[1];
					res = f.vars.find(var);
					if(!inreg(res->color))
					{
						loadsw(*res, sw2);
						reg2 = sw2;
					}
					else reg2 = regnames[res->color];
					printf("%s = %s + %s\n", sw1, sw1, reg2);
					var.name = n.p[3];
					res = f.vars.find(var);
					if(inreg(res->color))
						printf("%s [0, %s] = %s\n", sw1, n.p[2], regnames[res->color]);
					else
					{
						loadsw(*res, sw2);
						printf("%s [0, %s] = %s\n", sw1, n.p[2], sw2);
					}
					break;
				}
				case VEA:
				{
					const char *reg2;
					var.name = n.p[1];
					res = f.vars.find(var);
					loadsw(*res, sw1);
					var.name = n.p[2];
					res = f.vars.find(var);
					if(!inreg(res->color))
					{
						loadsw(*res, sw2);
						reg2 = sw2;
					}
					else reg2 = regnames[res->color];
					printf("%s = %s + %s\n", sw1, sw1, reg2);
					var.name = n.p[0];
					res = f.vars.find(var);
					if(inreg(res->color))
						printf("%s = %s [0, %s]\n", regnames[res->color], sw1, n.p[3]);
					else if(res->color >= regnum)
					{
						if(res->length > 0)
							ERR(n.lineno, "trying to assign a int/array to array");
						printf("%s = %s [0, %s]\n", sw1, sw1, n.p[3]);
						printf("store %s %d\n", sw1, res->color-regnum);
					}
					else
					{
						if(res->length > 0)
							ERR(n.lineno, "trying to assign a int/array to array");
						printf("loadaddr v%d %s\n", res->startat, sw2);
						printf("%s = %s [0, %s]\n", sw1, sw1, n.p[3]);
						printf("%s [0, %s] = %s\n", sw2, n.p[3], sw1);
					}
					break;
				}
				case VEV:
				{
					const char *reg1;
					var.name = n.p[0];
					res = f.vars.find(var);
					var.name = n.p[1];
					auto res2 = f.vars.find(var);
					if(res->color == res2->color && res->color >= 0) break;
					if(res->color < 0 && !strcmp(res->name, res2->name)) break;
					if(cfc[i+1]->live.test(res->index) != 1) break;
					if(!inreg(res2->color))
					{
						loadsw(*res2, sw1);
						reg1 = sw1;
					}
					else reg1 = regnames[res2->color];
					if(inreg(res->color))
						printf("%s = %s\n", regnames[res->color], reg1);
					else if(res->color >= regnum)
					{
						if(res->length > 0)
							ERR(n.lineno, "trying to assign a int/array to array");
						printf("store %s %d\n", reg1, res->color-regnum);
					}
					else if(res->color < 0)
					{
						if(res->length > 0)
							ERR(n.lineno, "trying to assign a int/array to array");
						printf("loadaddr v%d %s\n", res->startat, sw2);
						printf("%s [0, %d] = %s\n", sw2, long_size, reg1);
					}
					break;
				}
				case VEI:
				{
					var.name = n.p[0];
					res = f.vars.find(var);
					if(atoi(n.p[1]) == 0) n.p[1] = x0;
					if(cfc[i+1]->live.test(res->index) != 1 && strcmp(res->belongto, "global")) break;
					if(inreg(res->color))
						printf("%s = %s\n", regnames[res->color], n.p[1]);
					else if(res->color >= regnum)
					{
						if(res->length > 0)
							ERR(n.lineno, "trying to assign a int/array to array");
						printf("%s = %s\n", sw1, n.p[1]);
						printf("store %s %d\n", sw1, res->color-regnum);
					}
					else
					{
						if(res->length > 0)
							ERR(n.lineno, "trying to assign a int/array to array");
						printf("%s = %s\n", sw1, n.p[1]);
						printf("loadaddr v%d %s\n", res->startat, sw2);
						printf("%s [0, %d] = %s\n", sw2, long_size, sw1);
					}
					break;
				}
				case RETURN:
				{
					var.name = n.p[0];
					res = f.vars.find(var);
					loadvar(*res, a0);
					loadss(f);
					puts("return");
					break;
				}
				case PARAM:
				{
					var.name = n.p[0];
					res = f.vars.find(var);
					if(cfc[i-1]->t != PARAM)
						printf("loadaddr v0 %s\n", sw1);
					if(inreg(res->color))
						printf("%s [%ld, %d] = %s\n",
								sw1, long_size*para.size(),
								long_size, regnames[res->color]);
					else if(res->color >= regnum)
					{
						if(res->length > 0)
							printf("loadaddr %d %s\n", res->color-regnum, sw2);
						else
							printf("load %d %s\n", res->color-regnum, sw2);
						printf("%s [%ld, %d] = %s\n",
							sw1, long_size*para.size(),
							long_size, sw2);
					}
					else
					{
						if(res->length > 0)
							printf("loadaddr v%d %s\n", res->startat, sw2);
						else
							printf("load v%d %s\n", res->startat, sw2);
						printf("%s [%ld, %d] = %s\n",
							sw1, long_size*para.size(),
							long_size, sw2);
					}
					para.push_back(n.p[0]);
					break;
				}
				case CALL:
				{
					//TODO: save volatile registers
					bitmap buf(involregnum);
					for(const auto& v : f.vars)
					{
						if(n.live.test(v.index) == 1)
							if(invol(v.color))
								buf.set(v.color-s0);
					}
					vector<int> tmpcolor;
					for(const auto& v : f.vars)
					{
						if(n.live.test(v.index) == 1)
						{
							if(vol(v.color))
							{
								int tmp = buf.find();
								if(tmp >= 0)
								{
									buf.set(tmp);
									tmp += s0;
									tmpcolor.push_back(tmp);
									printf("%s = %s\n", regnames[tmp], regnames[v.color]);
								}
								else
								{
									tmp = max(tmpcolor.back()+1, f.callsaveat+regnum);
									tmpcolor.push_back(tmp);
									printf("store %s %d\n", regnames[v.color], tmp-regnum);
								}
							}
						}
					}
					//TODO: send params
					if(para.size()>0 && cfc[i-1]->t != PARAM) printf("loadaddr v0 %s\n", sw1);
					for(int j = 0; j < para.size(); j++)
					{
						printf("%s = %s [%d, %d]\n", regnames[a0+j], sw1, long_size*j, long_size);
					}
					printf("call %s\n", n.p[1]);
					var.name = n.p[0];
					res = f.vars.find(var);
					if(res->length > 0)
						ERR(n.lineno, "trying to assign a int/array to array");
					if(cfc[i+1]->live.test(res->index) == 1)
						printf("%s = %s\n", sw1, regnames[a0]);
					//TODO: restore volatile registers
					int __=0;
					for(const auto& v : f.vars)
					{
						if(n.live.test(v.index) == 1)
							if(vol(v.color))
							{
								int tmp = tmpcolor[__++];
								if(tmp < regnum)
								{
									printf("%s = %s\n", regnames[v.color], regnames[tmp]);
								}
								else
								{
									printf("load %d %s\n", tmp-regnum, regnames[v.color]);
								}
							}
					}
					if(cfc[i+1]->live.test(res->index) == 1)
						storesw(sw1, *res, long_size);
					para.clear();
					para.resize(0);
					break;
				}
				case LABEL:
				{
					printf("%s :\n", n.p[0]);
					break;
				}
				case VAR:
				default:break;
			}
		}
		printf("end %s\n", f.name);
	}
}


void analyze()
{
	analyze1();
	analyze2();
	analyze3();
	analyze4();
	analyze5();
/*
	for(auto& i : funclist)
	{
		printf("//%s\n//\t%d\n//\t%d\n//\t%d\n//\t%d\n",i.name,i.nparams,i.stacksize,i.startat,i.endat);
		putchar('\n');
		for(auto& j : i.vars)
		{
			printf("//\t%s : %d %d %d\n", j.name, j.index, cfc[i.startat]->live.test(j.index), j.color);
		}
	}
	for(auto& i : globalvarlist)
	{
		printf("//%s\n//\t%s\n//\t%d\n//\t%d\n//\t%d\n",i.name,i.belongto,i.length,i.init,i.startat);
	}
*/
}
