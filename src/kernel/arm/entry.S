
        .syntax unified
        .arch armv7-m

        .section ".text"

        .globl 	start
        .type 	start STT_FUNC
        .thumb_func
start:
        b start_func

        .globl 	SVC_Handler
        .type 	SVC_Handler STT_FUNC
        .thumb_func
SVC_Handler:
        push { lr }
        bl SVC_Handler1
        pop { lr }
        bx lr

        .section     ".vector_tbl"
        .globl        vector_table

vector_table:
        .long	0x20000000 + 8192
        .long   start
        .long   NMI_Handler
        .long   HardFault_Handler
        .long   MemManage_Handler
        .long   BusFault_Handler
        .long   UsageFault_Handler
        .long   0
        .long   0
        .long   0
        .long   0
        .long   SVC_Handler
        .long   DebugMon_Handler
        .long   0
        .long   PendSV_Handler
        .long   SysTick_Handler
