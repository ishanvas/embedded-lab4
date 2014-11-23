/**
 * @file device.c
 *
 * @brief Implements simulated devices.
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-12-01
 */

#include <types.h>
#include <assert.h>

#include <task.h>
#include <sched.h>
#include <device.h>
#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>

/**
 * @brief Fake device maintainence structure.
 * Since our tasks are periodic, we can represent 
 * tasks with logical devices. 
 * These logical devices should be signalled periodically 
 * so that you can instantiate a new job every time period.
 * Devices are signaled by calling dev_update 
 * on every timer interrupt. In dev_update check if it is 
 * time to create a tasks new job. If so, make the task runnable.
 * There is a wait queue for every device which contains the tcbs of
 * all tasks waiting on the device event to occur.
 */

struct dev
{
	tcb_t* sleep_queue;
	unsigned long   next_match;
};
typedef struct dev dev_t;

/* devices will be periodically signaled at the following frequencies */
const unsigned long dev_freq[NUM_DEVICES] = {100, 200, 500, 50};
static dev_t devices[NUM_DEVICES];

/**
 * @brief Initialize the sleep queues and match values for all devices.
 */
void dev_init(void)
{
	int i;
	/* initializing all the device next match values */
   for (i = 0; i < NUM_DEVICES; ++i)
   {	
   		devices[i].sleep_queue =  (tcb_t *) 0;
   		devices[i].next_match = dev_freq[i];
   }
}


/**
 * @brief Puts a task to sleep on the sleep queue until the next
 * event is signalled for the device.
 *
 * @param dev  Device number.
 */
void dev_wait(unsigned int dev __attribute__((unused)))
{

	tcb_t* cur_tcb = get_cur_tcb();

	tcb_t* cur_sleep_tcb = devices[dev].sleep_queue;

	uint32_t cpsr;
	asm volatile ("mrs %0, cpsr" : "=r" (cpsr));
	cpsr |= PSR_IRQ | PSR_FIQ;
	asm volatile ("msr cpsr_c, %0" : : "r" (cpsr) : "memory", "cc");

	if(cur_sleep_tcb == (tcb_t*)0)
	{
		devices[dev].sleep_queue =  cur_tcb;	

	}
	else
	{
		while(cur_sleep_tcb->sleep_queue != (tcb_t*)0)
		{
			cur_sleep_tcb = cur_sleep_tcb->sleep_queue;
		}

		cur_sleep_tcb->	sleep_queue = cur_tcb;
	}


	dispatch_sleep();
	
	asm volatile ("mrs %0, cpsr" : "=r" (cpsr));
	cpsr &= ~(PSR_IRQ | PSR_FIQ);
	asm volatile ("msr cpsr_c, %0" : : "r" (cpsr) : "memory", "cc");
	



	
}




/**
 * @brief Signals the occurrence of an event on all applicable devices. 
 * This function should be called on timer interrupts to determine that 
 * the interrupt corresponds to the event frequency of a device. If the 
 * interrupt corresponded to the interrupt frequency of a device, this 
 * function should ensure that the task is made ready to run 
 */
void dev_update(volatile unsigned long millis)
{
	int match =0;
	int i;
	for (i = 0; i < NUM_DEVICES; ++i)
	{
		if( millis % devices[i].next_match == 0)
		{
			match =1;
				/* Disabling interrupts since, add should be synchronized */
			
					/* Removing current task from run list */

			tcb_t* cur_sleep_tcb = devices[i].sleep_queue;
			while(cur_sleep_tcb != (tcb_t*) 0)
			{
				runqueue_add(devices[i].sleep_queue, devices[i].sleep_queue->cur_prio);	
				cur_sleep_tcb = cur_sleep_tcb->sleep_queue;
			}	
			/* enabling interrupts again */
	
			
		}
		/* code */
	}
	
	if(match)
	{	
		dispatch_save();				
	}

}

