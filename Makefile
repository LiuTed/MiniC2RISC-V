all:
	cd MiniC2Eeyore; make all
	cd Eeyore2Tigger; make all
	cd Tigger2RISCV; make all
	g++ minicc.cpp -o minicc -O2 -w

.PHONY: clean
clean:
	cd MiniC2Eeyore; make clean
	cd Eeyore2Tigger; make clean
	cd Tigger2RISCV; make clean
	rm -f minicc

handin:
	tar -cf 1500012923.tar MiniC2Eeyore Eeyore2Tigger Tigger2RISCV minicc.cpp Makefile
