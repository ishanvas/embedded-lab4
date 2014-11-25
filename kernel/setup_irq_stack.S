@ setup_irq_stack.S: Kernel startup routine
@
@ Author: Ishan Vashishtha <ivashish@andrew.cmu.edu>
@ Date:   Mon, 20 Oct 2014
#include <bits/swi.h>
	.file	"stack_irq_setup.S"
	.text
	.align 2
	.global	setup_irq_stack
	.type setup_irq_stack, %function
setup_irq_stack:	MRS r3, cpsr  @ changes mode from kernel to irq
	MOV r4, r3					  @ storing cpsr content to restore once done
	BIC r3, r3, #0xF000000F
	ORR r3, r3, #0xc2
	MSR cpsr,r3

@ setting the stack pointer of IRQ mode
	MOV sp, #0xa2000000
	
@ moving back to kernel mode
	MSR cpsr,r4				   

@ moving to user mode
	mov pc, lr				   
