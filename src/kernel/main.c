
#include "debug.h"

int main()
{
    debug_init();

	debug_putc('1');
	debug_putc('2');
	debug_putc('3');
	debug_putc('4');

    while (1);
}
