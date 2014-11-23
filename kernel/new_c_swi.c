#include <new_swi.h>

/* Handles SWI requests
* based on the SWI number redirects to appropriate SWI handler */
void C_SWI_Handler(unsigned swi_num, unsigned *regs )
  {                                                               
                                                                                  
          int fd;                                     
          char *buff;                                           
          size_t count;                                           
          size_t readCount;                                       
          size_t writeCount;                                      
          unsigned long time_elapsed;

          /* subtracting base SWI address */                                                              
          swi_num = swi_num - (0x900000);                     

          /* Routing based on swi_num */
          switch(swi_num) {                                   
          case 1 :                      
                  /* passing the adress of registers, helps in no return */                                         
                  exit(regs);                  
                  break;                        
          case 3 :                                                
                  /* populating args from values on stack */
                  fd = (int)*regs;                                           
                  buff =(char *)*(regs+1);     
                  count = (int)*(regs+2);      
                  readCount = read(fd,buff,count);                                
                  *regs = readCount;                                              
                  break;                                          
                                                                
        case 4 :                                                
                /* populating args from values on stack */
                fd = (int)*regs;                            
                buff =(char *)*(regs+1);                    
                count = (int)*(regs+2);                     
                writeCount = write(fd,buff,count);                             
                *regs = writeCount;                                            
                break;                              
        case 6 :
                /*Time returns the time since the kernel boot*/
                time_elapsed = time((unsigned)*regs);
                *regs = time_elapsed;
                
                break;

          case 7:
                sleep ((unsigned)*regs);

                break;
                                                  
        default :                                               
                break;                                                     
        }                                         

}

