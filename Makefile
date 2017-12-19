RISCVGCC = /opt/riscv/bin/riscv64-unknown-linux-gnu-gcc
RISCVAR = /opt/riscv/bin/riscv64-unknown-linux-gnu-ar

export RISCVGCC
export RISCVAR

all:
	cd MiniC2Eeyore; make all
	cd Eeyore2Tigger; make all
	cd Tigger2RISCV; make all
	cp Tigger2RISCV/libminic.a ./libminic.a
	g++ minicc.cpp -o minicc -O2
	cd test; make all

.PHONY: clean
clean:
	cd MiniC2Eeyore; make clean
	cd Eeyore2Tigger; make clean
	cd Tigger2RISCV; make clean
	cd test; make clean
	rm -f minicc libminic.a

handin: all
	tar -cf 1500012923.tar MiniC2Eeyore Eeyore2Tigger Tigger2RISCV minicc.cpp minicc Makefile libminic.a
