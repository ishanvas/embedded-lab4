/**
 * @file mutex.c
 *
 * @brief Implements mutices.
 *
 * @author Ishan Vashishtha
           Leelakrishna Nukala
 *
 * 
 * 11/20/2014
 */

#define DEBUG_MUTEX

#include <lock.h>
#include <task.h>
#include <sched.h>
#include <bits/errno.h>
#include <arm/psr.h>
#include <arm/exception.h>
#ifdef DEBUG_MUTEX
#include <exports.h>  
#endif


mutex_t gtMutex[OS_NUM_MUTEX];


/*Function to initiaize the available mutex structures*/
void mutex_init()
{   /*Disabling interrupts for atomicity*/
	disable_interrupts();
	int i = 0;
    while(i < OS_NUM_MUTEX){
        /*Make the mutex available for creation*/
        gtMutex[i].bAvailable = TRUE;
        /*Make the mutex free*/
        gtMutex[i].bLock = 0;
        /*The sleep queue is initialized to null*/
        gtMutex[i].pSleep_queue = (tcb_t*)0;
        /*The holdig tcb pointer is initialized to null*/
        gtMutex[i].pHolding_Tcb = (tcb_t*)0;

        i++;
    }
    /*enabling interrupts prior to exit*/
    enable_interrupts();
}

/*Function to create a mutex based on availability*/
int mutex_create(void)
{   
	disable_interrupts();
    int i = 0;
    int mutex_ret = -ENOMEM;
    while (i < OS_NUM_MUTEX){
        if(gtMutex[i].bAvailable == TRUE) {
            gtMutex[i].bAvailable = FALSE;
            mutex_ret =  i;
            break;
         }
         
         i++;
    enable_interrupts();
    }    
    
    /*If the function reaches here it means that none of the mutexs are available to be created*/
    return mutex_ret;
}

/*Function to lock a mutex based on status*/
int mutex_lock(int mutex)
{

	disable_interrupts();
	unsigned int ret = 0;

	/* Getting current running task */
	tcb_t* curr_task = get_cur_tcb();

    /*Check mutex number range and if the mutex was created before the lock*/
    if(gtMutex[mutex].bAvailable == TRUE || mutex >= 32 || mutex < 0){
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
            /*THe last element of the queue will point to the current task*/ 
            queue_rover->sleep_queue = curr_task;
        }
        
        dispatch_sleep();

        /*The lock will be held by the task that returns from dispatch sleep upon getting 
         *reaching the foreground due to cntext switch*/
		gtMutex[mutex].bLock = 1;
        gtMutex[mutex].pHolding_Tcb = curr_task;
    }        
    /*Mutex is unlocked and available*/
    else {

        gtMutex[mutex].bLock = 1;
        gtMutex[mutex].pHolding_Tcb = curr_task;
    }

    enable_interrupts();
    return ret;
	
}

/*Function to unlock a mutex*/
int mutex_unlock(int mutex)
{
    /*Disabling interrupts to provide function atomicity*/
	disable_interrupts();   
    tcb_t* curr_task = get_cur_tcb();
    
    /*Validate function parameters
     *Mutex could not have been created
     *mutex could already be unlocked
     *mutex number provided could be greater than 32*/
    if(gtMutex[mutex].bAvailable != FALSE || 
        mutex >= 32 || mutex < 0){
        
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
        
        /*If the queue is not empty, then the first task in the mutex queue will have to be added t the runlist
         *and removed from the queue*/        
        if(gtMutex[mutex].pSleep_queue != (tcb_t *) 0){
            tcb_t* first_in_queue = gtMutex[mutex].pSleep_queue;
            gtMutex[mutex].pSleep_queue = first_in_queue->sleep_queue;
            first_in_queue->sleep_queue = (tcb_t *) 0;
            runqueue_add(first_in_queue, first_in_queue->cur_prio);
        }
    }
    /*Enabling interputs for future processes*/
    enable_interrupts();
    return 0;
}
