/** @file ctx_switch.c
 * 
 * @brief C wrappers around assembly context switch routines.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-21
 */
 

#include <types.h>
#include <assert.h>

#include <config.h>
#include <kernel.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include "sched_i.h"


#ifdef DEBUG_MUTEX
#include <exports.h>
#endif

static tcb_t* cur_tcb; /* use this if needed */

/**
 * @brief Initialize the current TCB and priority.
 *
 * Set the initialization thread's priority to IDLE so that anything
 * will preempt it when dispatching the first task.
 */
void dispatch_init(tcb_t* idle )
{
	cur_tcb  = idle;
}


/**
 * @brief Context switch to the highest priority task while saving off the 
 * current task state.
 *
 * This function needs to be externally synchronized.
 * We could be switching from the idle task.  The priority searcher has been tuned
 * to return IDLE_PRIO for a completely empty run_queue case.
 */
void dispatch_save(void)
{
	/* getting priorities of current and new the tasks */	
	uint8_t cur_prio = get_cur_prio();
	uint8_t new_prio = highest_prio();

	/* if cur task is not the highest priority task*/
	if(cur_prio > new_prio)
	{
		tcb_t* curr_task = get_cur_tcb();

		/* getting new tasks tcb*/
		tcb_t* new_task  = &system_tcb[new_prio];

		/* setting the new task as current task */		
		cur_tcb = new_task;
		
		/*Switching context and saving prev context*/
		ctx_switch_full(&new_task->context,&curr_task->context);
	}
}

/**
 * @brief Context switch to the highest priority task that is not this task -- 
 * don't save the current task state.
 *
 * There is always an idle task to switch to.
 */
void dispatch_nosave(void)
{
	/* getting priorities of current and new the tasks */	
	uint8_t cur_prio = get_cur_prio();
	uint8_t new_prio = highest_prio();

	/* if cur task is not the highest priority task*/
	if(cur_prio > new_prio)
	{
		/* getting new tasks tcb*/
		tcb_t* new_task  = &system_tcb[new_prio];

		/* setting the new task as current task */		
		cur_tcb = new_task;

		/*Switching context without saving prev context*/
		ctx_switch_half(&new_task->context);
	}
}


/**
 * @brief Context switch to the highest priority task that is not this task -- 
 * and save the current task but don't mark is runnable.
 *
 * There is always an idle task to switch to.
 */
void dispatch_sleep(void)
{
	/* getting priorities of current and new the tasks */
	uint8_t cur_prio = get_cur_prio();
	

	/* getting the current task tcb*/
	tcb_t* curr_task = get_cur_tcb();

	/* if IDLE task is running, don't let it go to sleep */
	if (cur_prio < IDLE_PRIO)
	{

	/* Removing from the run queue */
	runqueue_remove(cur_prio);

	/* getting the higest priority task */
	uint8_t new_prio = highest_prio();
	/* getting new task */
	tcb_t* new_task  = &system_tcb[new_prio];

	/* setting the new task as current task */
	cur_tcb = new_task;

	/*Switching context and saving prev context*/
	ctx_switch_full(&new_task->context,&curr_task->context);
	}
	
}

/**
 * @brief Returns the priority value of the current task.
 */
uint8_t get_cur_prio(void)
{
	return cur_tcb->cur_prio; //fix this; dummy return to prevent compiler warning
}

/**
 * @brief Returns the TCB of the current task.
 */
tcb_t* get_cur_tcb(void)
{
	return cur_tcb; //fix this; dummy return to prevent compiler warning
}
