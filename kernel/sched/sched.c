/** @file sched.c
 * 
 * @brief Top level implementation of the scheduler.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-20
 */

#include <types.h>
#include <assert.h>

#include <kernel.h>
#include <config.h>
#include "sched_i.h"

#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/physmem.h>

tcb_t system_tcb[OS_MAX_TASKS]; /*allocate memory for system TCBs */

void sched_init(task_t* main_task  __attribute__((unused)))
{

}

/**
 * @brief This is the idle task that the system runs when no other task is runnable
 */
 
static void idle(void)
{
	 //enable_interrupts();
	uint32_t cpsr;
	asm volatile ("mrs %0, cpsr" : "=r" (cpsr));
	cpsr &= ~(PSR_IRQ | PSR_FIQ);
	asm volatile ("msr cpsr_c, %0" : : "r" (cpsr) : "memory", "cc");
	 while(1);
}

/**
 * @brief Allocate user-stacks and initializes the kernel contexts of the
 * given threads.
 *
 * This function assumes that:
 * - num_tasks < number of tasks allowed on the system.
 * - the tasks have already been deemed schedulable and have been appropriately
 *   scheduled.  In particular, this means that the task list is sorted in order
 *   of priority -- higher priority tasks come first.
 *
 * @param tasks  A list of scheduled task descriptors.
 * @param size   The number of tasks is the list.
 */
void allocate_tasks(task_t** tasks  , size_t num_tasks)
{
	/* initialize run queue */
	runqueue_init();

	/* initializing the tasks */
	unsigned int i;

	for (i = 0; i < num_tasks; ++i)
		{
			system_tcb[i].native_prio = i;
			system_tcb[i].cur_prio = i;
			system_tcb[i].holds_lock = 0;
			system_tcb[i].sleep_queue =(tcb_t *) 0;
			system_tcb[i].context.r6 = (uint32_t) tasks[0][i].stack_pos;
			system_tcb[i].context.r5 = (uint32_t) tasks[0][i].data;
			system_tcb[i].context.r4 = (uint32_t) tasks[0][i].lambda;
			system_tcb[i].context.sp = (void *) system_tcb[i].kstack_high;
			system_tcb[i].context.lr = (void *) launch_task;

			runqueue_add(&system_tcb[i],i);
		}	


	/* adding the idle task */
		system_tcb[IDLE_PRIO].native_prio = IDLE_PRIO;
		system_tcb[IDLE_PRIO].cur_prio = IDLE_PRIO;
		system_tcb[IDLE_PRIO].holds_lock = 0;
		system_tcb[IDLE_PRIO].sleep_queue =(tcb_t *) 0;
		system_tcb[IDLE_PRIO].context.r6 = (uint32_t) tasks[0][IDLE_PRIO].stack_pos;
		system_tcb[IDLE_PRIO].context.r4 = (uint32_t) (idle);
		system_tcb[IDLE_PRIO].context.r8 = (uint32_t) (global_data);
		system_tcb[IDLE_PRIO].context.sp = (void *) system_tcb[IDLE_PRIO].kstack_high;
		system_tcb[IDLE_PRIO].context.lr = (void*)(*launch_task);
		runqueue_add(&system_tcb[IDLE_PRIO],IDLE_PRIO);

		dispatch_init(&system_tcb[IDLE_PRIO]);
		dispatch_nosave();
}

