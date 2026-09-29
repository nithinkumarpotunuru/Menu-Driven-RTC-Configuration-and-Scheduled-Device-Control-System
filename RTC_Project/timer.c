#include "timer.h"

void delay_init(void)
{
    T0TCR = 1 << 1;
    T1TCR = 1 << 1;
    
    T0MCR = 1 << 2;
    T1MCR = 1 << 2;
}

void delay_ms(u32 ms)
{
    T0MR0 = ms;
    T0PR = 15000 - 1;
    T0TC = 0;
    T0TCR = 1 << 0;
    while(T0MR0 != T0TC);
}

void delay_ms1(u32 ms)
{
    T1MR0 = ms;
    T1PR = 15000 - 1;
    T1TC = 0;
    T1TCR = 1 << 0;
}
