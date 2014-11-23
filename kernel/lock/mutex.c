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

//#define DEBUG_MUTEX

#include <lock.h>
#include <task.h>
#include <sched.h>
#include <bits/errno.h>
#include <arm/psr.h>
#include <arm/exception.h>
#ifdef DEBUG_MUTEX
#include <exports.h> // temp
#endif

#define NULL 0

mutex_t gtMutex[OS_NUM_MUTEX];

void mutex_init()
{
	
}

int mutex_create(void)
{
    int i = 0;
    while (i < OS_NUM_MUTEX){
        if(gtMutex[i].bAvailable == TRUE) {
            gtMutex[i].bAvailable = FALSE;
            gtMutex[i].bLock = 0;
            return i;
         }
         i++;
    }
    /*If the function reaches here it means that none of the mutexs are available to be created*/
    return -ENOMEM;
}

int mutex_lock(int mutex)
{
    /*Check if the mutex was created before the lock*/
    if(gtMutex[mutex].bAvailable == FALSE){
        return -EINVAL;
    }
    /*Check if the mutex is already acquired*/
    if(gtMutex[mutex].bLock != 0 && gtMutex[mutex].pHolding_Tcb == get_cur_tcb()){
        return -EDEADLOCK;
    }
    /*Add task to sleepqueue if mutex is held by another TCB*/
    else if(gtMutex[mutex].bLock != 0){
        tcb_t* curr_task = get_cur_tcb();
        gtMutex[mutex].pSleep_queue = curr_task;
        return 0;
    }
    /*Mutex is unlocked and available*/
    else {
    gtMutex[mutex].bLock = 1;
    gtMutex[mutex].pHolding_Tcb = get_cur_tcb();
    gtMutex[mutex].pSleep_queue = NULL;
    return 0;
    }
	
}

int mutex_unlock(int mutex)
{   
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
        if(gtMutex[mutex].pSleep_queue == NULL){
            gtMutex[mutex].pHolding_Tcb = NULL;
        }
        else {
            gtMutex[mutex].pHolding_Tcb = gtMutex[mutex].pSleep_queue;
            /*shift the queue -- FIX THIS*/
        }
    }
    return 0;
}

