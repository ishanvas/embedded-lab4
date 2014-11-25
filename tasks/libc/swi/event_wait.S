/** @file event_wait.S
 *
 * @brief time sycall wrapper
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-10-31
 */

#include <asm.h>
#include <bits/swi.h>
#include <bits/errno.h>

	.file "event_wait.S"

FUNC(event_wait)
	swi EVENT_WAIT
	cmp r0, #0
	bpl L2
	mov r2, #-1
	mul r3, r0,r2
	ldr r1, code
	stm r1, {r3}
	mov r0, #-1
L2:
	mov  pc, lr
code:
	.word errno
