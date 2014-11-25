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


 /*Function to validate the user program specification for the tasks*/
static int validate_tasks(task_t* tasks, size_t num_tasks){
	unsigned int i=0;
	for(i = 0; i < num_tasks ; i++){

	/*Check if the tasks will meet their deadlines*/
		if(tasks[i].C > tasks[i].T){
			return -ESCHED;
		}

	/*Check if the number of tasks is supported by OS*/
		if(num_tasks > OS_MAX_TASKS){
			return -EINVAL;
		}

	/*Check the stack postion of the tasks*/
		if((uint32_t)tasks[i].stack_pos < USR_START_ADDR|| 
			(uint32_t)tasks[i].stack_pos > USR_END_ADDR)
			return -EFAULT;
	}

	return 0;
}


/* Sorts the input task list by their frequenct (T) */
static void sortTasks(task_t* tasks, size_t num_tasks)
{
	unsigned int i;
	unsigned int j;
	
	/* Simple bubble sort implementation*/

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
	/* validate tasks are schedulable */
	int error = validate_tasks(tasks ,num_tasks);

	/* Returning if there is an error */
	if(error < 0)
	{
		return error;
	}

	/* disabling interrupts before creating tasks 
	 * When kernel starts a user process, it enables the interrupts again 
	 * So no need to enable interrupts here */
	disable_interrupts();

	/* initializing devices, so that if any previces tasks were sleeping in device queue they are removed */
	dev_init();

	/* sort the tasks in order of priority */
	sortTasks(tasks,num_tasks);
	
	/* allocating tasks */
	allocate_tasks(&tasks,num_tasks);

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
