CC = g++
CFLAGS = -std=c++11 -O2
LINKAGE = -ll
BISON = bison -d
FLEX = lex

all: a.out

a.out: parser.tab.cpp lex.yy.cpp tools.cpp
	$(CC) lex.yy.cpp parser.tab.cpp tools.cpp $(CFLAGS) $(LINKAGE)

lex.yy.cpp: scan.l y.tab.h
	$(FLEX) scan.l
	mv lex.yy.c lex.yy.cpp

parser.tab.cpp parser.tab.h: parser.y
	$(BISON) parser.y
	mv parser.tab.c parser.tab.cpp

clean:
	rm -f a.out *.o parser.tab.* lex.yy.cpp 1500012923.tar

handin: all
	rm -f 1500012923.tar
	tar -cf 1500012923.tar *
