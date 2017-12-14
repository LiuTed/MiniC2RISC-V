	.file	"basic_func.c"
	.option nopic
	.section	.rodata
	.align	3
.LC0:
	.string	"%d"
	.text
	.align	1
	.globl	putint
	.type	putint, @function
putint:
	add	sp,sp,-32
	sd	ra,24(sp)
	sd	s0,16(sp)
	add	s0,sp,32
	mv	a5,a0
	sw	a5,-20(s0)
	lw	a5,-20(s0)
	mv	a1,a5
	lui	a5,%hi(.LC0)
	addi	a0,a5,%lo(.LC0)
	call	printf
	nop
	mv	a0,a5
	ld	ra,24(sp)
	ld	s0,16(sp)
	add	sp,sp,32
	jr	ra
	.size	putint, .-putint
	.align	1
	.globl	getint
	.type	getint, @function
getint:
	add	sp,sp,-32
	sd	ra,24(sp)
	sd	s0,16(sp)
	add	s0,sp,32
	add	a5,s0,-20
	mv	a1,a5
	lui	a5,%hi(.LC0)
	addi	a0,a5,%lo(.LC0)
	call	__isoc99_scanf
	lw	a5,-20(s0)
	mv	a0,a5
	ld	ra,24(sp)
	ld	s0,16(sp)
	add	sp,sp,32
	jr	ra
	.size	getint, .-getint
	.ident	"GCC: (GNU) 7.1.1 20170509"
