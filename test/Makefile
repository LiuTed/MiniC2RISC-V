FILES = bubble manyreg test
CC = /opt/riscv/bin/riscv64-unknown-linux-gnu-gcc
AR = /opt/riscv/bin/riscv64-unknown-linux-gnu-ar
MINICC = ../minicc
LIB = minic

all: libminic.a $(FILES)

$(FILES): %: %.s
	$(CC) $< -o $@ -static -l$(LIB) -L.

bubble.s: bubble.c
	$(MINICC) bubble.c -o bubble.t -t
	$(MINICC) bubble.t -o bubble.s

test.s: test.c
	$(MINICC) test.c -o test.t -t
	$(MINICC) test.t -o test.s

manyreg.s: manyreg.c
	$(MINICC) manyreg.c -o manyreg.t -t
	$(MINICC) manyreg.t -o manyreg.s

libminic.a: ../Tigger2RISCV/basic_func.c
	$(CC) ../Tigger2RISCV/basic_func.c -c -o lib.o
	$(AR) -q libminic.a lib.o
	rm lib.o

.PHONY: clean
clean:
	rm -f *.s *.t $(FILES) libminic.a
