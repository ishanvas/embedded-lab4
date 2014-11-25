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
	/* initializing all the devices, setting next match = dev frequency and sleep queue = NULL */
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
void dev_wait(unsigned int dev )
{
	/* Getting current running task */
	tcb_t* cur_tcb = get_cur_tcb();

	/* Getting current task in sleep queue */
	tcb_t* cur_sleep_tcb = devices[dev].sleep_queue;

	/* disabling interrupts */
	disable_interrupts();

	/* putting the task in sleep queue */
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

	/* making the current task sleep */
	dispatch_sleep();	

	/* process will enable interrupts when it is awaken */
	enable_interrupts();
	
}




/**
 * @brief Signals the occurrence of an event on all applicable devices. 
 * This function should be called on timer interrupts to determine that 
 * the interrupt corresponds to the event frequency of a device. If the 
 * interrupt corresponded to the interrupt frequency of a device, this 
 * function should ensure that the task is made ready to run 
 */
void dev_update(unsigned long millis)
{

	int match =0; /* set to 1 if even a single match has occured */
	unsigned int index =0;

	/* Wakes up all the sleeping tasks, whose dev frequency has matched */

	while (index < NUM_DEVICES)
	{	
		/* if time is >0 and a match has occured */
		if( millis >0  && (millis % devices[index].next_match == 0))
		{

			/* Removing current task from run list */
			while(devices[index].sleep_queue != (tcb_t*)0){
				match =1;
				tcb_t* cur_sleep_tcb = devices[index].sleep_queue;
				runqueue_add(devices[index].sleep_queue, devices[index].sleep_queue->cur_prio);
				devices[index].sleep_queue = cur_sleep_tcb->sleep_queue;
				cur_sleep_tcb->sleep_queue = (tcb_t*)0;
			}	
			
		}

		index=index+1;
	}
	
	/* if a process has woken up */
	if(match >0)
	{	
		/* doing a full context switch */
		dispatch_save();				
	}

}

