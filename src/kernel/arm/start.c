
#include "debug.h"

#include <string.h>
#include <stdio.h>

#include "arm.h"

extern char __etext__;
extern char __sdata__;
extern char __edata__;
extern char __sbss__;
extern char __ebss__;
extern char __erom__;

int main();
void SystemInit();
void thread1();

unsigned long stk1[256];
unsigned long stk2[256];

void start()
{
    /* Copy data from ROM to RAM */
    memcpy(&__sdata__, &__erom__, &__edata__ - &__sdata__);

    /* Clear the BSS */
    memset(&__sbss__, 0, &__ebss__ - &__sbss__);

    SystemInit();

    main();
}

void NMI_Handler()
{
    printf("NMI_Handler\n");
    while (1);
}

void HardFault_Handler()
{
    printf("HardFault_Handler\n");
    while (1);
}

void MemManage_Handler()
{
    printf("MemManage_Handler\n");
    while (1);
}

void BusFault_Handler()
{
    printf("BusFault_Handler\n");
    while (1);
}

void UsageFault_Handler()
{
    printf("UsageFault_Handler\n");
    while (1);
}

void SVC_Handler()
{
    printf("SVC_Handler\n");
    /* Put the register image near the top of stack */
    struct regpack *preg = ((struct regpack *)(stk1 + 256)) - 1;
    preg->psr = 0x01000000;
    preg->pc = (unsigned long)thread1;
    psp_set((unsigned long)(stk1 + 256 - 8));
}

void DebugMon_Handler()
{
    printf("DebugMon_Handler\n");
    while (1);
}

void PendSV_Handler()
{
    printf("PendSV_Handler\n");
    while (1);
}

int counter;

void SysTick_Handler()
{
    if (++counter > 1000)
    {
        printf("SysTick_Handler\n");
        counter = 0;
    }
}

void __attribute__((section(".vector_tbl"))) (*(vector_table[]))() =
{
    (void (*)())(0x20000000+8192),
    start, // Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0,
    0,
    0,
    0,
    SVC_Handler,
    DebugMon_Handler,
    0,
    PendSV_Handler,
    SysTick_Handler,
};
