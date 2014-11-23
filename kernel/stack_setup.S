@ stack_setup.S: Kernel startup routine
@
@ Author: Ishan Vashishtha <ivashish@andrew.cmu.edu>
@ Date:   Mon, 20 Oct 2014
#include <bits/swi.h>
	.file	"stack_setup.S"
	.text
	.align 2
	.global	setup_stack
	.type setup_stack, %function

setup_stack:	MRS r3, cpsr  @ copy content of cpsr in r3
	BIC r3, r3, #0x000000FF	  @ clears mode and IFT bits
	BIC r3, r3, #0xF0000000	  @ clears condition bits 
	ORR r3, r3, #0x50		  @ set mode as user mode
	MSR spsr,r3				  @ moving r3 contents to spsr 

						   	   
	MOV r4, #0xa3000000		  @ load 0xa3000000 in r4 and push r0 and r1 on it
	STMFD r4!, {r0,r1}		  @ sets

	STMFD sp!, {r4}		   	   @saving r4 on stack for ldm to access
	MOV r5, sp


	LDMFD r5, {sp}^ 	       @setting user mode sp
	ADD sp,sp ,#4
	STMFD sp!, {r1-r12,lr}     @saving kernel mode registers on stack
	MOV lr, #0xa0000000

	MOVS pc, lr				   @moving to user mode

	
	


