
.code32

.section ".boot"

        .globl	start
start:
        /*
         * This is where GRUB drops us. We won't assume too much.
         *
         * The entry point address is at physmem 0x100000
         */
        jmp		over_multiboot_header

        .align  4
        .long   0x1badb002  /* multiboot magic number */
        .long   0x00000003  /* tell grub we are an ELF file */
        .long   -(0x1badb002 + 0x00000003)  /* checksum */

over_multiboot_header:
        /* Set up a stack */
        lea     stack + (4*4096), %ecx
        mov     %ecx, %esp

        /* Push the Grub magic number */
        pushl   %eax

        /* Push the mbi pointer */
        pushl   %ebx

        /* Call the c code in init.c */
        cld
        call    main

        /* Save the value that we're passing to the kernel. We do it all in
         * regs because we are switching stacks when we get there
         */

.section ".data"

		.globl	start
stack:
		.fill 	4096, 4, 0
