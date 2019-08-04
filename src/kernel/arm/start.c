
#include "debug.h"

extern char __etext__;
extern char __sdata__;
extern char __edata__;
extern char __sbss__;
extern char __ebss__;

int main();
void SystemInit();

void start()
{
    int n;
    char *d, *s;

    /* Copy data from ROM to RAM */
    n = &__edata__ - &__sdata__;
    s = &__etext__;
    d = &__sdata__;
    while (n--)
    {
    	*d++ = *s++;
    }

    /* Clear the BSS */
    n = &__ebss__ - &__sbss__;
    d = &__sbss__;
    while (n--)
    {
    	*d++ = 0;
    }

    SystemInit();

    main();
}

void __attribute__((section(".vector_tbl"))) (*(vector_table[]))() =
{
    (void (*)())(0x20000000+8192),
    start,
};
