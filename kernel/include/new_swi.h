#include <types.h>


#ifndef NEW_SWI_H
#define NEW_SWI_H

#define SWI_VEC_ADDRESS  0x08

#define WRITE_ALLOWED_AREA_START 0xa0000000
#define WRITE_ALLOWED_AREA_END 0xa3efffff

#define READ_ALLOWED_AREA_START 0xa2000000
#define READ_ALLOWED_AREA_END 0xa2ffffff

void New_S_Handler();

ssize_t read(int fd, void *buf, size_t count)  ;
ssize_t write(int fd, const void *buf, size_t count);
void exit(unsigned *argv);
unsigned long time ();
void sleep (unsigned sleep_time);

#endif /* NEW_SWI_H */
