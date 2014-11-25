#include <new_swi.h>
#include <bits/swi.h>
#include <types.h>
#include <task.h>
#include <syscall.h>
#include <lock.h>

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
          int mutex;
          unsigned int dev;
          int create_err;
          /* Routing based on swi_num */
          switch(swi_num) {                                   
          //case EXIT_SWI :                      
                  /* passing the adress of registers, helps in no return */                                         
              //    exit(regs);                  
            //      break;                        
          case READ_SWI :                                                
                  /* populating args from values on stack */
                  fd = (int)*regs;                                           
                  buff =(char *)*(regs+1);     
                  count = (int)*(regs+2);      
                  readCount = read_syscall(fd,buff,count);                                
                  *regs = readCount;                                              
                  break;                                          

          case CREATE_SWI:
                {
                /* populating args */
                task_t* tasks = (task_t *)*regs;                            
                size_t count =(size_t )*(regs+1);                    
                create_err = task_create(tasks,count);
                *regs = create_err;
                break;
                }
                                                                
        case WRITE_SWI :                                                
                /* populating args from values on stack */
                fd = (int)*regs;                            
                buff =(char *)*(regs+1);                    
                count = (int)*(regs+2);                     
                writeCount = write_syscall(fd,buff,count);                             
                *regs = writeCount;                                            
                break;                              
        case TIME_SWI :
                /*Time returns the time since the kernel boot*/
                time_elapsed = time_syscall();
                *regs = time_elapsed;
                break;

         case MUTEX_CREATE:
                *regs = mutex_create(); 
                break;

          case MUTEX_LOCK:
                mutex = (int)*regs;
                *regs = mutex_lock(mutex);
                break;

          case MUTEX_UNLOCK:
                mutex =(int)*regs;
                *regs = mutex_unlock(mutex);
                break;

          case EVENT_WAIT:
                dev = (unsigned int)*regs;
                *regs = event_wait(dev);
                break;

          case SLEEP_SWI:
                sleep_syscall((unsigned)*regs);

                break;

        default :                                               
                invalid_syscall(swi_num);
                break;                                                     
        }                                         

}

