#include <types.h>


#ifndef NEW_SWI_H
#define NEW_SWI_H

#define SWI_VEC_ADDRESS  0x08

#define WRITE_ALLOWED_AREA_START 0xa0000000
#define WRITE_ALLOWED_AREA_END 0xa3efffff

#define READ_ALLOWED_AREA_START 0xa2000000
#define READ_ALLOWED_AREA_END 0xa2ffffff

void New_S_Handler();

#endif /* NEW_SWI_H */
