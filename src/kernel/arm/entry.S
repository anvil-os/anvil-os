
        .syntax unified
        .arch armv7-m

        .section    ".text"

/****************************************
 * The system entry point
 ***************************************/
        .globl      start
        .type       start STT_FUNC
        .thumb_func
start:
        b           start_func


/****************************************
 * Handler for SVC. All system calls
 * come through here
 ***************************************/
        .globl      SVC_Handler
        .type       SVC_Handler STT_FUNC
        .thumb_func
SVC_Handler:
        /* Some regs are saved automatically, save the rest now */
        mrs         r0, psp
        stmdb       r0!, {r4-r11}
        msr         psp, r0

        push        {lr}
        bl          syscall
        pop         {lr}

        mrs         r0, psp
        ldmia       r0!, {r4-r11}
        msr         psp, r0

        bx          lr


/****************************************
 * Handler for the system tick
 ***************************************/
        .globl      SysTick_Handler
        .type       SysTick_Handler STT_FUNC
        .thumb_func
SysTick_Handler:
        /* Some regs are saved automatically, save the rest now */
        mrs         r0, psp
        stmdb       r0!, {r4-r11}
        msr         psp, r0

        push        {lr}
        bl          SysTick_Handler1
        pop         {lr}

        mrs         r0, psp
        ldmia       r0!, {r4-r11}
        msr         psp, r0

        bx          lr


/****************************************
 * The vector table in itas own section
 ***************************************/
        .section    ".vector_tbl"

        .globl      vector_table
vector_table:
        .long       0x20000000 + 8192
        .long       start
        .long       NMI_Handler
        .long       HardFault_Handler
        .long       MemManage_Handler
        .long       BusFault_Handler
        .long       UsageFault_Handler
        .long       0
        .long       0
        .long       0
        .long       0
        .long       SVC_Handler
        .long       DebugMon_Handler
        .long       0
        .long       PendSV_Handler
        .long       SysTick_Handler
