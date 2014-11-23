@ stack_setup.S: Kernel startup routine
@
@ Author: Ishan Vashishtha <ivashish@andrew.cmu.edu>
@ Date:   Mon, 20 Oct 2014
#include <bits/swi.h>
	.file	"exit.S"
	.text
	.align 2
	.global	exit
	.type setup_stack, %function
exit:
	ADD sp, r0, #60 		@ r0 points to address of stored regs on stack
	ldr r0,[r0]				@ return number
	LDMFD sp!,{r1-r12,lr}	@ restoring kernel state
	
	MOV pc, lr              @ moving back to kernel (no return)

	
	


