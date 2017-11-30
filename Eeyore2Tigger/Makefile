CC = g++
CFLAGS = -std=c++11 -O2
LINKAGE = -ll
BISON = yacc -d
FLEX = lex
DEP = lex.yy.o y.tab.o tools.o bitmap.o

all: a.out

a.out: $(DEP)
	$(CC) $(DEP) $(CFLAGS) $(LINKAGE) -o a.out

$(DEP): %.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

lex.yy.cpp: scan.l y.tab.cpp
	$(FLEX) scan.l
	mv lex.yy.c lex.yy.cpp

y.tab.cpp: parser.y
	$(BISON) parser.y
	mv y.tab.c y.tab.cpp

.PHONY: clean
clean:
	rm -f a.out y.tab.cpp y.tab.h lex.yy.cpp $(DEP)
