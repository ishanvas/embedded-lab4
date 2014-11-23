#include <exports.h>
#include <types.h>
#include <bits/errno.h>
#include <new_swi.h>

ssize_t read(int fd, void *buf, size_t count)       
{                                                    
        /* returns error in case fd doesn't point to stdin*/
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
                        if(c == 4 )
                        {                                              
                                break;                                            
                        }                              
                        /* checking for delete and backsp */                           
                        else if ((c == 8)||(c == 127)) 
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


