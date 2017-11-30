all:
	cd MiniC2Eeyore; make all
	cd Eeyore2Tigger; make all
	gcc minicc.c -o minicc -O2

.PHONY: clean
clean:
	rm -f minicc m2e e2t
