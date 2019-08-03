
#include "debug.h"

static inline char inportb(int port)
{
	char value;
    __asm__ __volatile__ ( \
            "inb %w1, %b0"
            : "=a"(value)
            : "Nd"(port));
    return value;
}

static inline void outportb(int port, char value)
{
    __asm__ __volatile__ ( \
            "outb %b0, %w1"
            :
            : "a"(value), "Nd"(port));
}

#define BASE    (0x3f8)

/* DLAB=0  */
#define RBR     (BASE+0) /* RO */
#define THR     (BASE+0) /* WO */
#define IER     (BASE+1)
#define IIR     (BASE+2) /* RO */
#define FCR     (BASE+2) /* WO */
#define LCR     (BASE+3)
#define MCR     (BASE+4)
#define LSR     (BASE+5)
#define MSR     (BASE+6)
#define SCR     (BASE+7)

/* DLAB=1  */
#define DLL     (BASE+0)
#define DLM     (BASE+1)

int debug_init()
{
    /* Set up baud rate to 115200 - 0x00 0x01 */
    /* Set DLAB (DIV latch) */
    char c = inportb(LCR);
    outportb(LCR, c | 0x80);
    outportb(DLL, 0x01);
    outportb(DLM, 0x00);
    /* Release DIV latch */
    outportb(LCR, c);

    /* Set the LCR to N81 - 0000 0011 */
    outportb(LCR, 0x03);

    /* Disable all interrupts */
    outportb(IER, 0x0);
}

void debug_putc(int c)
{
	while ((inportb(IIR) & 0x7) == 0x2)
	{
	}
	outportb(THR, c);
}
