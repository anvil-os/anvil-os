
SECTIONS
{
    /* The image will be identity mapped at the 1MB mark */

	.text 0x100000 : AT ( 0x100000 ) ALIGN (0x1000)
	{
    	*(.boot)
        *(.text)
        *(.rodata)
	    . = ALIGN(4);
    	__etext__ = .;
	}

    .data : ALIGN (0x1000)
    {
		__sdata__ = .;
    	*(.data)
        __edata__ = .;
	} 
    
    .bss : ALIGN (0x1000)
	{
		__sbss__ = .;
    	*(.bss)
        *(COMMON)
        . = ALIGN(4);
		__ebss__ = .;
	}
}
