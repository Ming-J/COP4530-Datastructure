	lw	0	2 	mcand	//do 1*5 and store in register 1
	lw 	0	3	mplier
	lw	0	4	one	
start	beq	7	3 	done
	nand	3 	4 	5
	nand 	5	5	5 	
	add	5	7	7
	beq 	5	0	shift
	add	1 	2 	1 	
shift	nand 	4	4	5	
	add 	4	4	4 	
	add 	2 	2 	2 	
	beq	0	0	start
	noop
done 	halt
	noop
one	.fill	1
mcand	.fill	5
mplier	.fill	10
