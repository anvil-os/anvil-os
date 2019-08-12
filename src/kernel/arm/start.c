
#include "debug.h"

#include <string.h>

extern char __etext__;
extern char __sdata__;
extern char __edata__;
extern char __sbss__;
extern char __ebss__;
extern char __erom__;

int main();
void SystemInit();

void start()
{
    /* Copy data from ROM to RAM */
    memcpy(&__sdata__, &__erom__, &__edata__ - &__sdata__);

    /* Clear the BSS */
    memset(&__sbss__, 0, &__ebss__ - &__sbss__);

    SystemInit();

    main();
}

void __attribute__((section(".vector_tbl"))) (*(vector_table[]))() =
{
    (void (*)())(0x20000000+8192),
    start,
};
