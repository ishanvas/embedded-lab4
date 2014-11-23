@ write.S: write sycall wrapper
@
@ Authors: Ishan Vashishtha <ivashish@andrew.cmu.edu>
@ Date:    5th October 2014

#include <asm.h>
#include <bits/swi.h>
#include <bits/errno.h>

	.file	"write.S"
	.text
	.global	write
write:
	swi WRITE_SWI
	cmp  r0, #0
        bpl  L2  
        mov r2, #-1     
        mul r3, r0, r2
        ldr r1, code
        stm r1, {r3}
	mov r0, #-1
L2:
	mov pc,lr
code:
	.word errno
