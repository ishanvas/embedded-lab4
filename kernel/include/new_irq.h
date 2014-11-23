
#ifndef NEW_IRQ_H
#define NEW_IRQ_H

#define IRQ_VEC_ADDRESS  0x18

void C_IRQ_Handler();
void irq_wrapper();
void New_IRQ_Handler();

#endif /* NEW_IRQ_H */
