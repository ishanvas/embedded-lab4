@ New_S_handler.S: Kernel startup routine
@
@ Author: Ishan Vashishtha <ivashish@andrew.cmu.edu>
@ Date:   Mon, 20 Oct 2014

	.file	"new_swi.S"
	.text
	.global	New_S_Handler
	@New Swi handler

New_S_Handler:	SUB sp, sp, #4  @ Creating space for spsr
	STMFD sp!, {r0-r12, lr} 	@ saving unbanked registers

	MRS r2, spsr 				@ moving spsr to r2
	STR r2, [sp, #14*4]			@ saving spsr in the space created
	MOV r1, sp 					@ moving stack pointer to r1
	LDR r0, [lr, #-4]			@ gettig SWI instruction
	BIC r0,r0,#0xff000000		@Getting the swi number from instrustion

	STMFD sp, {sp, lr}^			@ Saving user mode r0, sp and lr
	add   sp, sp, #-8			@ Moving sp below

	MRS r4, cpsr  				@ enabling irq during SWI handling
	BIC r4, r4, #0x000000FF		
	ORR r4, r4, #0x13
	MSR cpsr,r4

	ldr r2, =global_data		@ restoring r8 of svc mode to global_data 
	ldr r8, [r2]

	BL C_SWI_Handler


	ldmfd    sp, {sp, lr}^ 		@restoring user mode sp and lr
	add      sp, sp, #8			 @Moving sp up.

	LDR r2, [sp, #14*4]
	MSR spsr, r2
	
	
	LDMFD sp!, {r0-r12, lr}		@ Restoring all the registers

	ADD sp, sp, #4
	MOVS pc, lr

