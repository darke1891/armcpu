#ifndef _H_UTILS_
#define _H_UTILS_ value

#include "defs.h"

#define nop() __asm volatile ("nop")
#define LSB(x) ((x) & 0xFF)
#define MSB(x) (((x) >> 8) & 0xFF)

void delay_ms(int ms);
void delay_us(int us);

int eth_memcmp(int * a, int * b, int length);
void eth_memcpy(int * dst, int * src, int length);

int mem2int(int * data, int length);
void int2mem(int * data, int length, int val);

uint32_t rand();

#define read_c0_status()	__read_32bit_c0_register($12, 0)
#define read_c0_cause()		__read_32bit_c0_register($13, 0)
#define write_c0_status(val)	__write_32bit_c0_register($12, 0, val)

#define __read_32bit_c0_register(reg, sel)				\
({ uint32_t __res;								\
    if (sel == 0)							\
        __asm__ __volatile__(					\
            "mfc0\t%0, " #reg "\n\t"			\
            : "=r" (__res));				\
    else								\
        __asm__ __volatile__(					\
            ".set\tmips32\n\t"				\
            "mfc0\t%0, " #reg ", " #sel "\n\t"		\
            ".set\tmips0\n\t"				\
            : "=r" (__res));				\
    __res;								\
})

#define __write_32bit_c0_register(reg, sel, value)			\
do {									\
    if (sel == 0)							\
        __asm__ __volatile__(					\
            "mtc0\t%z0, " #reg "\n\t"			\
            : : "Jr" ((unsigned int)(value)));		\
    else								\
        __asm__ __volatile__(					\
            ".set\tmips32\n\t"				\
            "mtc0\t%z0, " #reg ", " #sel "\n\t"	\
            ".set\tmips0"					\
            : : "Jr" ((unsigned int)(value)));		\
} while (0)

#define read_c0_count()		__read_32bit_c0_register($9, 0)

#endif
