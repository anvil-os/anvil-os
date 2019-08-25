
#ifndef ARM_H_INCLUDED
#define ARM_H_INCLUDED

struct regpack
{
    /* These will need to be pushed manually */
    unsigned long r4;
    unsigned long r5;
    unsigned long r6;
    unsigned long r7;
    unsigned long r8;
    unsigned long r9;
    unsigned long r10;
    unsigned long r11;

    /* These 8 are pushed by the processor */
    unsigned long r0;
    unsigned long r1;
    unsigned long r2;
    unsigned long r3;
    unsigned long r12;
    unsigned long lr;
    unsigned long pc;
    unsigned long psr;

} __attribute__ ((packed)) __attribute__ ((aligned(4)));

static __inline__ void psp_set(unsigned long reg)
{
    __asm__ __volatile__ (
            "msr psp, %0\n\t"
            "isb\n\t"
            :
            : "r" (reg)
    );
}

static __inline__ unsigned long psp_get()
{
    unsigned long reg;
    __asm__ __volatile__ (
            "mrs %0, psp\n\t"
            : "=r" (reg)
    );
    return reg;
}

static __inline__ unsigned long msp_get()
{
    unsigned long reg;
    __asm__ __volatile__ (
            "mrs %0, msp\n\t"
            : "=r" (reg)
    );
    return reg;
}

static __inline__ void control_set(unsigned long reg)
{
    __asm__ __volatile__ (
            "msr control, %0\n\t"
            "isb\n\t"
            :
            : "r" (reg)
    );
}

static __inline__ unsigned long control_get()
{
    unsigned long reg;
    __asm__ __volatile__ (
            "mrs %0, control\n\t"
            : "=r" (reg)
    );
    return reg;
}

static __inline__ void svc()
{
    __asm__ __volatile__ (
            "svc #0"
            :
            :
            : "memory"
    );
}

#endif /* ARM_H_INCLUDED */
