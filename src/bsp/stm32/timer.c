
#include "timer.h"
#include "stm32f10x.h"

void sys_tick_init()
{
    SysTick_Config(SystemCoreClock / 1000);
}
