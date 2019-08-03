
MEMORY
{
    INTERNAL_ROM (RX)  : ORIGIN = 0x08000000, LENGTH = 128K
    INTERNAL_RAM (RWX) : ORIGIN = 0x20000000, LENGTH = 8K
}

SECTIONS
{
    .text :
    {
		KEEP(*(.vector_tbl))
		*(.text*)
		*(.rodata*)
	    . = ALIGN(4);
    	__etext__ = .;
	} > INTERNAL_ROM

	.data : AT (__etext__)
	{
		__sdata__ = .;
		*(.data*)
        . = ALIGN(4);
        __edata__ = .;
	} > INTERNAL_RAM

	.bss : ALIGN (4)
	{
		__sbss__ = .;
		*(.bss*)
		*(COMMON)
        . = ALIGN(4);
		__ebss__ = .;
	} > INTERNAL_RAM
}
