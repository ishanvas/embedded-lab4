#include <arm/timer.h>
#include <arm/interrupt.h>


volatile unsigned long sys_time =0;

/*Timer Driver : Design Specs
*1. The interrupts from OSMR1, OSMR2 and OSMR3 are masked.
*2. Interrupts from OSMR0 are set
*3. Bit 26 of ICLR is set so that OSMR0 generates IRQ
*4. The OSCR is set to 0.
*5. The OSMR0 is set to 3.25*10^3 to generate a match every milisecond.
*6. Setting the 0th bit of OIER to 1 to allow OSSR to be set when OSMR0 match occurs
*/ 
void init_timer()
{
    volatile unsigned *oscr = (unsigned *) (TIMER_BASE + OSTMR_OSCR_ADDR);
    volatile unsigned *osmr0 =(unsigned *) (TIMER_BASE + OSTMR_OSMR_ADDR(0));
    volatile unsigned *oier =(unsigned *) (TIMER_BASE + OSTMR_OIER_ADDR);
    volatile unsigned *iclr =(unsigned *) (TIMER_BASE + INT_ICLR_ADDR);
    volatile unsigned *icmr =(unsigned *) (TIMER_BASE + INT_ICMR_ADDR);

    unsigned temp = *oier & 0xFFFFFFF0;
    
    *oier = (temp) | 0x1;

    *icmr = ( 1 << INT_OSTMR_0);
    
    *iclr =  (~( 1 << INT_OSTMR_0));
    
    *osmr0 = 10*3250;

    *oscr = 0x0;
    
}
