/** @file io.c
 *
 * @brief Kernel I/O syscall implementations
 *
 * @author Mike Kasick <mkasick@andrew.cmu.edu>
 * @date   Sun, 14 Oct 2007 00:07:38 -0400
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-11-16
 */

#include <types.h>
#include <bits/errno.h>
#include <bits/fileno.h>
#include <arm/physmem.h>
#include <syscall.h>
#include <exports.h>
#include <kernel.h>
 #include <new_swi.h>

#define EOT_CHAR 0x04
#define DEL_CHAR 0x7f
#define BACKSPACE_CHAR 0x8
#define NEWLINE_CHAR 0XA
#define CRLF_CHAR 0xC


/* Read count bytes (or less) from fd into the buffer buf. */
ssize_t read_syscall(int fd __attribute__((unused)), void *buf __attribute__((unused)), size_t count __attribute__((unused)))
{

   if(fd!=0)
        {
            return -EBADF;
        }


   char* bufptr = (char *)buf;

   /* checks if the buffer is the allowed area of memory */
   if( (unsigned int)&buf < READ_ALLOWED_AREA_START || ( ((unsigned int)(buf)+count ) > READ_ALLOWED_AREA_END ) )
   {
       return -EFAULT;
   }

   unsigned int i =0;

   /* Start reading characters from STDIN*/
   for (i = 0; i < count; ++i)
   {
       /* read a character from STDIN */
       char c = getc();

       /* checks for EOT character */
       if(c == EOT_CHAR )
       {
           break;
       }
       /* checking for delete and backsp */
       else if ((c == BACKSPACE_CHAR)||(c == DEL_CHAR))
       {
           i--;
           puts("\b \b");
       }
       /* for everything else */
       else
       {
       /* checks for new line or CR */
           if ((c == 10)||(c == 13))
           {
               bufptr[i] = '\n';
               putc('\n');
               /* since we are breaking, value of i would be 1 less
                * So incrementing i */
               i++;
               break;
           }
           else
           {
           /* Stores the character in the buffer */
               bufptr[i] = c;
           /* Display character to the user */
               putc(c);
           }
      }

  } 
  /* finally 'i' will contain count of characters read(written in buffer) */
  return i;
}

/* Write count bytes to fd from the buffer buf. */
ssize_t write_syscall(int fd  __attribute__((unused)), const void *buf  __attribute__((unused)), size_t count  __attribute__((unused)))
{
// returning error in case fd doesn't point to stdout                   
    if(fd != 1)
    {
        return -EBADF;
    }

    char* bufptr = (char*)buf;
/* if the buffer points to a restricted area of memory , returns -EFAULT */
    if( (unsigned int)buf < WRITE_ALLOWED_AREA_START || ( ( (unsigned int)(buf)+count ) > WRITE_ALLOWED_AREA_END ) )
    {
        return -EFAULT;
    }

    unsigned int i =0;
/* Read characters from buffer*/
    for (i = 0; i < count; ++i)
    {
    /* Reads character at index i from buffer*/
        char c = bufptr[i];
    /* prints \n for CR or LF */
        if ((c == NEWLINE_CHAR)||(c == CRLF_CHAR))
        {
    /* Change line */
            putc('\n');
        }    
        else
        {
    /* Displays character on STDOUT*/
            putc(c);
        }
    }
/* finally 'i' will contain count of characters written 
 * So returning i */
    return i;	
}
