#include <arm/timer.h>


extern volatile unsigned long sys_time;

void C_IRQ_Handler(){                                                                                                                             
	
	sys_time += 10;

  volatile unsigned *ossr = (unsigned *) (TIMER_BASE + OSTMR_OSSR_ADDR);
  volatile unsigned *oscr =(unsigned *) (TIMER_BASE + OSTMR_OSCR_ADDR);
  *ossr = *ossr | 0x1; /*Writing 1 to clear the status register*/
  *oscr = 0;
}
