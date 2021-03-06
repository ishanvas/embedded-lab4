/** @file time.c
 *
 * @brief Kernel timer based syscall implementations
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-11-16
 */

#include <types.h>
#include <config.h>
#include <bits/errno.h>
#include <arm/timer.h>
#include <syscall.h>

extern volatile unsigned long sys_time;


unsigned long time_syscall(void)
{
     return (unsigned long) sys_time;
}



/** @brief Waits in a tight loop for atleast the given number of milliseconds.
 *
 * @param millis  The number of milliseconds to sleep.
 *
 * 
 */
void sleep_syscall(unsigned long millis  )
{
	unsigned long wake_up_time = sys_time + millis;

    while(1)
    {
       if (sys_time > wake_up_time)
            break;
      //printf("sleeping %lu until %lu\n", sys_time,wake_up_time);
    }
}
