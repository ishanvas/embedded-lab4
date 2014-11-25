/**
 * @file mutex.c
 *
 * @brief Implements mutices.
 *
 * @author Harry Q Bovik < PUT YOUR NAMES HERE
 *
 * 
 * @date  
 */

#define DEBUG_MUTEX

#include <lock.h>
#include <task.h>
#include <sched.h>
#include <bits/errno.h>
#include <arm/psr.h>
#include <arm/exception.h>
#ifdef DEBUG_MUTEX
#include <exports.h>  temp
#endif


mutex_t gtMutex[OS_NUM_MUTEX];


void mutex_init()
{
	////disable_interrupts();
	int i = 0;
    while(i < OS_NUM_MUTEX){
        gtMutex[i].bAvailable = TRUE;
        gtMutex[i].bLock = 0;
        gtMutex[i].pSleep_queue = (tcb_t*)0;
        gtMutex[i].pHolding_Tcb = (tcb_t*)0;
        i++;
    }
    ////enable_interrupts();
}

int mutex_create(void)
{   
	//disable_interrupts();
    int i = 0;
    int mutex_ret = -ENOMEM;
    while (i < OS_NUM_MUTEX){
        if(gtMutex[i].bAvailable == TRUE) {
            gtMutex[i].bAvailable = FALSE;
            mutex_ret =  i;
            break;
         }
         
         i++;
    //enable_interrupts();
    }

    
    /*If the function reaches here it means that none of the mutexs are available to be created*/
    return mutex_ret;
}

int mutex_lock(int mutex)
{

	//disable_interrupts();p
	unsigned int ret = 0;

	/* Getting current running task */
	tcb_t* curr_task = get_cur_tcb();

    /*Check if the mutex was created before the lock*/
    if(gtMutex[mutex].bAvailable == TRUE){
        ret = -EINVAL;
    }
    /*Check if the mutex is already acquired by the same process*/
    if(gtMutex[mutex].bLock != 0 && gtMutex[mutex].pHolding_Tcb == curr_task){
        ret = -EDEADLOCK;
    }
    /*Add task to sleepqueue if mutex is held by another process*/
    else if(gtMutex[mutex].bLock != 0){
        
        /* if the sleep queue is empty */
        if(gtMutex[mutex].pSleep_queue == (tcb_t *) 0){
            gtMutex[mutex].pSleep_queue = curr_task;
        }
        /* reach the end of queue and add the tcb */
        else{
            
            tcb_t* queue_rover;
            queue_rover = gtMutex[mutex].pSleep_queue;
            
            while(queue_rover->sleep_queue != ((tcb_t *) 0)){
                queue_rover = queue_rover->sleep_queue;
            }

            queue_rover->sleep_queue = curr_task;

        }
        
        dispatch_sleep();

        /* giving the mutex to awaken task */
		gtMutex[mutex].bLock = 1;
        gtMutex[mutex].pHolding_Tcb = curr_task;
    }        
    /*Mutex is unlocked and available*/
    else {

        gtMutex[mutex].bLock = 1;
        gtMutex[mutex].pHolding_Tcb = curr_task;
    }

    //enable_interrupts();
    return ret;
	
}

int mutex_unlock(int mutex)
{
	//disable_interrupts();   
    tcb_t* curr_task = get_cur_tcb();
    
    /*Validate function parameters*/
    if(gtMutex[mutex].bAvailable != FALSE || gtMutex[mutex].bLock != 1 || mutex >= 32 || mutex < 0){
        return -EINVAL;
    }

    /*Validate the task requesting the unlock*/
    else if(gtMutex[mutex].pHolding_Tcb != curr_task) {
        return -EPERM;
    }

    /*Unlock the mutex*/
    else {
        gtMutex[mutex].bLock = 0;
        gtMutex[mutex].pHolding_Tcb = (tcb_t *) 0;
                
        if(gtMutex[mutex].pSleep_queue != (tcb_t *) 0){
            tcb_t* first_in_queue = gtMutex[mutex].pSleep_queue;
            gtMutex[mutex].pSleep_queue = first_in_queue->sleep_queue;
            first_in_queue->sleep_queue = (tcb_t *) 0;

            /* Removing current task from run list */
            runqueue_add(first_in_queue, first_in_queue->cur_prio);
            /* enabling interrupts again */
            
        }
    }

    //enable_interrupts();
    return 0;
}


