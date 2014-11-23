@ crt0.S: Execution startup routine
@
@ Authors: Ishan Vashishtha <ivashish@andrew.cmu.edu> 
@ Date:    5th October 2014

	.file	"crt0.S"
	.text

	.global	_start
_start:
	ldr r0,[sp]
	ldr r1,[sp, #4]

	bl main
	
