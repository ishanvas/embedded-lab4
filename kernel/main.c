/*
 * kernel.c: Kernel main (entry) function
 *
 * Author: Author: Ishan Vashishtha <ivashish@andrew.cmu.edu>
 * Date:   Mon, 20 Oct 2014
 */

#include <exports.h>

#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/interrupt.h>
#include <arm/timer.h>

#include <new_swi.h>
#include <new_irq.h>
#include <kernel_utils.h>
#include <types.h>
#include <exports.h>
#include <timer_driver.h> 


uint32_t global_data;

/* Pointer to SWI Handler */
volatile unsigned *swi_ptr;

/* Pointer to IRQ Handler */
volatile unsigned *irq_ptr;


/* Sets the global pointer for IRQ & SWI handler */ 
void set_handler_ptr(unsigned vector_address)
{
  unsigned *vector = (unsigned *) vector_address;
  unsigned vec_off = (*vector & 0x00000FFF); /* getting the last 12 bits from the instruction*/
  unsigned *table_ptr =  (unsigned *)(0x8 + vector_address +vec_off); /* adding 8 since pc is pointing 8 bytes ahead*/
  unsigned *t_ptr = (unsigned *) (*table_ptr); /* pointer to the defaukt swi */

  if (vector_address == SWI_VEC_ADDRESS)
  {
    swi_ptr = t_ptr;
  }
  else
  {
    irq_ptr = t_ptr;  
  }
  
}



unsigned * get_handler_ptr(volatile unsigned vector_address)
{
  unsigned *vector = (unsigned *) vector_address;
  unsigned vec_off = (*vector & 0x00000FFF); /* getting the last 12 bits from the instruction*/
  unsigned *table_ptr =  (unsigned *)(0x8 + vector_address +vec_off); /* adding 8 since pc is pointing 8 bytes ahead*/
  unsigned *swi_ptr = (unsigned *) (*table_ptr); /* pointer to the defaukt swi */

  return swi_ptr;
}

 void install_redirect_ins(unsigned* hdlr_ptr, unsigned new_swi_adr, unsigned ins[])
{
  unsigned offset = 0x004;

  ins[0]=*hdlr_ptr; /* getting old instruction in old_ptr1*/
  ins[1]=*(hdlr_ptr+1); /* getting old instruction in old_ptr2*/
  *hdlr_ptr=(offset|0xe51FF000); /* storing ldr pc, [pc,#-4]*/
  *(hdlr_ptr+1)=new_swi_adr; /* storing address of new swi handler*/	
}


 void install_g_handler(unsigned vector_address, unsigned new_adr, unsigned ins[])
{
  //unsigned* hdlr_ptr = get_handler_ptr(vector_address); 
  unsigned offset = 0x004;
  if (vector_address == SWI_VEC_ADDRESS)
  {    
      ins[0]=*swi_ptr; /* getting old instruction in old_ptr1*/
      ins[1]=*(swi_ptr+1); /* getting old instruction in old_ptr2*/
      *swi_ptr=(offset|0xe51FF000); /* storing ldr pc, [pc,#-4]*/
      *(swi_ptr+1)=new_adr;
  }
  else
  {
    /* saving current instructions, to restore in future */
      ins[0]=*irq_ptr; 
      ins[1]=*(irq_ptr+1); /* getting old instruction in old_ptr2*/

      /* storing ldr pc, [pc,#-4] and address of new handler */
      *irq_ptr=(offset|0xe51FF000); 
      *(irq_ptr+1)=new_adr;
  }
}



void install_handler(unsigned vector_address, unsigned new_swi_adr, unsigned ins[])
{
  unsigned* hdlr_ptr = get_handler_ptr(vector_address); 
  install_redirect_ins(hdlr_ptr,new_swi_adr,ins);
}

/* Restore handler's original state */
void restore_g_handler(unsigned vector_address, unsigned ins[])
{
  if (vector_address == SWI_VEC_ADDRESS)
  {
      *swi_ptr=ins[0]; /* restoring first instruction */
      *(swi_ptr+1)=ins[1]; /* restoring second instruction */
  }
  else
  {
      *irq_ptr=ins[0]; /* restoring first instruction */
      *(irq_ptr+1)=ins[1]; /* restoring second instruction */
  }

}


 void restore_handler(unsigned vector_address, unsigned ins[])
{
  unsigned* hdlr_ptr = get_handler_ptr(vector_address); 
  
  *hdlr_ptr=ins[0]; 
  *(hdlr_ptr+1)=ins[1];
}


int kmain(int argc, char** argv, uint32_t table)
{
	app_startup(); /* bss is valid after this point */
	global_data = table;

	unsigned swi_ins[2] ={0,0}; /* array to store replaced SWI instructions */
	unsigned irq_ins[2] ={0,0}; /* array to store replaced IRQ instructions */
	unsigned new_swi_adr = (unsigned) New_S_Handler; /* pointer to new SWI handler */
	unsigned new_irq_adr = (unsigned) New_IRQ_Handler; /* pointer to new IRQ handler */

  /* Setting the global ptrs of SWI and IRQ handler */
  set_handler_ptr(SWI_VEC_ADDRESS);
  set_handler_ptr(IRQ_VEC_ADDRESS);


	/* installs new swi handler */
  install_g_handler(SWI_VEC_ADDRESS,new_swi_adr,swi_ins);

  /* installs new irq handler */
  install_g_handler(IRQ_VEC_ADDRESS,new_irq_adr,irq_ins);
	
	
  /* initializes OS timer */
	init_timer();

  /* setups up stack for IRQ handler */
	setup_irq_stack();

  /* sets up user stack and calling user program */
	setup_stack(argc,argv); 

	/* Restoring SWI and IRQ handler to their original state */
  restore_g_handler(SWI_VEC_ADDRESS,swi_ins);
  restore_g_handler(IRQ_VEC_ADDRESS,irq_ins);
	
	return 0;
}

