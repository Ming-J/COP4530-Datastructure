	lw	0	2	mcand	//See the comment on assembler.c
	lw	0	3	mplier
	lw	0	4	one
start	beq	7	3	done
	nand	3	4	5
	nand	5	5	5
	add	5	7	7
	beq	5	0	shift
	add	1	2	1
shift	add	2	2	2
	nand	4	4	5
	add	4	4	4
	beq	0	0	start
done	halt
one	.fill	1
mcand	.fill	32766
mplier	.fill	10383
