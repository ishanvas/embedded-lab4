#include <exports.h>
#include <types.h>
#include <bits/errno.h>
#include <new_swi.h>


/* SWI Handler for write */
 ssize_t write(int fd, const void *buf, size_t count)                            
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
			                  if ((c == 10)||(c == 13))
                  			{
                          /* Change line */
                  				putc('\n');
                  			} 
                  			else{
                          /* Displays character on STDOUT*/
                  				putc(c);                                                
                  			}
                 }                                                                       
                                                                                         
          /* finally 'i' will contain count of characters written 
           * So returning i */                         
                                                                                         
          return i;                                                               
  }                                                                   
