/*
* @Author: BlahGeek
* @Date:   2014-06-05
* @Last Modified by:   BlahGeek
* @Last Modified time: 2014-06-05
*/


#include "utils.h"
#include "defs.h"

void delay_ms(int ms) {
	int i, j;
    for(i = 0 ; i < ms ; i += 1)
        for(j = 0 ; j < 446 ; j += 1) nop();
}

void delay_us(int us) {
	int i;
    for(i = 0 ; i < us ; i += 1) {
        nop();nop();nop();
        nop();nop();
    }
}


int eth_memcmp(int * a, int * b, int length) {
	int i;
    for(i = 0 ; i < length ; i += 1)
        if(a[i] != b[i])
            return -1;
    return 0;
}

void eth_memcpy(int * dst, int * src, int length) {
	int i;
    for(i = 0 ; i < length ; i += 1)
        dst[i] = LSB(src[i]);
}


int mem2int(int * data, int length) {
    int ret = 0;
	int i;
    for(i = 0 ; i < length ; i += 1) {
        ret <<= 8;
        ret |= LSB(data[i]);
    }
    return ret;
}

void int2mem(int * data, int length, int val) {
	int i;
    for(i = 0 ; i < length ; i += 1) {
        data[length - i - 1] = LSB(val);
        val >>= 8;
    }
}

uint32_t rand() {
    // see wiki,  multiply-with-carry by George Marsaglia
    static uint32_t m_z, m_w;
    if (!m_z || !m_w) {
        m_z = read_c0_count();
        m_w = ~read_c0_count();
    }
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;  /* 32-bit result */
}
