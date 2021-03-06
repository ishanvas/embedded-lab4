/** @file run_queue.c
 * 
 * @brief Run queue maintainence routines.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-21
 */

#include <types.h>
#include <assert.h>

#include <kernel.h>
#include <sched.h>
#include "sched_i.h"



static tcb_t* run_list[OS_MAX_TASKS] ;

/* A high bit in this bitmap means that the task whose priority is
 * equal to the bit number of the high bit is runnable.
 */
static uint8_t run_bits[OS_MAX_TASKS/8] ;

/* This is a trie structure.  Tasks are grouped in groups of 8.  If any task
 * in a particular group is runnable, the corresponding group flag is set.
 * Since we can only have 64 possible tasks, a single byte can represent the
 * run bits of all 8 groups.
 */
static uint8_t group_run_bits ;

/* This unmap table finds the bit position of the lowest bit in a given byte
 * Useful for doing reverse lookup.
 */
static uint8_t prio_unmap_table[]  __attribute__((unused)) =
{

0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};

/**
 * @brief Clears the run-queues and sets them all to empty.
 */
void runqueue_init(void)
{   
	/*Initialize the group run bits*/
	group_run_bits =0;
	/*Initialize all the bits in the run bits*/
	unsigned int i =0;
	for (i = 0; i < (OS_MAX_TASKS/8); ++i)
	{
		run_bits[i] =0;
	}

}

/**
 * @brief Adds the thread identified by the given TCB to the runqueue at
 * a given priority.
 *
 * The native priority of the thread need not be the specified priority.  The
 * only requirement is that the run queue for that priority is empty.  This
 * function needs to be externally synchronized.
 */
void runqueue_add(tcb_t* tcb , uint8_t prio  )
{
	/*The corresponding run list element is equated to tcb*/ 
	run_list[prio] = tcb;

	unsigned int array_index = prio/8;
	unsigned int bit_index = prio%8;
    /*The corresponding run_bits and group_run_bits bit are set to indicate the addition*/
	run_bits[array_index]  = run_bits[array_index] | (1<<bit_index);
	group_run_bits = group_run_bits | (1 << (array_index));
}


/**
 * @brief Empty the run queue of the given priority.
 * @return  The tcb at enqueued at the given priority.
 * This function needs to be externally synchronized.
 */
tcb_t* runqueue_remove(uint8_t prio)
{
	unsigned int array_index = prio/8;
	unsigned int bit_index = prio%8;
	/*Set the corresponding bit in run bits to zero*/
	run_bits[array_index] = run_bits[array_index] & ~(1<<bit_index);

    /*if run bits becomes zero, then set the bit in the group run bits to zero*/
	if (run_bits[array_index] == 0)
    {
    	group_run_bits = group_run_bits & ~(1 << (array_index));
    }

	return run_list[prio]; 
}

/**
 * @brief This function examines the run bits and the run queue and returns the
 * priority of the runnable task with the highest priority (lower number).
 */
uint8_t highest_prio(void)
{   /*Makes use of the map provided to get the highest priority running*/
    /*Makes use of the least significant set bit in the group run buts
     *and the least significant set bit in the corresponding run bits*/
	unsigned int y = prio_unmap_table[group_run_bits];
    unsigned int x = prio_unmap_table[run_bits[y]];
    unsigned int prio= (y << 3) + x;
    return prio; // fix this; dummy return to prevent warning messages	
}
