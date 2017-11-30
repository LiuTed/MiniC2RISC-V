CC = g++
CFLAGS = -std=c++11 -O2
LINKAGE = -ll
BISON = yacc -d
FLEX = lex

all: a.out

a.out: y.tab.cpp lex.yy.cpp tools.cpp
	$(CC) lex.yy.cpp y.tab.cpp tools.cpp $(CFLAGS) $(LINKAGE)

lex.yy.cpp: scan.l y.tab.h
	$(FLEX) scan.l
	mv lex.yy.c lex.yy.cpp

y.tab.cpp y.tab.h: parser.y
	$(BISON) parser.y
	mv y.tab.c y.tab.cpp

clean:
	rm -f a.out lex.yy.o y.tab.o y.tab.cpp y.tab.h lex.yy.cpp 1500012923.tar

handin: all
	rm -f 1500012923.tar
	tar -cf 1500012923.tar a.out lex.yy.cpp y.tab.cpp tools.cpp scan.l y.tab.h parser.y Makefile
