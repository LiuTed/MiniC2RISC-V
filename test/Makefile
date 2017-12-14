FILES = bubble manyreg test
CC = /opt/riscv/bin/riscv64-unknown-linux-gnu-gcc
MINICC = ../minicc
LIB = minic

all: $(FILES)

$(FILES): %: %.s
	$(CC) $< -o $@ -static -L.. -l$(LIB)

bubble.s: bubble.c
	$(MINICC) bubble.c -o bubble.t -t
	$(MINICC) bubble.t -o bubble.s

test.s: test.c
	$(MINICC) test.c -o test.t -t
	$(MINICC) test.t -o test.s

manyreg.s: manyreg.c
	$(MINICC) manyreg.c -o manyreg.t -t
	$(MINICC) manyreg.t -o manyreg.s

.PHONY: clean
clean:
	rm -f *.s *.t $(FILES)
