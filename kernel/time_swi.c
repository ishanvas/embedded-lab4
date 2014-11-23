

extern volatile unsigned long sys_time;

unsigned long time ()
{
    return (unsigned long) sys_time;
}
