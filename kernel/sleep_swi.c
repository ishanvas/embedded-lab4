

extern volatile unsigned long sys_time;

void sleep (unsigned sleep_time)
{
    unsigned long wake_up_time = sys_time + sleep_time;

    while(1)
    {
       if (sys_time > wake_up_time)
            break;
      //printf("sleeping %lu until %lu\n", sys_time,wake_up_time);
    }
}
