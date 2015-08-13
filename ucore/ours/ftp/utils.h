// This utils file is for ethernet only.
#ifndef _H_UTILS_
#define _H_UTILS_ value

#define nop() __asm volatile ("nop")
#define LSB(x) ((x) & 0xFF)
#define MSB(x) (((x) >> 8) & 0xFF)

void delay_ms(int ms);
void delay_us(int us);

int eth_memcmp(int * a, int * b, int length);
void eth_memcpy(int * dst, int * src, int length);

int mem2int(int * data, int length);
void int2mem(int * data, int length, int val);

#endif
