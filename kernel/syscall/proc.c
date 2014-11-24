/** @file proc.c
 * 
 * @brief Implementation of `process' syscalls
 *
 * @author Mike Kasick <mkasick@andrew.cmu.edu>
 * @date   Sun, 14 Oct 2007 00:07:38 -0400
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-12
 */

#include <exports.h>
#include <bits/errno.h>
#include <config.h>
#include <kernel.h>
#include <syscall.h>
#include <sched.h>

#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/physmem.h>
#include <device.h>



static void sortTasks(task_t* tasks, size_t num_tasks)
{
	unsigned int i;
	unsigned int j;
	/* implementing bubble sort */
	
	for (i = 0; i < (num_tasks-1); ++i)
	{
		for (j = 0; j < num_tasks-(1+i); ++j)
		{
			if(tasks[j].T > tasks[j+1].T )
			{
				task_t temp = tasks[j];
				tasks[j] = tasks[j+1];
				tasks[j+1] = temp;
			}
			
		}
	}
}



int task_create(task_t* tasks , size_t num_tasks)
{
	uint32_t cpsr;

	asm volatile ("mrs %0, cpsr" : "=r" (cpsr));
	cpsr |= PSR_IRQ | PSR_FIQ;
	asm volatile ("msr cpsr_c, %0" : : "r" (cpsr) : "memory", "cc");

	/* validate tasks are schedulable */

	/* initializing devices */
	dev_init();

	/* sort the tasks in order of priority */
	sortTasks(tasks,num_tasks);
	
	/* call allocate_tasks */
	allocate_tasks(&tasks,num_tasks);

	asm volatile ("mrs %0, cpsr" : "=r" (cpsr));
	cpsr &= ~(PSR_IRQ | PSR_FIQ);
	asm volatile ("msr cpsr_c, %0" : : "r" (cpsr) : "memory", "cc");

    return 0; /* remove this line after adding your code */
}

int event_wait(unsigned int dev)
{    
    /*Validate the input parameter to the function*/
    if(dev >= NUM_DEVICES) { 
        return -EINVAL;
    }

    /*Add current task to sleep queue for the device*/
    dev_wait(dev);    
 
    /*Event wait succesful*/
    return 0;
}

/* An invalid syscall causes the kernel to exit. */
void invalid_syscall(unsigned int call_num  __attribute__((unused)))
{
	printf("Kernel panic: invalid syscall -- 0x%08x\n", call_num);

	//disable_interrupts();
	while(1);
}
